#pragma once
#include "HephAudioFramework.h"
#include "AudioBuffer.h"
#include "File.h"
#include "AudioFileFormatManager.h"
#include "AudioObject.h"
#include "NativeAudio.h"
#include "IAudioCodec.h"

namespace HephAudio
{
	/// <summary>
	/// Reads the audio data from the file just before rendering.
	/// </summary>
	class AudioStream final
	{
	private:
		static std::vector<AudioStream*> streams;
	private:
		Native::NativeAudio* pNativeAudio;
		HephCommon::File* pFile;
		FileFormats::IAudioFileFormat* pFileFormat;
		Codecs::IAudioCodec* pAudioCodec;
		AudioFormatInfo formatInfo;
		std::shared_ptr<AudioObject> pao;
	public:
		/// <summary>
		/// Creates and initalizes an AudioStream instance.
		/// </summary>
		/// <param name="pNativeAudio">Pointer to the used NativeAudio instance for rendering.</param>
		/// <param name="filePath">Path of the audio file.</param>
		AudioStream(Native::NativeAudio* pNativeAudio, HephCommon::StringBuffer filePath);
		AudioStream(const AudioStream&) = delete;
		AudioStream(AudioStream&& rhs) noexcept;
		~AudioStream();
		AudioStream& operator=(const AudioStream&) = delete;
		AudioStream& operator=(AudioStream&& rhs) noexcept;
		HephCommon::File* GetFile() const noexcept;
		FileFormats::IAudioFileFormat* GetFileFormat() const noexcept;
		Codecs::IAudioCodec* GetAudioCodec() const noexcept;
		std::shared_ptr<AudioObject> GetAudioObject() const noexcept;
		const AudioFormatInfo& GetAudioFormatInfo() const noexcept;
		void Release() noexcept;
	private:
		void Release(bool destroyAO) noexcept;
		static void RemoveStream(AudioStream* pStream) noexcept;
		static AudioStream* FindStream(const AudioObject* pAudioObject);
		static void OnRender(HephCommon::EventArgs* pArgs, HephCommon::EventResult* pResult);
		static void OnFinishedPlaying(HephCommon::EventArgs* pArgs, HephCommon::EventResult* pResult);
	};
}