#pragma once
#include "HephCommonFramework.h"
#include "HephMath.h"

namespace HephCommon
{
	struct Complex final
	{
		heph_float real;
		heph_float imaginary;
		constexpr Complex() : real(0.0f), imaginary(0.0f) {}
		constexpr Complex(heph_float real, heph_float imaginary) : real(real), imaginary(imaginary) {}
		constexpr Complex(const Complex& rhs) : real(rhs.real), imaginary(rhs.imaginary) {}
		constexpr Complex& operator=(const Complex& rhs) noexcept
		{
			this->real = rhs.real;
			this->imaginary = rhs.imaginary;
			return *this;
		}
		constexpr Complex operator+() const noexcept
		{
			return Complex(this->real, this->imaginary);
		}
		constexpr Complex operator-() const noexcept
		{
			return Complex(-this->real, -this->imaginary);
		}
		constexpr Complex operator+(const Complex& rhs) const noexcept
		{
			return Complex(this->real + rhs.real, this->imaginary + rhs.imaginary);
		}
		constexpr Complex& operator+=(const Complex& rhs) noexcept
		{
			this->real += rhs.real;
			this->imaginary += rhs.imaginary;
			return *this;
		}
		constexpr Complex operator-(const Complex& rhs) const noexcept
		{
			return Complex(this->real - rhs.real, this->imaginary - rhs.imaginary);
		}
		constexpr Complex& operator-=(const Complex& rhs) noexcept
		{
			this->real -= rhs.real;
			this->imaginary -= rhs.imaginary;
			return *this;
		}
		constexpr Complex operator*(const Complex& rhs) const noexcept
		{
			return Complex(this->real * rhs.real - this->imaginary * rhs.imaginary, this->imaginary * rhs.real + this->real * rhs.imaginary);
		}
		constexpr Complex& operator*=(const Complex& rhs) noexcept
		{
			const heph_float newReal = this->real * rhs.real - this->imaginary * rhs.imaginary;
			this->imaginary = this->imaginary * rhs.real + this->real * rhs.imaginary;
			this->real = newReal;
			return *this;
		}
		constexpr Complex operator/(const Complex& rhs) const noexcept
		{
			Complex result;
			const heph_float denomiter = rhs.real * rhs.real + rhs.imaginary * rhs.imaginary;
			result.real = (this->real * rhs.real + this->imaginary * rhs.imaginary) / denomiter;
			result.imaginary = (this->imaginary * rhs.real - this->real * rhs.imaginary) / denomiter;
			return result;
		}
		constexpr Complex& operator/=(const Complex& rhs) noexcept
		{
			const heph_float denomiter = rhs.real * rhs.real + rhs.imaginary * rhs.imaginary;
			const heph_float newReal = (this->real * rhs.real + this->imaginary * rhs.imaginary) / denomiter;
			this->imaginary = (this->imaginary * rhs.real - this->real * rhs.imaginary) / denomiter;
			this->real = newReal;
			return *this;
		}
		constexpr bool operator==(const Complex& rhs) const noexcept
		{
			return this->real == rhs.real && this->imaginary == rhs.imaginary;
		}
		constexpr bool operator!=(const Complex& rhs) const noexcept
		{
			return this->real != rhs.real || this->imaginary != rhs.imaginary;
		}
		constexpr Complex operator+(heph_float rhs) const noexcept
		{
			return Complex(this->real + rhs, this->imaginary);
		}
		constexpr Complex& operator+=(heph_float rhs) noexcept
		{
			this->real += rhs;
			return *this;
		}
		constexpr Complex operator-(heph_float rhs) const noexcept
		{
			return Complex(this->real - rhs, this->imaginary);
		}
		constexpr Complex& operator-=(heph_float rhs) noexcept
		{
			this->real -= rhs;
			return *this;
		}
		constexpr Complex operator*(heph_float rhs) const noexcept
		{
			return Complex(this->real * rhs, this->imaginary * rhs);
		}
		constexpr Complex& operator*=(heph_float rhs) noexcept
		{
			this->real *= rhs;
			this->imaginary *= rhs;
			return *this;
		}
		constexpr Complex operator/(heph_float rhs) const noexcept
		{
			return Complex(this->real / rhs, this->imaginary / rhs);
		}
		constexpr Complex& operator/=(heph_float rhs) noexcept
		{
			this->real /= rhs;
			this->imaginary /= rhs;
			return *this;
		}
		constexpr Complex Conjugate() const noexcept
		{
			return Complex(this->real, -this->imaginary);
		}
		constexpr heph_float MagnitudeSquared() const noexcept
		{
			return this->real * this->real + this->imaginary * this->imaginary;
		}
		heph_float Magnitude() const noexcept
		{
			return std::sqrt(this->MagnitudeSquared());
		}
		heph_float Phase() const noexcept
		{
			return std::atan2(this->imaginary, this->real);
		}
	};
}
inline heph_float abs(const HephCommon::Complex& rhs) noexcept
{
	return rhs.Magnitude();
}
inline heph_float phase(const HephCommon::Complex& rhs) noexcept
{
	return rhs.Phase();
}
inline constexpr HephCommon::Complex operator+(heph_float lhs, const HephCommon::Complex& rhs) noexcept
{
	return rhs + lhs;
}
inline constexpr HephCommon::Complex operator-(heph_float lhs, const HephCommon::Complex& rhs) noexcept
{
	return HephCommon::Complex(lhs - rhs.real, -rhs.imaginary);
}
inline constexpr HephCommon::Complex operator*(heph_float lhs, const HephCommon::Complex& rhs) noexcept
{
	return rhs * lhs;
}
inline HEPH_CONSTEVAL HephCommon::Complex operator""_j(unsigned long long int x) noexcept
{
	return HephCommon::Complex(0, x);
}
inline HEPH_CONSTEVAL HephCommon::Complex operator""_j(long double x) noexcept
{
	return HephCommon::Complex(0, x);
}
inline HEPH_CONSTEVAL HephCommon::Complex operator""_J(unsigned long long int x) noexcept
{
	return HephCommon::Complex(0, x);
}
inline HEPH_CONSTEVAL HephCommon::Complex operator""_J(long double x) noexcept
{
	return HephCommon::Complex(0, x);
}