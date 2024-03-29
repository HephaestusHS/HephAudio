#pragma once
#include "HephAudioShared.h"
#include "Window.h"

namespace HephAudio
{
	class HannWindow final : public Window
	{
	private:
		heph_float N;
	public:
		HannWindow();
		HannWindow(size_t size);
		heph_float operator[](size_t n) const override;
		void SetSize(size_t newSize) override;
	};
}