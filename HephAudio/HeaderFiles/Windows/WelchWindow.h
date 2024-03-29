#pragma once
#include "HephAudioShared.h"
#include "Window.h"

namespace HephAudio
{
	class WelchWindow final : public Window
	{
	private:
		heph_float hN;
	public:
		WelchWindow();
		WelchWindow(size_t size);
		heph_float operator[](size_t n) const override;
		void SetSize(size_t newSize) override;
	};
}