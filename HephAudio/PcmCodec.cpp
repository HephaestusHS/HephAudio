#include "PcmCodec.h"
#include "AudioException.h"
#include "AudioFile.h"

namespace HephAudio
{
	namespace Codecs
	{
		uint32_t PcmCodec::Tag() const noexcept
		{
			return WAVE_FORMAT_PCM;
		}
		AudioBuffer PcmCodec::Decode(const EncodedBufferInfo& encodedBufferInfo) const
		{
			AudioBuffer resultBuffer = AudioBuffer(encodedBufferInfo.size_frame, AudioFormatInfo(WAVE_FORMAT_IEEE_FLOAT, encodedBufferInfo.formatInfo.channelCount, sizeof(hephaudio_float) * 8, encodedBufferInfo.formatInfo.sampleRate));

			if (encodedBufferInfo.endian != AudioFile::GetSystemEndian())
			{
				switch (encodedBufferInfo.formatInfo.bitsPerSample)
				{
				case 8:
				{
					for (size_t i = 0; i < encodedBufferInfo.size_frame; i++)
					{
						for (size_t j = 0; j < encodedBufferInfo.formatInfo.channelCount; j++)
						{
							resultBuffer[i][j] = (hephaudio_float)((uint8_t*)encodedBufferInfo.pBuffer)[i * encodedBufferInfo.formatInfo.channelCount + j] / (hephaudio_float)UINT8_MAX;
						}
					}
				}
				break;
				case 16:
				{
					for (size_t i = 0; i < encodedBufferInfo.size_frame; i++)
					{
						for (size_t j = 0; j < encodedBufferInfo.formatInfo.channelCount; j++)
						{
							int16_t pcmSample = ((int16_t*)encodedBufferInfo.pBuffer)[i * encodedBufferInfo.formatInfo.channelCount + j];
							AudioFile::ChangeEndian((uint8_t*)&pcmSample, 2);

							resultBuffer[i][j] = (hephaudio_float)pcmSample / (hephaudio_float)INT16_MAX;
						}
					}
				}
				break;
				case 24:
				{
					for (size_t i = 0; i < encodedBufferInfo.size_frame; i++)
					{
						for (size_t j = 0; j < encodedBufferInfo.formatInfo.channelCount; j++)
						{
							int24 pcmSample = ((int24*)encodedBufferInfo.pBuffer)[i * encodedBufferInfo.formatInfo.channelCount + j];
							AudioFile::ChangeEndian((uint8_t*)&pcmSample, 3);

							resultBuffer[i][j] = (hephaudio_float)pcmSample / (hephaudio_float)INT24_MAX;
						}
					}
				}
				break;
				case 32:
				{
					for (size_t i = 0; i < encodedBufferInfo.size_frame; i++)
					{
						for (size_t j = 0; j < encodedBufferInfo.formatInfo.channelCount; j++)
						{
							int32_t pcmSample = ((int32_t*)encodedBufferInfo.pBuffer)[i * encodedBufferInfo.formatInfo.channelCount + j];
							AudioFile::ChangeEndian((uint8_t*)&pcmSample, 4);

							resultBuffer[i][j] = (hephaudio_float)pcmSample / (hephaudio_float)INT32_MAX;
						}
					}
				}
				break;
				default:
					throw AudioException(E_FAIL, "WavFormat::ReadFile", "Invalid sample size.");
				}
			}
			else
			{
				switch (encodedBufferInfo.formatInfo.bitsPerSample)
				{
				case 8:
				{
					for (size_t i = 0; i < encodedBufferInfo.size_frame; i++)
					{
						for (size_t j = 0; j < encodedBufferInfo.formatInfo.channelCount; j++)
						{
							resultBuffer[i][j] = (hephaudio_float)((uint8_t*)encodedBufferInfo.pBuffer)[i * encodedBufferInfo.formatInfo.channelCount + j] / (hephaudio_float)UINT8_MAX;
						}
					}
				}
				break;
				case 16:
				{
					for (size_t i = 0; i < encodedBufferInfo.size_frame; i++)
					{
						for (size_t j = 0; j < encodedBufferInfo.formatInfo.channelCount; j++)
						{
							resultBuffer[i][j] = (hephaudio_float)((int16_t*)encodedBufferInfo.pBuffer)[i * encodedBufferInfo.formatInfo.channelCount + j] / (hephaudio_float)INT16_MAX;
						}
					}
				}
				break;
				case 24:
				{
					for (size_t i = 0; i < encodedBufferInfo.size_frame; i++)
					{
						for (size_t j = 0; j < encodedBufferInfo.formatInfo.channelCount; j++)
						{
							resultBuffer[i][j] = (hephaudio_float)((int24*)encodedBufferInfo.pBuffer)[i * encodedBufferInfo.formatInfo.channelCount + j] / (hephaudio_float)INT24_MAX;
						}
					}
				}
				break;
				case 32:
				{
					for (size_t i = 0; i < encodedBufferInfo.size_frame; i++)
					{
						for (size_t j = 0; j < encodedBufferInfo.formatInfo.channelCount; j++)
						{
							resultBuffer[i][j] = (hephaudio_float)((int32_t*)encodedBufferInfo.pBuffer)[i * encodedBufferInfo.formatInfo.channelCount + j] / (hephaudio_float)INT32_MAX;
						}
					}
				}
				break;
				default:
					throw AudioException(E_FAIL, "WavFormat::ReadFile", "Invalid sample size.");
				}
			}

			return resultBuffer;
		}
		void PcmCodec::Encode(AudioBuffer& bufferToEncode, EncodedBufferInfo& encodedBufferInfo) const
		{
			AudioBuffer tempBuffer = AudioBuffer(encodedBufferInfo.size_frame, AudioFormatInfo(WAVE_FORMAT_PCM, encodedBufferInfo.formatInfo.channelCount, encodedBufferInfo.formatInfo.bitsPerSample, encodedBufferInfo.formatInfo.sampleRate));

			if (encodedBufferInfo.endian != AudioFile::GetSystemEndian())
			{
				switch (encodedBufferInfo.formatInfo.bitsPerSample)
				{
				case 8:
				{
					for (size_t i = 0; i < encodedBufferInfo.size_frame; i++)
					{
						for (size_t j = 0; j < encodedBufferInfo.formatInfo.channelCount; j++)
						{
							((uint8_t*)tempBuffer.Begin())[i * encodedBufferInfo.formatInfo.channelCount + j] = (bufferToEncode[i][j] * 0.5 + 0.5) * UINT8_MAX;
						}
					}
				}
				break;
				case 16:
				{
					for (size_t i = 0; i < encodedBufferInfo.size_frame; i++)
					{
						for (size_t j = 0; j < encodedBufferInfo.formatInfo.channelCount; j++)
						{
							int16_t pcmSample = bufferToEncode[i][j] * INT16_MAX;
							AudioFile::ChangeEndian((uint8_t*)&pcmSample, 2);

							((int16_t*)tempBuffer.Begin())[i * encodedBufferInfo.formatInfo.channelCount + j] = pcmSample;
						}
					}
				}
				break;
				case 24:
				{
					for (size_t i = 0; i < encodedBufferInfo.size_frame; i++)
					{
						for (size_t j = 0; j < encodedBufferInfo.formatInfo.channelCount; j++)
						{
							int24 pcmSample = bufferToEncode[i][j] * INT24_MAX;
							AudioFile::ChangeEndian((uint8_t*)&pcmSample, 3);

							((int24*)tempBuffer.Begin())[i * encodedBufferInfo.formatInfo.channelCount + j] = pcmSample;
						}
					}
				}
				break;
				case 32:
				{
					for (size_t i = 0; i < encodedBufferInfo.size_frame; i++)
					{
						for (size_t j = 0; j < encodedBufferInfo.formatInfo.channelCount; j++)
						{
							int32_t pcmSample = bufferToEncode[i][j] * INT32_MAX;
							AudioFile::ChangeEndian((uint8_t*)&pcmSample, 4);

							((int32_t*)tempBuffer.Begin())[i * encodedBufferInfo.formatInfo.channelCount + j] = pcmSample;
						}
					}
				}
				break;
				default:
					throw AudioException(E_FAIL, "WavFormat::ReadFile", "Invalid sample size.");
				}
			}
			else
			{
				switch (encodedBufferInfo.formatInfo.bitsPerSample)
				{
				case 8:
				{
					for (size_t i = 0; i < encodedBufferInfo.size_frame; i++)
					{
						for (size_t j = 0; j < encodedBufferInfo.formatInfo.channelCount; j++)
						{
							((uint8_t*)tempBuffer.Begin())[i * encodedBufferInfo.formatInfo.channelCount + j] = (bufferToEncode[i][j] * 0.5 + 0.5) * UINT8_MAX;
						}
					}
				}
				break;
				case 16:
				{
					for (size_t i = 0; i < encodedBufferInfo.size_frame; i++)
					{
						for (size_t j = 0; j < encodedBufferInfo.formatInfo.channelCount; j++)
						{
							((int16_t*)tempBuffer.Begin())[i * encodedBufferInfo.formatInfo.channelCount + j] = bufferToEncode[i][j] * INT16_MAX;
						}
					}
				}
				break;
				case 24:
				{
					for (size_t i = 0; i < encodedBufferInfo.size_frame; i++)
					{
						for (size_t j = 0; j < encodedBufferInfo.formatInfo.channelCount; j++)
						{
							((int24*)tempBuffer.Begin())[i * encodedBufferInfo.formatInfo.channelCount + j] = bufferToEncode[i][j] * INT24_MAX;
						}
					}
				}
				break;
				case 32:
				{
					for (size_t i = 0; i < encodedBufferInfo.size_frame; i++)
					{
						for (size_t j = 0; j < encodedBufferInfo.formatInfo.channelCount; j++)
						{
							((int32_t*)tempBuffer.Begin())[i * encodedBufferInfo.formatInfo.channelCount + j] = bufferToEncode[i][j] * INT32_MAX;
						}
					}
				}
				break;
				default:
					throw AudioException(E_FAIL, "WavFormat::ReadFile", "Invalid sample size.");
				}
			}

			bufferToEncode = std::move(tempBuffer);
		}
	}
}