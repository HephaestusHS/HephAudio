#include "Window.h"

using namespace HephCommon;

namespace HephAudio
{
	Window::Window() : size(0) {}
	Window::Window(size_t size) : size(size) {}
	FloatBuffer Window::GenerateBuffer() const
	{
		FloatBuffer buffer(this->size);
		for (size_t i = 0; i < this->size; i++)
		{
			buffer[i] = (*this)[i];
		}
		return buffer;
	}
	size_t Window::GetSize() const noexcept
	{
		return this->size;
	}
	void Window::SetSize(size_t newSize) noexcept
	{
		this->size = newSize;
	}
}