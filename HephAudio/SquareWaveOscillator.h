#pragma once
#include "framework.h"
#include "Oscillator.h"

namespace HephAudio
{
	class SquareWaveOscillator final : public Oscillator
	{
	public:
		SquareWaveOscillator(uint32_t sampleRate);
		SquareWaveOscillator(hephaudio_float peakAmplitude, hephaudio_float frequency, uint32_t sampleRate, hephaudio_float phase = 0.0, AngleUnit angleUnit = AngleUnit::Radian);
		hephaudio_float Oscillate(size_t t_sample) const noexcept override;
	};
}