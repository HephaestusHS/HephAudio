#pragma once
#include "framework.h"
#include "StringBuffer.h"
#include <cinttypes>

namespace HephAudio
{
	enum class AudioDeviceType : uint8_t
	{
		Null = 0,
		Render = 1,
		Capture = 2,
		All = Render | Capture
	};
	/// <summary>
	/// Holds information about an audio device.
	/// </summary>
	struct AudioDevice
	{
		/// <summary>
		/// The unique identifier of the audio device.
		/// </summary>
		StringBuffer id{ "" };
		/// <summary>
		/// The human-readable name of the audio device.
		/// </summary>
		StringBuffer name{ "" };
		/// <summary>
		/// Tells whether the device is for rendering or capturing audio.
		/// </summary>
		AudioDeviceType type{ AudioDeviceType::Render };
		/// <summary>
		/// Tells whether the audio device is the default device.
		/// </summary>
		bool isDefault{ false };
	};
	constexpr AudioDeviceType operator|(const AudioDeviceType& lhs, const AudioDeviceType& rhs)
	{
		return (AudioDeviceType)((uint8_t)lhs | (uint8_t)rhs);
	}
	constexpr AudioDeviceType operator|=(AudioDeviceType& lhs, const AudioDeviceType& rhs)
	{
		return lhs | rhs;
	}
	constexpr AudioDeviceType operator&(const AudioDeviceType& lhs, const AudioDeviceType& rhs)
	{
		return (AudioDeviceType)((uint8_t)lhs & (uint8_t)rhs);
	}
	constexpr AudioDeviceType operator&=(AudioDeviceType& lhs, const AudioDeviceType& rhs)
	{
		return lhs & rhs;
	}
	constexpr AudioDeviceType operator^(const AudioDeviceType& lhs, const AudioDeviceType& rhs)
	{
		return (AudioDeviceType)((uint8_t)lhs ^ (uint8_t)rhs);
	}
	constexpr AudioDeviceType operator^=(AudioDeviceType& lhs, const AudioDeviceType& rhs)
	{
		return lhs ^ rhs;
	}
	constexpr AudioDeviceType operator~(const AudioDeviceType& lhs)
	{
		return (AudioDeviceType)(~(uint8_t)lhs);
	}
}