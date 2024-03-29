#pragma once
#if defined(_WIN32)
#include "HephAudioShared.h"
#include "NativeAudio.h"
#include "WinAudioBase.h"
#include "StringBuffer.h"
#include <mmeapi.h>

namespace HephAudio
{
	namespace Native
	{
		// Uses MMEAPI (waveIn and waveOut)
		class WinAudioMME final : public WinAudioBase
		{
		public:
			using NativeAudio::InitializeRender;
			using NativeAudio::InitializeCapture;
		private:
			static constexpr uint32_t HDR_COUNT = 2;
		private:
			HWAVEOUT hwo;
			HWAVEIN hwi;
			WAVEHDR renderHdrs[HDR_COUNT];
			WAVEHDR captureHdrs[HDR_COUNT];
		public:
			WinAudioMME();
			WinAudioMME(const WinAudioMME&) = delete;
			WinAudioMME& operator=(const WinAudioMME&) = delete;
			~WinAudioMME();
			void SetMasterVolume(heph_float volume) override;
			heph_float GetMasterVolume() const override;
			void InitializeRender(AudioDevice* device, AudioFormatInfo format);
			void StopRendering() override;
			void InitializeCapture(AudioDevice* device, AudioFormatInfo format) override;
			void StopCapturing() override;
		private:
			bool EnumerateAudioDevices() override;
			AudioFormatInfo GetClosestFormat(DWORD dwFormats, const AudioFormatInfo& format) const;
			static size_t CalculateBufferSize(uint32_t byteRate, uint32_t sampleRate);
			static void CALLBACK RenderCallback(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2);
			static void CALLBACK CaptureCallback(HWAVEIN hwi, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2);
			static HephCommon::StringBuffer GetErrorString(MMRESULT mmResult);
		};
	}
}
#endif