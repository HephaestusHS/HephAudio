#ifdef __ANDROID__
#include "AndroidAudio.h"
#include "AudioProcessor.h"

#define ANDROIDAUDIO_EXCPT(slres, androidAudio, method, message) if(slres != 0) { RAISE_AUDIO_EXCPT(androidAudio, AudioException(slres, method, message)); throw AudioException(slres, method, message); }
#define ANDROIDAUDIO_RENDER_THREAD_EXCPT(slres, androidAudio, method, message) if(slres != 0) { RAISE_AUDIO_EXCPT(androidAudio, AudioException(slres, method, message)); goto RENDER_EXIT; }
#define ANDROIDAUDIO_CAPTURE_THREAD_EXCPT(slres, androidAudio, method, message) if(slres != 0) { RAISE_AUDIO_EXCPT(androidAudio, AudioException(slres, method, message)); goto CAPTURE_EXIT; }
#define ANDROIDAUDIO_DEVICE_THREAD_EXCPT(slres, androidAudio, method, message) if(slres != 0) { RAISE_AUDIO_EXCPT(androidAudio, AudioException(slres, method, message)); }

namespace HephAudio
{
	namespace Native
	{
		AndroidAudio::AndroidAudio() : INativeAudio()
		{
			if (__ANDROID_API__ < 21)
			{
				throw AudioException(E_FAIL, L"AndroidAudio::AndroidAudio", L"Api level must be 21 or greater.");
			}
			audioEngineObject = nullptr;
			audioEngine = nullptr;
			audioPlayerObject = nullptr;
			audioPlayer = nullptr;
			audioRecorderObject = nullptr;
			audioRecorder = nullptr;
			masterVolume = 1.0;
			renderBufferSize = 0;
			captureBufferSize = 0;
			SLEngineOption engineOption;
			engineOption.feature = SL_ENGINEOPTION_THREADSAFE;
			engineOption.data = SL_BOOLEAN_TRUE;
			SLInterfaceID engineInterfaceIds = SL_IID_ENGINE;
			SLboolean engineInterfaceBools = SL_BOOLEAN_TRUE;
			ANDROIDAUDIO_EXCPT(slCreateEngine(&audioEngineObject, 1, &engineOption, 1, &engineInterfaceIds, &engineInterfaceBools), this, L"AndroidAudio::AndroidAudio", L"An error occurred whilst creating the audio engine object.");
			ANDROIDAUDIO_EXCPT((*audioEngineObject)->Realize(audioEngineObject, SL_BOOLEAN_FALSE), this, L"AndroidAudio::AndroidAudio", L"An error occurred whilst creating the audio engine object.");
			ANDROIDAUDIO_EXCPT((*audioEngineObject)->GetInterface(audioEngineObject, SL_IID_ENGINE, &audioEngine), this, L"AndroidAudio::AndroidAudio", L"An error occurred whilst getting the audio engine interface.");
		}
		AndroidAudio::~AndroidAudio()
		{
			disposing = true;
			JoinRenderThread();
			JoinCaptureThread();
			JoinQueueThreads();
			StopRendering();
			StopCapturing();
			(*audioEngineObject)->Destroy(audioEngineObject);
		}
		void AndroidAudio::SetMasterVolume(double volume)
		{
			masterVolume = volume;
		}
		double AndroidAudio::GetMasterVolume() const
		{
			return masterVolume;
		}
		void AndroidAudio::InitializeRender(AudioDevice* device, AudioFormatInfo format)
		{
			StopRendering();
			SLDataSource dataSource;
			SLDataFormat_PCM pcmFormat = ToSLFormat(format);
			renderFormat = format;
			SLDataLocator_BufferQueue bufferQueueLocator;
			bufferQueueLocator.locatorType = SL_DATALOCATOR_BUFFERQUEUE;
			bufferQueueLocator.numBuffers = 1;
			dataSource.pLocator = &bufferQueueLocator;
			dataSource.pFormat = &pcmFormat;
			renderBufferSize = renderFormat.sampleRate * 8;
			SLDataSink dataSink;
			SLObjectItf outputMixObject;
			SLDataLocator_OutputMix outputMixLocator;
			ANDROIDAUDIO_EXCPT((*audioEngine)->CreateOutputMix(audioEngine, &outputMixObject, 0, nullptr, nullptr), this, L"AndroidAudio::InitializeRender", L"An error occurred whilst creating audio player.");
			ANDROIDAUDIO_EXCPT((*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE), this, L"AndroidAudio::InitializeRender", L"An error occurred whilst creating audio player.");
			outputMixLocator.locatorType = SL_DATALOCATOR_OUTPUTMIX;
			outputMixLocator.outputMix = outputMixObject;
			dataSink.pLocator = &outputMixLocator;
			dataSink.pFormat = &pcmFormat;
			SLboolean audioPlayerBools[2] = { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };
			SLInterfaceID audioPlayerIIDs[2] = { SL_IID_PLAY, SL_IID_BUFFERQUEUE };
			ANDROIDAUDIO_EXCPT((*audioEngine)->CreateAudioPlayer(audioEngine, &audioPlayerObject, &dataSource, &dataSink, 2, audioPlayerIIDs, audioPlayerBools), this, L"AndroidAudio::InitializeRender", L"An error occurred whilst creating audio player.");
			ANDROIDAUDIO_EXCPT((*audioPlayerObject)->Realize(audioPlayerObject, SL_BOOLEAN_FALSE), this, L"AndroidAudio::InitializeRender", L"An error occurred whilst creating audio player.");
			ANDROIDAUDIO_EXCPT((*audioPlayerObject)->GetInterface(audioPlayerObject, SL_IID_PLAY, &audioPlayer), this, L"AndroidAudio::InitializeRender", L"An error occurred whilst creating audio player.");
			SLBufferQueueItf bufferQueue;
			ANDROIDAUDIO_EXCPT((*audioPlayerObject)->GetInterface(audioPlayerObject, SL_IID_BUFFERQUEUE, &bufferQueue), this, L"AndroidAudio::InitializeRender", L"An error occurred whilst creating render buffer.");
			isRenderInitialized = true;
			renderThread = std::thread(&AndroidAudio::RenderData, this, bufferQueue);
		}
		void AndroidAudio::StopRendering()
		{
			if (isRenderInitialized)
			{
				isRenderInitialized = false;
				JoinRenderThread();
				if (audioPlayerObject != nullptr)
				{
					(*audioPlayerObject)->Destroy(audioPlayerObject);
				}
			}
		}
		void AndroidAudio::InitializeCapture(AudioDevice* device, AudioFormatInfo format)
		{
			StopCapturing();
			SLDataLocator_IODevice deviceLocator;
			deviceLocator.locatorType = SL_DATALOCATOR_IODEVICE;
			deviceLocator.deviceType = SL_IODEVICE_AUDIOINPUT;
			deviceLocator.deviceID = SL_DEFAULTDEVICEID_AUDIOINPUT;
			deviceLocator.device = nullptr;
			SLDataSource dataSource;
			dataSource.pLocator = &deviceLocator;
			dataSource.pFormat = NULL;
			SLDataSink dataSink;
			SLDataFormat_PCM pcmFormat = ToSLFormat(format);
			captureFormat = format;
			captureBufferSize = captureFormat.sampleRate * 8;
			void* dataBuffer = malloc(captureBufferSize);
			if (dataBuffer == nullptr)
			{
				RAISE_AUDIO_EXCPT(this, AudioException(E_FAIL, L"AndroidAudio::InitializeCapture", L"Insufficient memory."));
				return;
			}
			SLDataLocator_Address addressLocator;
			addressLocator.locatorType = SL_DATALOCATOR_ADDRESS;
			addressLocator.pAddress = dataBuffer;
			addressLocator.length = captureBufferSize;
			dataSink.pLocator = &addressLocator;
			dataSink.pFormat = &pcmFormat;
			SLboolean audiorecordertrue = SL_BOOLEAN_TRUE;
			ANDROIDAUDIO_EXCPT((*audioEngine)->CreateAudioRecorder(audioEngine, &audioRecorderObject, &dataSource, &dataSink, 1, &SL_IID_RECORD, &audiorecordertrue), this, L"AndroidAudio::InitializeCapture", L"An error occurred whilst initializing capture.");
			ANDROIDAUDIO_EXCPT((*audioRecorderObject)->Realize(audioRecorderObject, SL_BOOLEAN_FALSE), this, L"AndroidAudio::InitializeCapture", L"An error occurred whilst initializing capture.");
			ANDROIDAUDIO_EXCPT((*audioRecorderObject)->GetInterface(audioRecorderObject, SL_IID_RECORD, &audioRecorder), this, L"AndroidAudio::InitializeCapture", L"An error occurred whilst initializing capture.");
			CallbackContext context;
			context.pAndroidAudio = this;
			context.pDataBase = (SLint8*)dataBuffer;
			context.pData = context.pDataBase;
			context.size = captureBufferSize;
			ANDROIDAUDIO_EXCPT((*audioRecorder)->RegisterCallback(audioRecorder, &AndroidAudio::RecordEventCallback, &context), this, L"AndroidAudio::InitializeCapture", L"An error occurred whilst initializing capture.");
			ANDROIDAUDIO_EXCPT((*audioRecorder)->SetPositionUpdatePeriod(audioRecorder, 200), this, L"AndroidAudio::InitializeCapture", L"An error occurred whilst initializing capture."); // update callback every 200 ms.
			ANDROIDAUDIO_EXCPT((*audioRecorder)->SetCallbackEventsMask(audioRecorder, SL_RECORDEVENT_HEADATNEWPOS), this, L"AndroidAudio::InitializeCapture", L"An error occurred whilst initializing capture.");
			captureFormat = format;
			isCaptureInitialized = true;
			captureThread = std::thread(&AndroidAudio::CaptureData, this, dataBuffer);
		}
		void AndroidAudio::StopCapturing()
		{
			if (isCaptureInitialized)
			{
				isCaptureInitialized = false;
				JoinCaptureThread();
				if (audioRecorderObject != nullptr)
				{
					(*audioRecorderObject)->Destroy(audioRecorderObject);
				}
			}
		}
		void AndroidAudio::SetDisplayName(std::wstring displayName)
		{
			RAISE_AUDIO_EXCPT(this, AudioException(E_NOTIMPL, L"AndroidAudio::SetDisplayName", L"AndroidAudio does not support this method."));
		}
		void AndroidAudio::SetIconPath(std::wstring iconPath)
		{
			RAISE_AUDIO_EXCPT(this, AudioException(E_NOTIMPL, L"AndroidAudio::SetIconPath", L"AndroidAudio does not support this method."));
		}
		AudioDevice AndroidAudio::GetDefaultAudioDevice(AudioDeviceType deviceType) const
		{
			RAISE_AUDIO_EXCPT(this, AudioException(E_NOTIMPL, L"AndroidAudio::GetDefaultAudioDevice", L"AndroidAudio does not support this method."));
		}
		std::vector<AudioDevice> AndroidAudio::GetAudioDevices(AudioDeviceType deviceType, bool includeInactive) const
		{
			RAISE_AUDIO_EXCPT(this, AudioException(E_NOTIMPL, L"AndroidAudio::GetAudioDevices", L"AndroidAudio does not support this method."));
		}
		void AndroidAudio::RenderData(SLBufferQueueItf bufferQueue)
		{
			void* audioBuffer = malloc(renderBufferSize);
			if (audioBuffer == nullptr)
			{
				RAISE_AUDIO_EXCPT(this, AudioException(E_FAIL, L"AndroidAudio", L"Insufficient memory."));
				return;
			}
			memset(audioBuffer, 0, renderBufferSize);
			CallbackContext context;
			context.pAndroidAudio = this;
			context.pDataBase = (SLint8*)audioBuffer;
			context.pData = context.pDataBase;
			context.size = renderBufferSize;
			ANDROIDAUDIO_RENDER_THREAD_EXCPT((*bufferQueue)->RegisterCallback(bufferQueue, &AndroidAudio::BufferQueueCallback, &context), this, L"AndroidAudio", L"An error occurred whilst rendering data.");
			ANDROIDAUDIO_RENDER_THREAD_EXCPT((*bufferQueue)->Enqueue(bufferQueue, context.pData, renderBufferSize * 0.01), this, L"AndroidAudio", L"An error occurred whilst rendering data.");
			ANDROIDAUDIO_RENDER_THREAD_EXCPT((*audioPlayer)->SetPlayState(audioPlayer, SL_PLAYSTATE_PLAYING), this, L"AndroidAudio", L"An error occurred whilst rendering data.");
			while (!disposing && isRenderInitialized);
			ANDROIDAUDIO_RENDER_THREAD_EXCPT((*audioPlayer)->SetPlayState(audioPlayer, SL_PLAYSTATE_STOPPED), this, L"AndroidAudio", L"An error occurred whilst rendering data.");
		RENDER_EXIT:
			free(audioBuffer);
		}
		void AndroidAudio::CaptureData(void* dataBuffer)
		{
			ANDROIDAUDIO_CAPTURE_THREAD_EXCPT((*audioRecorder)->SetRecordState(audioRecorder, SL_RECORDSTATE_RECORDING), this, L"AndroidAudio", L"An error occurred whilst capturing data.");
			while (!disposing && isCaptureInitialized);
			ANDROIDAUDIO_CAPTURE_THREAD_EXCPT((*audioRecorder)->SetRecordState(audioRecorder, SL_RECORDSTATE_STOPPED), this, L"AndroidAudio", L"An error occurred whilst capturing data.");
		CAPTURE_EXIT:
			free(dataBuffer);
		}
		double AndroidAudio::GetFinalAOVolume(std::shared_ptr<IAudioObject> audioObject) const
		{
			return INativeAudio::GetFinalAOVolume(audioObject) * masterVolume;
		}
		SLDataFormat_PCM AndroidAudio::ToSLFormat(AudioFormatInfo& formatInfo)
		{
			SLDataFormat_PCM pcmFormat;
			pcmFormat.formatType = SL_DATAFORMAT_PCM;
			pcmFormat.numChannels = formatInfo.channelCount;
			pcmFormat.samplesPerSec = formatInfo.sampleRate * 1000;
			pcmFormat.bitsPerSample = formatInfo.bitsPerSample;
			pcmFormat.containerSize = formatInfo.bitsPerSample;
			pcmFormat.channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
			pcmFormat.endianness = SL_BYTEORDER_LITTLEENDIAN;
			return pcmFormat;
		}
		void AndroidAudio::BufferQueueCallback(SLBufferQueueItf bufferQueue, void* pContext)
		{
			CallbackContext* pCallbackContext = (CallbackContext*)pContext;
			if (pCallbackContext != nullptr)
			{
				const uint32_t renderCallbackFrameCount = pCallbackContext->pAndroidAudio->renderBufferSize * 0.01 / pCallbackContext->pAndroidAudio->renderFormat.FrameSize();
				AudioBuffer dataBuffer(renderCallbackFrameCount, pCallbackContext->pAndroidAudio->renderFormat);
				pCallbackContext->pAndroidAudio->Mix(dataBuffer, renderCallbackFrameCount);
				memcpy(pCallbackContext->pData, dataBuffer.Begin(), dataBuffer.Size());
				SLresult slres = (*bufferQueue)->Enqueue(bufferQueue, pCallbackContext->pData, dataBuffer.Size());
				if (slres != 0)
				{
					RAISE_AUDIO_EXCPT(pCallbackContext->pAndroidAudio, AudioException(E_FAIL, L"AndroidAudio", L"An error occurred whilst rendering data."));
					return;
				}
				pCallbackContext->pData += dataBuffer.Size();
				if (pCallbackContext->pData >= pCallbackContext->pDataBase + pCallbackContext->pAndroidAudio->renderBufferSize)
				{
					pCallbackContext->pData = pCallbackContext->pDataBase;
				}
			}
		}
		void AndroidAudio::RecordEventCallback(SLRecordItf audioRecorder, void* pContext, SLuint32 e)
		{
			CallbackContext* pCallbackContext = (CallbackContext*)pContext;
			if (pCallbackContext != nullptr && pCallbackContext->pAndroidAudio->OnCapture != nullptr)
			{
				const uint32_t captureCallbackSize = pCallbackContext->pAndroidAudio->captureBufferSize * 0.01;
				const uint32_t captureCallbackFrameCount = captureCallbackSize / pCallbackContext->pAndroidAudio->captureFormat.FrameSize();
				AudioBuffer captureBuffer(captureCallbackFrameCount, pCallbackContext->pAndroidAudio->captureFormat);
				memcpy(captureBuffer.Begin(), pCallbackContext->pData, captureCallbackSize);
				AudioProcessor::ConvertPcmToInnerFormat(captureBuffer);
				pCallbackContext->pAndroidAudio->OnCapture(captureBuffer);
				pCallbackContext->pData += captureCallbackSize;
				if (pCallbackContext->pData >= pCallbackContext->pDataBase + pCallbackContext->pAndroidAudio->captureBufferSize - captureCallbackSize)
				{
					pCallbackContext->pData = pCallbackContext->pDataBase;
					SLresult slres = (*audioRecorder)->SetMarkerPosition(audioRecorder, 0);
					if (slres != 0)
					{
						RAISE_AUDIO_EXCPT(pCallbackContext->pAndroidAudio, AudioException(E_FAIL, L"AndroidAudio", L"An error occurred whilst capturing data."));
					}
				}
			}
		}
	}
}
#endif