#include "AiffFormat.h"
#include "HephException.h"
#include "AudioCodecManager.h"
#include "AudioProcessor.h"

#define WAVE_FORMAT_PCM_BIG_ENDIAN (WAVE_FORMAT_PCM << 8)

using namespace HephAudio::Codecs;

namespace HephAudio
{
	namespace FileFormats
	{
		constexpr uint32_t formID = 0x464F524D; // "FORM"
		constexpr uint32_t fverID = 0x46564552; // "FVER"
		constexpr uint32_t aiffID = 0x41494646; // "AIFF"
		constexpr uint32_t aifcID = 0x41494643; // "AIFC"
		constexpr uint32_t commID = 0x434F4D4D; // "COMM"
		constexpr uint32_t ssndID = 0x53534E44; // "SSND"
		constexpr uint32_t aifc_v1 = 0xA2805140;

		HephCommon::StringBuffer AiffFormat::Extension() const
		{
			return ".aiff .aifc .aif";
		}
		AudioFormatInfo AiffFormat::ReadAudioFormatInfo(const HephCommon::File* pAudioFile) const
		{
			AudioFormatInfo formatInfo;
			uint32_t data32, formType, chunkSize;
			uint64_t srBits;

			pAudioFile->Read(&data32, 4, Endian::Big);
			if (data32 != formID)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_fail, "AiffFormat::ReadAudioFormatInfo", "Failed to read the file. File might be corrupted."));
			}
			pAudioFile->IncreaseOffset(4);
			pAudioFile->Read(&formType, 4, Endian::Big);
			if (formType != aiffID && formType != aifcID)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_fail, "AiffFormat::ReadAudioFormatInfo", "Failed to read the file. File might be corrupted."));
			}

			pAudioFile->Read(&data32, 4, Endian::Big);
			while (data32 != commID)
			{
				pAudioFile->Read(&chunkSize, 4, Endian::Big);
				if (chunkSize % 2 == 1)
				{
					chunkSize += 1;
				}
				if (pAudioFile->GetOffset() + chunkSize >= pAudioFile->FileSize())
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_fail, "AiffFormat::ReadAudioFormatInfo", "Failed to read the file. File might be corrupted."));
				}

				pAudioFile->IncreaseOffset(chunkSize);
				pAudioFile->Read(&data32, 4, Endian::Big);
			}
			pAudioFile->Read(&chunkSize, 4, Endian::Big);
			const uint32_t commChunkEnd = pAudioFile->GetOffset() + chunkSize;

			pAudioFile->Read(&formatInfo.channelCount, 2, Endian::Big);
			pAudioFile->IncreaseOffset(4);
			pAudioFile->Read(&formatInfo.bitsPerSample, 2, Endian::Big);
			pAudioFile->Read(&srBits, 8, Endian::Big);
			this->SampleRateFrom64(srBits, formatInfo);
			pAudioFile->IncreaseOffset(2);

			if (formType == aifcID)
			{
				pAudioFile->Read(&data32, 4, Endian::Big);
				this->FormatTagFrom32(data32, formatInfo);
			}
			else
			{
				formatInfo.formatTag = WAVE_FORMAT_PCM;
			}

			pAudioFile->SetOffset(commChunkEnd);

			pAudioFile->Read(&data32, 4, Endian::Big);
			while (data32 != ssndID)
			{
				pAudioFile->Read(&chunkSize, 4, Endian::Big);
				if (chunkSize % 2 == 1)
				{
					chunkSize += 1;
				}
				if (pAudioFile->GetOffset() + chunkSize >= pAudioFile->FileSize())
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_fail, "AiffFormat::ReadAudioFormatInfo", "Failed to read the file. File might be corrupted."));
				}

				pAudioFile->IncreaseOffset(chunkSize);
				pAudioFile->Read(&data32, 4, Endian::Big);
			}

			return formatInfo;
		}
		AudioBuffer AiffFormat::ReadFile(const HephCommon::File* pAudioFile) const
		{
			AudioFormatInfo audioFormatInfo = this->ReadAudioFormatInfo(pAudioFile);
			Endian audioDataEndian = Endian::Big;
			if (audioFormatInfo.formatTag == WAVE_FORMAT_PCM_BIG_ENDIAN)
			{
				audioFormatInfo.formatTag = WAVE_FORMAT_PCM;
				audioDataEndian = Endian::Little;
			}

			IAudioCodec* pAudioCodec = AudioCodecManager::FindCodec(audioFormatInfo.formatTag);
			if (pAudioCodec == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_fail, "AiffFormat::ReadFile", "Unsupported audio codec."));
			}

			uint32_t audioDataSize;
			pAudioFile->Read(&audioDataSize, 4, Endian::Big);
			pAudioFile->IncreaseOffset(8);

			const uint8_t bytesPerSample = audioFormatInfo.bitsPerSample / 8;

			EncodedBufferInfo encodedBufferInfo;
			encodedBufferInfo.pBuffer = malloc(audioDataSize);
			if (encodedBufferInfo.pBuffer == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_insufficient_memory, "AiffFormat::ReadFile", "Insufficient memory."));
			}
			pAudioFile->ReadToBuffer(encodedBufferInfo.pBuffer, bytesPerSample, audioDataSize / bytesPerSample);

			encodedBufferInfo.size_byte = audioDataSize;
			encodedBufferInfo.size_frame = audioDataSize / audioFormatInfo.FrameSize();
			encodedBufferInfo.formatInfo = audioFormatInfo;
			encodedBufferInfo.endian = audioDataEndian;

			const AudioBuffer hephaudioBuffer = pAudioCodec->Decode(encodedBufferInfo);

			free(encodedBufferInfo.pBuffer);

			return hephaudioBuffer;
		}
		bool AiffFormat::SaveToFile(HephCommon::StringBuffer filePath, AudioBuffer& buffer, bool overwrite) const
		{
			try
			{
				const HephCommon::File audioFile = HephCommon::File(filePath, overwrite ? HephCommon::FileOpenMode::WriteOverride : HephCommon::FileOpenMode::Write);
				const AudioFormatInfo& bufferFormatInfo = buffer.FormatInfo();
				uint32_t data32, compressionType;
				HephCommon::StringBuffer compressionName;

				audioFile.Write(&formID, 4, Endian::Big);
				data32 = 4;
				audioFile.Write(&data32, 4, Endian::Big);
				audioFile.Write(&aifcID, 4, Endian::Big);

				audioFile.Write(&fverID, 4, Endian::Big);
				audioFile.Write(&data32, 4, Endian::Big);
				audioFile.Write(&aifc_v1, 4, Endian::Big);

				audioFile.Write(&commID, 4, Endian::Big);
				this->FormatTagTo32(bufferFormatInfo, compressionType, compressionName);
				data32 = 22 + compressionName.Size();
				audioFile.Write(&data32, 4, Endian::Big);

				audioFile.Write(&bufferFormatInfo.channelCount, 2, Endian::Big);

				data32 = buffer.FrameCount();
				audioFile.Write(&data32, 4, Endian::Big);
				audioFile.Write(&bufferFormatInfo.bitsPerSample, 2, Endian::Big);

				uint64_t srBits = this->SampleRateTo64(bufferFormatInfo);
				audioFile.Write(&srBits, 8, Endian::Big);
				data32 = 0;
				audioFile.Write(&data32, 2, Endian::Big);

				audioFile.Write(&compressionType, 4, Endian::Big);
				audioFile.Write(compressionName.Begin(), compressionName.Size(), HephCommon::File::GetSystemEndian());

				audioFile.Write(&ssndID, 4, Endian::Big);
				data32 = buffer.Size() + 8;
				audioFile.Write(&data32, 4, Endian::Big);
				data32 = 0;
				audioFile.Write(&data32, 4, Endian::Big);
				audioFile.Write(&data32, 4, Endian::Big);

				if (HephCommon::File::GetSystemEndian() == Endian::Little)
				{
					AudioProcessor::ChangeEndian(buffer);
				}

				audioFile.WriteToBuffer(buffer.Begin(), bufferFormatInfo.bitsPerSample / 8, buffer.FrameCount() * bufferFormatInfo.channelCount);
			}
			catch (HephCommon::HephException)
			{
				return false;
			}

			return true;
		}
		void AiffFormat::SampleRateFrom64(uint64_t srBits, AudioFormatInfo& formatInfo) const
		{
			if (srBits == 0x400FAC4400000000)
			{
				formatInfo.sampleRate = 88200;
			}
			else if (srBits == 0x400EAC4400000000)
			{
				formatInfo.sampleRate = 44100;
			}
			else if (srBits == 0x400DAC4400000000)
			{
				formatInfo.sampleRate = 22050;
			}
			else if (srBits == 0x400CAC4400000000)
			{
				formatInfo.sampleRate = 11025;
			}
			else if (srBits == 0x400FBB8000000000)
			{
				formatInfo.sampleRate = 96000;
			}
			else if (srBits == 0x400EBB8000000000)
			{
				formatInfo.sampleRate = 48000;
			}
			else if (srBits == 0x400DBB8000000000)
			{
				formatInfo.sampleRate = 24000;
			}
			else if (srBits == 0x400CBB8000000000)
			{
				formatInfo.sampleRate = 12000;
			}
			else if (srBits == 0x400BBB8000000000)
			{
				formatInfo.sampleRate = 6000;
			}
			else if (srBits == 0x400FFA0000000000)
			{
				formatInfo.sampleRate = 128000;
			}
			else if (srBits == 0x400EFA0000000000)
			{
				formatInfo.sampleRate = 64000;
			}
			else if (srBits == 0x400DFA0000000000)
			{
				formatInfo.sampleRate = 32000;
			}
			else if (srBits == 0x400CFA0000000000)
			{
				formatInfo.sampleRate = 16000;
			}
			else if (srBits == 0x400BFA0000000000)
			{
				formatInfo.sampleRate = 8000;
			}
			else if (srBits == 0x400AFA0000000000)
			{
				formatInfo.sampleRate = 4000;
			}
			else if (srBits == 0x4009FA0000000000)
			{
				formatInfo.sampleRate = 2000;
			}
			else if (srBits == 0x4008FA0000000000)
			{
				formatInfo.sampleRate = 1000;
			}
			else
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_fail, "AiffFormat", "Unknown sample rate."));
			}
		}
		uint64_t AiffFormat::SampleRateTo64(const AudioFormatInfo& formatInfo) const
		{
			if (formatInfo.sampleRate == 88200)
			{
				return 0x400FAC4400000000;
			}
			else if (formatInfo.sampleRate == 44100)
			{
				return 0x400EAC4400000000;
			}
			else if (formatInfo.sampleRate == 22050)
			{
				return 0x400DAC4400000000;
			}
			else if (formatInfo.sampleRate == 11025)
			{
				return 0x400CAC4400000000;
			}
			else if (formatInfo.sampleRate == 96000)
			{
				return 0x400FBB8000000000;
			}
			else if (formatInfo.sampleRate == 48000)
			{
				return 0x400EBB8000000000;
			}
			else if (formatInfo.sampleRate == 24000)
			{
				return 0x400DBB8000000000;
			}
			else if (formatInfo.sampleRate == 12000)
			{
				return 0x400CBB8000000000;
			}
			else if (formatInfo.sampleRate == 6000)
			{
				return 0x400BBB8000000000;
			}
			else if (formatInfo.sampleRate == 128000)
			{
				return 0x400FFA0000000000;
			}
			else if (formatInfo.sampleRate == 64000)
			{
				return 0x400EFA0000000000;
			}
			else if (formatInfo.sampleRate == 32000)
			{
				return 0x400DFA0000000000;
			}
			else if (formatInfo.sampleRate == 16000)
			{
				return 0x400CFA0000000000;
			}
			else if (formatInfo.sampleRate == 8000)
			{
				return 0x400BFA0000000000;
			}
			else if (formatInfo.sampleRate == 4000)
			{
				return 0x400AFA0000000000;
			}
			else if (formatInfo.sampleRate == 2000)
			{
				return 0x4009FA0000000000;
			}
			else if (formatInfo.sampleRate == 1000)
			{
				return 0x4008FA0000000000;
			}
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_fail, "AiffFormat", "Unknown sample rate."));
		}
		void AiffFormat::FormatTagFrom32(uint32_t tagBits, AudioFormatInfo& formatInfo) const
		{
			switch (tagBits)
			{
			case 0x4E4F4E45: // "NONE"
				formatInfo.formatTag = WAVE_FORMAT_PCM;
				break;
			case 0x736F7774: // "sowt"
				formatInfo.formatTag = WAVE_FORMAT_PCM_BIG_ENDIAN;
				break;
			case 0x666C3332: // "fl32"
			case 0x666C3634: // "fl64"
			case 0x464C3332: // "FL32"
				formatInfo.formatTag = WAVE_FORMAT_IEEE_FLOAT;
				break;
			case 0x414C4157: // "ALAW"
				formatInfo.sampleRate = 8000;
			case 0x616C6177: // "alaw"
				formatInfo.formatTag = WAVE_FORMAT_ALAW;
				formatInfo.bitsPerSample = 8;
				break;
			case 0x554C4157: // "ULAW"
				formatInfo.sampleRate = 8000;
			case 0x756C6177: // "ulaw"
				formatInfo.formatTag = WAVE_FORMAT_MULAW;
				formatInfo.bitsPerSample = 8;
				break;
			default:
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_fail, "AiffFormat", "Unknown codec."));
			}
		}
		void AiffFormat::FormatTagTo32(const AudioFormatInfo& audioFormatInfo, uint32_t& outTagBits, HephCommon::StringBuffer& outCompressionName) const
		{
			switch (audioFormatInfo.formatTag)
			{
			case WAVE_FORMAT_PCM:
				outTagBits = 0x4E4F4E45; // "NONE"
				outCompressionName = "not compressed";
				break;
			case WAVE_FORMAT_IEEE_FLOAT:
				if (audioFormatInfo.bitsPerSample == 32)
				{
					outTagBits = 0x666C3332; // "fl32"
					outCompressionName = " IEEE 32-bit float";
				}
				else
				{
					outTagBits = 0x666C3634; // "fl64"
					outCompressionName = " IEEE 64-bit float";
				}
				break;
			case WAVE_FORMAT_ALAW:
				outTagBits = 0x616C6177; // "alaw"
				outCompressionName = "ALaw 2:1";
				break;
			case WAVE_FORMAT_MULAW:
				outTagBits = 0x756C6177;
				outCompressionName = "�Law 2:1";
				break;
			default:
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_fail, "AiffFormat", "Codec not supported."));
			}
		}
	}
}