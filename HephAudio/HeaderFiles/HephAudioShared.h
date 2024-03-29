#pragma once
#include "HephCommonShared.h"
#include <cmath>

#if !defined(HEPHAUDIO_FORMAT_TAG_PCM)

#define HEPHAUDIO_FORMAT_TAG_PCM 0x0001
#define HEPHAUDIO_FORMAT_TAG_IEEE_FLOAT 0x0003
#define HEPHAUDIO_FORMAT_TAG_ALAW 0x0006
#define HEPHAUDIO_FORMAT_TAG_MULAW 0x0007
#define HEPHAUDIO_FORMAT_TAG_WMA 0x0161
#define HEPHAUDIO_FORMAT_TAG_WMAUDIO_LOSSLESS 0x0163
#define HEPHAUDIO_FORMAT_TAG_WMAVOICE 0x000A
#define HEPHAUDIO_FORMAT_TAG_MPEG 0x0050
#define HEPHAUDIO_FORMAT_TAG_MP3 0x0055
#define HEPHAUDIO_FORMAT_TAG_AAC 0xA106
#define HEPHAUDIO_FORMAT_TAG_ALAC 0x6C61
#define HEPHAUDIO_FORMAT_TAG_FLAC 0xF1AC
#define HEPHAUDIO_FORMAT_TAG_OPUS 0x704F
#define HEPHAUDIO_FORMAT_TAG_VORBIS 0x674F
#define HEPHAUDIO_FORMAT_TAG_EXTENSIBLE 0xFFFE

#endif

#if !defined(HEPHAUDIO_STOPWATCH_START)

#ifdef HEPHAUDIO_INFO_LOGGING
#define HEPHAUDIO_STOPWATCH_START HephCommon::StopWatch::StaticStart()
#define HEPHAUDIO_STOPWATCH_RESET HephCommon::StopWatch::StaticReset()
#define HEPHAUDIO_STOPWATCH_DT(prefix) HephCommon::StopWatch::StaticDeltaTime(prefix)
#define HEPHAUDIO_STOPWATCH_STOP HephCommon::StopWatch::StaticStop()
#define HEPHAUDIO_LOG(logMessage, logType) HephCommon::ConsoleLogger::Log(logMessage, logType, "HephAudio")
#else
#define HEPHAUDIO_STOPWATCH_START
#define HEPHAUDIO_STOPWATCH_RESET
#define HEPHAUDIO_STOPWATCH_DT(prefix)
#define HEPHAUDIO_STOPWATCH_STOP
#define HEPHAUDIO_LOG(logMessage, logType)
#endif

#endif

#if !defined(HEPHAUDIO_SPEED_OF_SOUND)
#define HEPHAUDIO_SPEED_OF_SOUND 343.0 // speed of sound in dry air at 20�C in meters per second
#endif

#if !defined(INT24_MAX)

#define UINT24_MAX 16777215
#define INT24_MIN (-8388608)
#define INT24_MAX 8388607

#endif

#if !defined(HEPH_AUDIO_SAMPLE)

#if defined(HEPHAUDIO_SAMPLE_TYPE_DBL)
typedef double heph_audio_sample;
#define HEPH_AUDIO_SAMPLE_MIN -1.0
#define HEPH_AUDIO_SAMPLE_MAX 1.0
#define HEPHAUDIO_FORMAT_TAG_HEPHAUDIO_INTERNAL HEPHAUDIO_FORMAT_TAG_IEEE_FLOAT
#elif defined(HEPHAUDIO_SAMPLE_TYPE_FLT)
typedef float heph_audio_sample;
#define HEPH_AUDIO_SAMPLE_MIN -1.0f
#define HEPH_AUDIO_SAMPLE_MAX 1.0f
#define HEPHAUDIO_FORMAT_TAG_HEPHAUDIO_INTERNAL HEPHAUDIO_FORMAT_TAG_IEEE_FLOAT
#elif defined(HEPHAUDIO_SAMPLE_TYPE_S64)
typedef int64_t heph_audio_sample;
#define HEPH_AUDIO_SAMPLE_MIN INT64_MIN
#define HEPH_AUDIO_SAMPLE_MAX INT64_MAX
#define HEPHAUDIO_FORMAT_TAG_HEPHAUDIO_INTERNAL HEPHAUDIO_FORMAT_TAG_PCM
#elif defined(HEPHAUDIO_SAMPLE_TYPE_S32)
typedef int32_t heph_audio_sample;
#define HEPH_AUDIO_SAMPLE_MIN INT32_MIN
#define HEPH_AUDIO_SAMPLE_MAX INT32_MAX
#define HEPHAUDIO_FORMAT_TAG_HEPHAUDIO_INTERNAL HEPHAUDIO_FORMAT_TAG_PCM
#elif defined(HEPHAUDIO_SAMPLE_TYPE_S16)
typedef int16_t heph_audio_sample;
#define HEPH_AUDIO_SAMPLE_MIN INT16_MIN
#define HEPH_AUDIO_SAMPLE_MAX INT16_MAX
#define HEPHAUDIO_FORMAT_TAG_HEPHAUDIO_INTERNAL HEPHAUDIO_FORMAT_TAG_PCM
#else
typedef float heph_audio_sample;
#define HEPH_AUDIO_SAMPLE_MIN -1.0f
#define HEPH_AUDIO_SAMPLE_MAX 1.0f
#define HEPHAUDIO_FORMAT_TAG_HEPHAUDIO_INTERNAL HEPHAUDIO_FORMAT_TAG_IEEE_FLOAT
#endif

#define HEPH_AUDIO_SAMPLE heph_audio_sample

#if HEPHAUDIO_FORMAT_TAG_HEPHAUDIO_INTERNAL == HEPHAUDIO_FORMAT_TAG_PCM

#define HEPH_AUDIO_SAMPLE_TO_IEEE_FLT(sample) (((double)sample) / (-((double)HEPH_AUDIO_SAMPLE_MIN)))
#define HEPH_AUDIO_SAMPLE_FROM_IEEE_FLT(fltSample) ((heph_audio_sample)((fltSample) * (-((double)HEPH_AUDIO_SAMPLE_MIN))))

#elif HEPHAUDIO_FORMAT_TAG_HEPHAUDIO_INTERNAL == HEPHAUDIO_FORMAT_TAG_IEEE_FLOAT

#define HEPH_AUDIO_SAMPLE_TO_IEEE_FLT(sample) sample
#define HEPH_AUDIO_SAMPLE_FROM_IEEE_FLT(fltSample) fltSample

#else
#error heph_audio_sample/ieee_float conversions are not implemented for this internal format yet.
#endif

#endif

#pragma region Helper Methods
namespace HephAudio
{
	inline heph_float DecibelToGain(heph_float decibel)
	{
		return pow(10.0, decibel * 0.05);
	}
	inline heph_float GainToDecibel(heph_float gain)
	{
		return gain == 0 ? -120.0 : 20.0 * log10(abs(gain));
	}
	inline constexpr heph_float SemitoneToCent(heph_float semitone)
	{
		return semitone * 100.0;
	}
	inline constexpr heph_float SemitoneToOctave(heph_float semitone)
	{
		return semitone / 12.0;
	}
	inline constexpr heph_float CentToSemitone(heph_float cent)
	{
		return cent / 100.0;
	}
	inline constexpr heph_float CentToOctave(heph_float cent)
	{
		return cent / 1200.0;
	}
	inline constexpr heph_float OctaveToSemitone(heph_float octave)
	{
		return octave * 12.0;
	}
	inline constexpr heph_float OctaveToCent(heph_float octave)
	{
		return octave * 1200.0;
	}
}
#pragma endregion