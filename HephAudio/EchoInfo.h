#pragma once
#include "HephAudioFramework.h"
#include "AudioBuffer.h"
#include <cstdint>

namespace HephAudio
{
	/// <summary>
	/// Holds the necessary information for an echo effect to be applied.
	/// </summary>
	struct EchoInfo
	{
		/// <summary>
		/// The number of times the sound waves will be reflected.
		/// </summary>
		uint32_t reflectionCount{ 0 };
		/// <summary>
		/// The delay (in seconds) between each reflection.
		/// </summary>
		heph_float reflectionDelay_s{ 0.0 };
		/// <summary>
		/// The volume loss between each reflection.
		/// </summary>
		heph_float decayFactor{ 0.0 };
		/// <summary>
		/// The beginning of the audio data which will be used as echo.
		/// Must be between 0 and 1.
		/// </summary>
		heph_float echoStartPosition{ 0.0 };
		/// <summary>
		/// The end of the audio data which will be used as echo.
		/// Must be greater than echoStartPosition and must be between 0 and 1.
		/// </summary>
		heph_float echoEndPosition{ 1.0 };
		/// <summary>
		/// Calculates the given audio buffer's frame count after the echo is applied.
		/// </summary>
		/// <returns>The audio buffer's frame count after the echo is applied.</returns>
		size_t CalculateAudioBufferFrameCount(const AudioBuffer& buffer) const;
	};
}