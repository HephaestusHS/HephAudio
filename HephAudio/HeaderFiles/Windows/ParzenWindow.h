#pragma once
#include "HephAudioShared.h"
#include "Window.h"

namespace HephAudio
{
	class ParzenWindow final : public Window
	{
	private:
		heph_float hN;
		heph_float hL;
		heph_float qL;
	public:
		ParzenWindow();
		ParzenWindow(size_t size);
		heph_float operator[](size_t n) const override;
		void SetSize(size_t newSize) override;
	};
}