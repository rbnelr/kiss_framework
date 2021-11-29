#pragma once
#include "stdint.h"
#include "kissmath/output/float3.hpp"
#include "kissmath/output/float4.hpp"
#include "kissmath/output/int3.hpp"
#include "kissmath/output/int4.hpp"
#include "kissmath/output/uint8v3.hpp"
#include "kissmath/output/uint8v4.hpp"

namespace kissmath {

	typedef float3 lrgb;
	typedef float4 lrgba;
	typedef uint8v3 srgb8;
	typedef uint8v4 srgba8;

	// color [0,255] to color [0,1]
	inline float u8_to_float (uint8_t srgb) {
		return (float)srgb / 255.0f;
	}
	// color [0,1] to color [0,255]
	// (clamped)
	inline uint8_t float_to_u8 (float linear) {
		return (uint8_t)roundi(clamp(linear * 255.0f, 0.0f, 255.0f));
	}

	// srgb [0,1] to linear [0,1]
	inline float to_linear (float srgb) {
		if (srgb <= 0.0404482362771082f) {
			return srgb * 1/12.92f;
		} else {
			return pow( (srgb +0.055f) * 1/1.055f, 2.4f );
		}
	}
	// linear [0,1] to srgb [0,1]
	inline float to_srgb (float linear) {
		if (linear <= 0.00313066844250063f) {
			return linear * 12.92f;
		} else {
			return ( 1.055f * pow(linear, 1/2.4f) ) -0.055f;
		}
	}

	inline lrgb to_linear (srgb8 const& srgb) {
		return lrgb(	to_linear(u8_to_float(srgb.x)),
						to_linear(u8_to_float(srgb.y)),
						to_linear(u8_to_float(srgb.z)) );
	}
	inline lrgba to_linear (srgba8 const& srgba) {
		return lrgba(	to_linear(u8_to_float(srgba.x)),
						to_linear(u8_to_float(srgba.y)),
						to_linear(u8_to_float(srgba.z)),
								  u8_to_float(srgba.w) );
	}

	inline srgb8 to_srgb (lrgb const& lrgb) {
		return srgb8(	float_to_u8(to_srgb(lrgb.x)),
						float_to_u8(to_srgb(lrgb.y)),
						float_to_u8(to_srgb(lrgb.z)) );
	}
	inline srgba8 to_srgb (lrgba const& lrgba) {
		return srgba8(	float_to_u8(to_srgb(lrgba.x)),
						float_to_u8(to_srgb(lrgba.y)),
						float_to_u8(to_srgb(lrgba.z)),
						float_to_u8(        lrgba.w ) );
	}

	inline lrgb srgb (uint8_t all) {
		return to_linear(srgb8(all));
	}
	inline lrgb srgb (uint8_t r, uint8_t g, uint8_t b) {
		return to_linear(srgb8(r,g,b));
	}

	inline lrgba srgba (uint8_t r, uint8_t g, uint8_t b) {
		return lrgba(to_linear(srgb8(r,g,b)), 1);
	}

	// 0xrrbbgg format like in web
	//inline lrgba srgba (int32_t rgb_hex) {
	//	return srgba((rgb_hex >> 16) & 0xff, (rgb_hex >> 8) & 0xff, rgb_hex & 0xff);
	//}

	// alpha is linear
	inline lrgba srgba (uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		return to_linear(srgba8(r,g,b,a));
	}
	inline lrgba srgba (uint8_t col, uint8_t a=255) {
		return to_linear(srgba8(col,col,col,a));
	}

	// hue, saturation, value to linear rgb
	// all in in [0,1]
	// (hue is modulo 1, so out of range values will repeat)
	inline lrgb hsv2rgb (float hue, float saturation, float value) {
		// https://www.rapidtables.com/convert/color/hsv-to-rgb.html
		float h6 = hue*6.0f;

		float c = value * saturation;
		float x = c * (1.0f - abs(wrap(h6, 2.0f) - 1.0f));
		float m = value - c;

		float3 rgb;
		if      ( h6 < 1.0f ) rgb = float3(c,x,0);
		else if ( h6 < 2.0f ) rgb = float3(x,c,0);
		else if ( h6 < 3.0f ) rgb = float3(0,c,x);
		else if ( h6 < 4.0f ) rgb = float3(0,x,c);
		else if ( h6 < 5.0f ) rgb = float3(x,0,c);
		else                  rgb = float3(c,0,x);
		rgb += m;

		// TODO: colors don't seem like they should be linear already
		// since these formulars are likely usually used with srgb values
		return rgb;
	}

	inline lrgb hsl2rgb (float3 const& hsv) {
		return hsv2rgb(hsv.x, hsv.y, hsv.z);
	}
}
