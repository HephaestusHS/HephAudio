#pragma once
#include "HephAudioFramework.h"
#include "Oscillator.h"

namespace HephAudio
{
	class SineWaveOscillator final : public Oscillator
	{
	public:
		SineWaveOscillator(uint32_t sampleRate);
		SineWaveOscillator(heph_float peakAmplitude, heph_float frequency, uint32_t sampleRate, heph_float phase = 0.0, AngleUnit angleUnit = AngleUnit::Radian);
		heph_float Oscillate(size_t t_sample) const noexcept override;
	};
}