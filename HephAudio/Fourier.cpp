#include "Fourier.h"

using namespace HephCommon;

namespace HephAudio
{
	ComplexBuffer Fourier::FFT_Forward(const FloatBuffer& floatBuffer)
	{
		return FFT_Forward(floatBuffer, floatBuffer.FrameCount());
	}
	ComplexBuffer Fourier::FFT_Forward(const FloatBuffer& floatBuffer, size_t fftSize)
	{
		ComplexBuffer complexBuffer = ComplexBuffer(fftSize);
		for (size_t i = 0; i < floatBuffer.FrameCount(); i++)
		{
			complexBuffer[i].real = floatBuffer[i];
		}
		FFT(complexBuffer, fftSize, true);
		return complexBuffer;
	}
	void Fourier::FFT_Forward(ComplexBuffer& complexBuffer)
	{
		Fourier::FFT_Forward(complexBuffer, complexBuffer.FrameCount());
	}
	void Fourier::FFT_Forward(ComplexBuffer& complexBuffer, size_t fftSize)
	{
		complexBuffer.Resize(fftSize);
		FFT(complexBuffer, fftSize, true);
	}
	void Fourier::FFT_Inverse(FloatBuffer& floatBuffer, ComplexBuffer& complexBuffer)
	{
		FFT(complexBuffer, complexBuffer.FrameCount(), false);
		for (size_t i = 0; i < floatBuffer.FrameCount(); i++)
		{
			complexBuffer[i] /= complexBuffer.FrameCount();
			floatBuffer[i] = complexBuffer[i].real;
		}
	}
	void Fourier::FFT_Inverse(ComplexBuffer& complexBuffer, bool scale)
	{
		FFT(complexBuffer, complexBuffer.FrameCount(), false);
		if (scale)
		{
			complexBuffer /= complexBuffer.FrameCount();
		}
	}
	heph_float Fourier::BinFrequencyToIndex(size_t sampleRate, size_t fftSize, heph_float frequency)
	{
		return round(frequency * fftSize / sampleRate);
	}
	heph_float Fourier::IndexToBinFrequency(size_t sampleRate, size_t fftSize, size_t index)
	{
		return (heph_float)index * sampleRate / fftSize;
	}
	size_t Fourier::CalculateFFTSize(size_t bufferSize)
	{
		if (!(bufferSize > 0 && !(bufferSize & (bufferSize - 1)))) // if not power of 2
		{
			return 1 << (size_t)ceil(log2(bufferSize)); // smallest power of 2 thats greater than nSample
		}
		return bufferSize;
	}
	void Fourier::ReverseBits(ComplexBuffer& complexBuffer, size_t fftSize)
	{
		size_t j = 0;
		for (size_t i = 0; i < fftSize; i++)
		{
			if (i < j)
			{
				const Complex temp = complexBuffer[j];
				complexBuffer[j] = complexBuffer[i];
				complexBuffer[i] = temp;
			}
			j ^= fftSize - fftSize / ((i ^ (i + 1)) + 1);
		}
	}
	void Fourier::FFT(ComplexBuffer& complexBuffer, size_t fftSize, bool isForward)
	{
		ReverseBits(complexBuffer, fftSize);
		const size_t p = log2(fftSize);
		Complex a = Complex(-1.0hf, 0.0hf);
		for (size_t i = 0; i < p; i++)
		{
			const size_t s = (1 << i);
			const size_t s2 = s << 1;
			Complex b = Complex(1.0hf, 0.0hf);
			for (size_t j = 0; j < s; j++)
			{
				for (size_t k = j; k < fftSize; k += s2)
				{
					const Complex temp = b * complexBuffer[k + s];
					complexBuffer[k + s] = complexBuffer[k] - temp;
					complexBuffer[k] += temp;
				}
				b *= a;
			}
			a.imaginary = isForward ? -sqrt((1.0hf - a.real) * 0.5hf) : sqrt((1.0hf - a.real) * 0.5hf);
			a.real = sqrt((1.0hf + a.real) * 0.5hf);
		}
	}
}