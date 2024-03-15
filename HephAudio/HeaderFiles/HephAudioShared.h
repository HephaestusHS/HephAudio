#pragma once
#include "../HephCommon/HeaderFiles/HephCommonShared.h"
#include <cmath>

#ifndef HEPHAUDIO_FORMAT_TAG_PCM
#define HEPHAUDIO_FORMAT_TAG_PCM 0x0001
#define HEPHAUDIO_FORMAT_TAG_IEEE_FLOAT 0x0003
#define HEPHAUDIO_FORMAT_TAG_ALAW 0x0006
#define HEPHAUDIO_FORMAT_TAG_MULAW 0x0007
#define HEPHAUDIO_FORMAT_TAG_QUALCOMM_PUREVOICE 0x0150
#define HEPHAUDIO_FORMAT_TAG_WMAUDIO2 0x0161
#define HEPHAUDIO_FORMAT_TAG_WMAUDIO3 0x0162
#define HEPHAUDIO_FORMAT_TAG_WMAUDIO_LOSSLESS 0x0163
#define HEPHAUDIO_FORMAT_TAG_WMAVOICE9 0x000A
#define HEPHAUDIO_FORMAT_TAG_WMAVOICE10 0x000B
#define HEPHAUDIO_FORMAT_TAG_MPEG 0x0050
#define HEPHAUDIO_FORMAT_TAG_MPEGLAYER3 0x0055
#define HEPHAUDIO_FORMAT_TAG_ALAC 0x6C61
#define HEPHAUDIO_FORMAT_TAG_OPUS 0x704F
#define HEPHAUDIO_FORMAT_TAG_VORBIS 0x674F
#define HEPHAUDIO_FORMAT_TAG_MPEG4_AAC 0xA106
#define HEPHAUDIO_FORMAT_TAG_FLAC 0xF1AC
#define HEPHAUDIO_FORMAT_TAG_EXTENSIBLE 0xFFFE
#endif

#ifndef HEPHAUDIO_STOPWATCH_START
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

#ifndef HEPHAUDIO_SPEED_OF_SOUND
#define HEPHAUDIO_SPEED_OF_SOUND 343.0 // speed of sound in dry air at 20�C in meters per second
#endif

#ifndef HEPH_AUDIO_SAMPLE

#if defined(HEPHAUDIO_SAMPLE_TYPE_FLOAT64)
typedef double heph_audio_sample;
#define HEPH_AUDIO_SAMPLE_MIN -1.0
#define HEPH_AUDIO_SAMPLE_MAX 1.0
#define HEPHAUDIO_FORMAT_TAG_HEPHAUDIO_INTERNAL HEPHAUDIO_FORMAT_TAG_IEEE_FLOAT
#elif defined(HEPHAUDIO_SAMPLE_TYPE_FLOAT32)
typedef float heph_audio_sample;
#define HEPH_AUDIO_SAMPLE_MIN -1.0f
#define HEPH_AUDIO_SAMPLE_MAX 1.0f
#define HEPHAUDIO_FORMAT_TAG_HEPHAUDIO_INTERNAL HEPHAUDIO_FORMAT_TAG_IEEE_FLOAT
#elif defined(HEPHAUDIO_SAMPLE_TYPE_HEPH_FLOAT)
typedef heph_float heph_audio_sample;
#define HEPH_AUDIO_SAMPLE_MIN -1.0_hf
#define HEPH_AUDIO_SAMPLE_MAX 1.0_hf
#define HEPHAUDIO_FORMAT_TAG_HEPHAUDIO_INTERNAL HEPHAUDIO_FORMAT_TAG_IEEE_FLOAT
#else
typedef float heph_audio_sample;
#define HEPH_AUDIO_SAMPLE_MIN -1.0f
#define HEPH_AUDIO_SAMPLE_MAX 1.0f
#define HEPHAUDIO_FORMAT_TAG_HEPHAUDIO_INTERNAL HEPHAUDIO_FORMAT_TAG_IEEE_FLOAT
#endif

#define HEPH_AUDIO_SAMPLE heph_audio_sample
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