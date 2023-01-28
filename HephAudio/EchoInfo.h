#pragma once
#include "framework.h"
#include "AudioBuffer.h"
#include <cstdint>

namespace HephAudio
{
	struct EchoInfo
	{
		uint32_t reflectionCount;
		/// <summary>
		/// The delay (in seconds) between each reflection.
		/// </summary>
		double reflectionDelay;
		/// <summary>
		/// The volume loss between each reflaction.
		/// </summary>
		double volumeFactor;
		/// <summary>
		/// The beginning of the audio data which will be used as echo.
		/// Must be between 0 and 1.
		/// </summary>
		double echoStartPosition;
		/// <summary>
		/// The end of the audio data which will be used as echo.
		/// Must be greater than echoStartPosition and must be between 0 and 1.
		/// </summary>
		double echoEndPosition;
		EchoInfo();
		virtual ~EchoInfo() = default;
		/// <summary>
		/// Calculates the given audio buffer's frame count after the echo is applied.
		/// </summary>
		/// <returns>The audio buffer's frame count after echo is applied</returns>
		virtual size_t CalculateAudioBufferFrameCount(const AudioBuffer& buffer) const;
	};
}