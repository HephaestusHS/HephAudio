#pragma once
#include "HephAudioShared.h"
#include "Window.h"

namespace HephAudio
{
	class ExactBlackmanWindow final : public Window
	{
	private:
		heph_float N;
	public:
		ExactBlackmanWindow();
		ExactBlackmanWindow(size_t size);
		heph_float operator[](size_t n) const override;
		void SetSize(size_t newSize) override;
	};
}