#pragma once
#ifdef __ANDROID__
#include "framework.h"
#include "AndroidAudioBase.h"
#include <aaudio/AAudio.h>

namespace HephAudio
{
	namespace Native
	{
		// Uses AAudio, min api target = 27. Use AndroidAudioSLES for api level 26 or lower (min 16) (uses OpenSL ES).
		class AndroidAudioA : public AndroidAudioBase
		{
		private:
			AAudioStream* pRenderStream;
			AAudioStream* pCaptureStream;
			size_t renderBufferFrameCount;
			size_t captureBufferFrameCount;
			hephaudio_float masterVolume;
		public:
			AndroidAudioA(JavaVM* jvm);
			AndroidAudioA(const AndroidAudioA&) = delete;
			AndroidAudioA& operator=(const AndroidAudioA&) = delete;
			virtual ~AndroidAudioA();
			virtual void SetMasterVolume(hephaudio_float volume) override;
			virtual hephaudio_float GetMasterVolume() const override;
			virtual void InitializeRender(AudioDevice* device, AudioFormatInfo format) override;
			virtual void StopRendering() override;
			virtual void InitializeCapture(AudioDevice* device, AudioFormatInfo format) override;
			virtual void StopCapturing() override;
			virtual void SetDisplayName(StringBuffer displayName) override;
			virtual void SetIconPath(StringBuffer iconPath) override;
		protected:
			virtual void RenderData();
			virtual void CaptureData();
			virtual hephaudio_float GetFinalAOVolume(std::shared_ptr<AudioObject> pAudioObject) const override;
		};
	}
}
#endif