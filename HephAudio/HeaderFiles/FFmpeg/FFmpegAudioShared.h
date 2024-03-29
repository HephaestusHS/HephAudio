#pragma once
#if defined(HEPHAUDIO_USE_FFMPEG)
#include "HephAudioShared.h"
#include "AudioFormatInfo.h"
#include "StringBuffer.h"

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
#include <libswresample/swresample.h>
}

#if defined(_WIN32) && defined(_MSVC_LANG)
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "swresample.lib")
#endif


#if LIBAVFORMAT_VERSION_MAJOR < 60
#error unsupported libavformat version.
#endif

#if LIBAVCODEC_VERSION_MAJOR < 60
#error unsupported libavcodec version.
#endif

#if LIBAVUTIL_VERSION_MAJOR < 58
#error unsupported libavutil version.
#endif

#if LIBSWRESAMPLE_VERSION_MAJOR < 4
#error unsupported libswresample version.
#endif



#if !defined(HEPHAUDIO_INTERNAL_SAMPLE_FMT)

namespace HephAudio
{
	inline HephCommon::StringBuffer FFmpegGetErrorMessage(int errorCode)
	{
		char errorMessage[AV_ERROR_MAX_STRING_SIZE]{ };
		if (av_strerror(errorCode, errorMessage, AV_ERROR_MAX_STRING_SIZE) < 0)
		{
			return "error message not found.";
		}
		return errorMessage;
	}

	inline AVCodecID CodecIdFromAudioFormatInfo(const AudioFormatInfo& audioFormatInfo)
	{
		switch (audioFormatInfo.formatTag)
		{

		case HEPHAUDIO_FORMAT_TAG_PCM:
		{
			if (audioFormatInfo.endian == HephCommon::Endian::Little)
			{
				switch (audioFormatInfo.bitsPerSample)
				{
				case 8:
					return AV_CODEC_ID_PCM_U8;
				case 16:
					return AV_CODEC_ID_PCM_S16LE;
				case 24:
					return AV_CODEC_ID_PCM_S24LE;
				case 32:
					return AV_CODEC_ID_PCM_S32LE;
				case 64:
					return AV_CODEC_ID_PCM_S64LE;
				default:
					return AV_CODEC_ID_NONE;
				}
			}
			else
			{
				switch (audioFormatInfo.bitsPerSample)
				{
				case 8:
					return AV_CODEC_ID_PCM_U8;
				case 16:
					return AV_CODEC_ID_PCM_S16BE;
				case 24:
					return AV_CODEC_ID_PCM_S24BE;
				case 32:
					return AV_CODEC_ID_PCM_S32BE;
				case 64:
					return AV_CODEC_ID_PCM_S64BE;
				default:
					return AV_CODEC_ID_NONE;
				}
			}
		}

		case HEPHAUDIO_FORMAT_TAG_IEEE_FLOAT:
		{
			if (audioFormatInfo.endian == HephCommon::Endian::Little)
			{
				return audioFormatInfo.bitsPerSample == sizeof(double) ? AV_CODEC_ID_PCM_F64LE : AV_CODEC_ID_PCM_F32LE;
			}
			return audioFormatInfo.bitsPerSample == sizeof(double) ? AV_CODEC_ID_PCM_F64BE : AV_CODEC_ID_PCM_F32BE;
		}

		case HEPHAUDIO_FORMAT_TAG_ALAW:
			return AV_CODEC_ID_PCM_ALAW;

		case HEPHAUDIO_FORMAT_TAG_MULAW:
			return AV_CODEC_ID_PCM_MULAW;

		case HEPHAUDIO_FORMAT_TAG_WMA:
			return AV_CODEC_ID_WMAV2;

		case HEPHAUDIO_FORMAT_TAG_WMAUDIO_LOSSLESS:
			return AV_CODEC_ID_WMALOSSLESS;

		case HEPHAUDIO_FORMAT_TAG_WMAVOICE:
			return AV_CODEC_ID_WMAVOICE;

		case HEPHAUDIO_FORMAT_TAG_MPEG:
			return AV_CODEC_ID_MP2;

		case HEPHAUDIO_FORMAT_TAG_MP3:
			return AV_CODEC_ID_MP3;

		case HEPHAUDIO_FORMAT_TAG_AAC:
			return AV_CODEC_ID_AAC;

		case HEPHAUDIO_FORMAT_TAG_ALAC:
			return AV_CODEC_ID_ALAC;

		case HEPHAUDIO_FORMAT_TAG_FLAC:
			return AV_CODEC_ID_FLAC;

		case HEPHAUDIO_FORMAT_TAG_OPUS:
			return AV_CODEC_ID_OPUS;

		case HEPHAUDIO_FORMAT_TAG_VORBIS:
			return AV_CODEC_ID_VORBIS;

		default:
			return AV_CODEC_ID_NONE;
		}
	}
}

#define HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(errorCode) HephAudio::FFmpegGetErrorMessage(errorCode)


#if defined(HEPHAUDIO_SAMPLE_TYPE_DBL)
#define HEPHAUDIO_FFMPEG_INTERNAL_SAMPLE_FMT AV_SAMPLE_FMT_DBL
#elif defined(HEPHAUDIO_SAMPLE_TYPE_FLT)
#define HEPHAUDIO_FFMPEG_INTERNAL_SAMPLE_FMT AV_SAMPLE_FMT_FLT
#elif defined(HEPHAUDIO_SAMPLE_TYPE_S64)
#define HEPHAUDIO_FFMPEG_INTERNAL_SAMPLE_FMT AV_SAMPLE_FMT_S64
#elif defined(HEPHAUDIO_SAMPLE_TYPE_S32)
#define HEPHAUDIO_FFMPEG_INTERNAL_SAMPLE_FMT AV_SAMPLE_FMT_S32
#elif defined(HEPHAUDIO_SAMPLE_TYPE_S16)
#define HEPHAUDIO_FFMPEG_INTERNAL_SAMPLE_FMT AV_SAMPLE_FMT_S16
#else
#define HEPHAUDIO_FFMPEG_INTERNAL_SAMPLE_FMT AV_SAMPLE_FMT_FLT
#endif

#endif

#endif