// file was generated by kissmath.py at <TODO: add github link>
#pragma once

////// Forward declarations

#include "uint8.hpp"

namespace kissmath {
	//// forward declarations
	
	struct uint8v2;
	struct float4;
	struct int64v4;
	struct uint8v3;
	struct int4;
	struct bool4;
	
	struct uint8v4 {
		union { // Union with named members and array members to allow vector[] operator, not 100% sure that this is not undefined behavoir, but I think all compilers definitely don't screw up this use case
			struct {
				uint8	x, y, z, w;
			};
			uint8		arr[4];
		};
		
		// Component indexing operator
		inline constexpr uint8& operator[] (int i);
		
		// Component indexing operator
		inline constexpr uint8 const& operator[] (int i) const;
		
		
		// uninitialized constructor
		inline uint8v4 () = default;
		
		// sets all components to one value
		// implicit constructor -> float3(x,y,z) * 5 will be turned into float3(x,y,z) * float3(5) by to compiler to be able to execute operator*(float3, float3), which is desirable
		// and short initialization like float3 a = 0; works
		inline constexpr uint8v4 (uint8 all);
		
		// supply all components
		inline constexpr uint8v4 (uint8 x, uint8 y, uint8 z, uint8 w);
		
		// extend vector
		inline constexpr uint8v4 (uint8v2 xy, uint8 z, uint8 w);
		
		// extend vector
		inline constexpr uint8v4 (uint8v3 xyz, uint8 w);
		
		
		//// Truncating cast operators
		
		// truncating cast operator
		inline constexpr explicit operator uint8v2 () const;
		
		// truncating cast operator
		inline constexpr explicit operator uint8v3 () const;
		
		
		//// Type cast operators
		
		// type cast operator
		inline constexpr explicit operator bool4 () const;
		
		// type cast operator
		inline constexpr explicit operator float4 () const;
		
		// type cast operator
		inline constexpr explicit operator int4 () const;
		
		// type cast operator
		inline constexpr explicit operator int64v4 () const;
		
		
		// componentwise arithmetic operator
		inline uint8v4 operator+= (uint8v4 r);
		
		// componentwise arithmetic operator
		inline uint8v4 operator-= (uint8v4 r);
		
		// componentwise arithmetic operator
		inline uint8v4 operator*= (uint8v4 r);
		
		// componentwise arithmetic operator
		inline uint8v4 operator/= (uint8v4 r);
		
	};
	
	//// arthmethic ops
	
	inline constexpr uint8v4 operator+ (uint8v4 v);
	
	inline constexpr uint8v4 operator+ (uint8v4 l, uint8v4 r);
	
	inline constexpr uint8v4 operator- (uint8v4 l, uint8v4 r);
	
	inline constexpr uint8v4 operator* (uint8v4 l, uint8v4 r);
	
	inline constexpr uint8v4 operator/ (uint8v4 l, uint8v4 r);
	
	
	//// bitwise ops
	
	inline constexpr uint8v4 operator~ (uint8v4 v);
	
	inline constexpr uint8v4 operator& (uint8v4 l, uint8v4 r);
	
	inline constexpr uint8v4 operator| (uint8v4 l, uint8v4 r);
	
	inline constexpr uint8v4 operator^ (uint8v4 l, uint8v4 r);
	
	inline constexpr uint8v4 operator<< (uint8v4 l, uint8v4 r);
	
	inline constexpr uint8v4 operator>> (uint8v4 l, uint8v4 r);
	
	
	//// comparison ops
	
	// componentwise comparison returns a bool vector
	inline constexpr bool4 operator< (uint8v4 l, uint8v4 r);
	
	// componentwise comparison returns a bool vector
	inline constexpr bool4 operator<= (uint8v4 l, uint8v4 r);
	
	// componentwise comparison returns a bool vector
	inline constexpr bool4 operator> (uint8v4 l, uint8v4 r);
	
	// componentwise comparison returns a bool vector
	inline constexpr bool4 operator>= (uint8v4 l, uint8v4 r);
	
	// componentwise equality comparison, returns a bool vector
	inline constexpr bool4 equal (uint8v4 l, uint8v4 r);
	
	// componentwise inequality comparison, returns a bool vector
	inline constexpr bool4 nequal (uint8v4 l, uint8v4 r);
	
	// full equality comparison, returns true only if all components are equal
	inline constexpr bool operator== (uint8v4 l, uint8v4 r);
	
	// full inequality comparison, returns true if any components are inequal
	inline constexpr bool operator!= (uint8v4 l, uint8v4 r);
	
	// componentwise ternary (c ? l : r)
	inline constexpr uint8v4 select (bool4 c, uint8v4 l, uint8v4 r);
	
	
	//// misc ops
	// componentwise minimum
	inline constexpr uint8v4 min (uint8v4 l, uint8v4 r);
	
	// componentwise maximum
	inline constexpr uint8v4 max (uint8v4 l, uint8v4 r);
	
	// componentwise clamp into range [a,b]
	inline constexpr uint8v4 clamp (uint8v4 x, uint8v4 a, uint8v4 b);
	
	// componentwise clamp into range [0,1] also known as saturate in hlsl
	inline constexpr uint8v4 clamp (uint8v4 x);
	
	// get minimum component of vector, optionally get component index via min_index
	inline uint8 min_component (uint8v4 v, int* min_index=nullptr);
	
	// get maximum component of vector, optionally get component index via max_index
	inline uint8 max_component (uint8v4 v, int* max_index=nullptr);
	
	
	// componentwise wrap
	inline uint8v4 wrap (uint8v4 v, uint8v4 range);
	
	// componentwise wrap
	inline uint8v4 wrap (uint8v4 v, uint8v4 a, uint8v4 b);
	
	
	
	//// Vector math
	
}


#include "uint8v4.inl"
