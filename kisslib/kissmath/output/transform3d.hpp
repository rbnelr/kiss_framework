// file was generated by kissmath.py at <TODO: add github link>
#pragma once

////// Forward declarations

#include "float3.hpp"
#include "float3x3.hpp"
#include "float3x4.hpp"

namespace kissmath {
	
	inline float3x3 rotate3_X (float ang);
	
	inline float3x3 rotate3_Y (float ang);
	
	inline float3x3 rotate3_Z (float ang);
	
	inline constexpr float3x3 scale (float3 v);
	
	inline constexpr float3x4 translate (float3 v);
	
	
	
}


#include "transform3d.inl"
