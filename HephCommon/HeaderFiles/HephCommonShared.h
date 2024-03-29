#pragma once
#include <cinttypes>

#if !defined(CPP_VERSION)

#if defined(_MSVC_LANG)

#define CPP_VERSION _MSVC_LANG

#else

#define CPP_VERSION __cplusplus

#endif

#define CPP_VERSION_PRE_98 1L
#define CPP_VERSION_98 199711L
#define CPP_VERSION_11 201103L
#define CPP_VERSION_14 201402L
#define CPP_VERSION_17 201703L
#define CPP_VERSION_20 202002L
#define CPP_VERSION_23 202101L

#if CPP_VERSION < CPP_VERSION_14
#error C++ 14 or above is required
#endif

#endif

#if !defined(HEPH_CONSTEVAL)

#if CPP_VERSION >= CPP_VERSION_20
#define HEPH_CONSTEVAL consteval
#else
#define HEPH_CONSTEVAL constexpr
#endif

#endif


#if (!defined(_MSC_VER) || defined(__INTEL_COMPILER))

#include <cstring>

#endif



#if !defined(HEPH_FLOAT)

#if defined(HEPH_HIGH_PRECISION_FLOAT)
typedef double heph_float;
#else
typedef float heph_float;
#endif

#define HEPH_FLOAT heph_float

#endif


namespace HephCommon
{
#if !defined(HEPH_ENDIAN)
	
	enum Endian : uint8_t
	{
		Little = 0x00,
		Big = 0x01,
		Unknown = 0xFF
	};
	extern Endian systemEndian;
	void ChangeEndian(uint8_t* pData, uint8_t dataSize);
	constexpr inline Endian operator!(const Endian& lhs) { return lhs == Endian::Big ? Endian::Little : (lhs == Endian::Little ? Endian::Big : Endian::Unknown); }

#define HEPH_ENDIAN HephCommon::Endian
#define HEPH_SYSTEM_ENDIAN HephCommon::systemEndian
#define HEPH_CHANGE_ENDIAN(pData, dataSize) HephCommon::ChangeEndian(pData, dataSize)

#endif

#if !defined(HEPH_CONVOLUTION_MODE)
	
	enum ConvolutionMode
	{
		Full = 0,
		Central = 1,
		ValidPadding = 2
	};
#define HEPH_CONVOLUTION_MODE HephCommon::ConvolutionMode

#endif
}