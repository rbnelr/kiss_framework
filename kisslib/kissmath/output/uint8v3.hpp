// file was generated by kissmath.py at <TODO: add github link>
#pragma once

////// Forward declarations

#include "uint8.hpp"

namespace kissmath {
	//// forward declarations
	
	struct uint8v4;
	struct bool3;
	struct int3;
	struct float3;
	struct uint8v2;
	struct int64v3;
	
	struct uint8v3 {
		union { // Union with named members and array members to allow vector[] operator, not 100% sure that this is not undefined behavoir, but I think all compilers definitely don't screw up this use case
			struct {
				uint8	x, y, z;
			};
			uint8		arr[3];
		};
		
		// Component indexing operator
		inline constexpr uint8& operator[] (int i);
		
		// Component indexing operator
		inline constexpr uint8 const& operator[] (int i) const;
		
		
		// uninitialized constructor
		inline uint8v3 () = default;
		
		// sets all components to one value
		// implicit constructor -> float3(x,y,z) * 5 will be turned into float3(x,y,z) * float3(5) by to compiler to be able to execute operator*(float3, float3), which is desirable
		// and short initialization like float3 a = 0; works
		inline constexpr uint8v3 (uint8 all);
		
		// supply all components
		inline constexpr uint8v3 (uint8 x, uint8 y, uint8 z);
		
		// extend vector
		inline constexpr uint8v3 (uint8v2 xy, uint8 z);
		
		// truncate vector
		inline constexpr uint8v3 (uint8v4 v);
		
		
		//// Truncating cast operators
		
		// truncating cast operator
		inline constexpr explicit operator uint8v2 () const;
		
		
		//// Type cast operators
		
		// type cast operator
		inline constexpr explicit operator bool3 () const;
		
		// type cast operator
		inline constexpr explicit operator float3 () const;
		
		// type cast operator
		inline constexpr explicit operator int3 () const;
		
		// type cast operator
		inline constexpr explicit operator int64v3 () const;
		
		
		// componentwise arithmetic operator
		inline uint8v3 operator+= (uint8v3 r);
		
		// componentwise arithmetic operator
		inline uint8v3 operator-= (uint8v3 r);
		
		// componentwise arithmetic operator
		inline uint8v3 operator*= (uint8v3 r);
		
		// componentwise arithmetic operator
		inline uint8v3 operator/= (uint8v3 r);
		
	};
	
	//// arthmethic ops
	
	inline constexpr uint8v3 operator+ (uint8v3 v);
	
	inline constexpr uint8v3 operator+ (uint8v3 l, uint8v3 r);
	
	inline constexpr uint8v3 operator- (uint8v3 l, uint8v3 r);
	
	inline constexpr uint8v3 operator* (uint8v3 l, uint8v3 r);
	
	inline constexpr uint8v3 operator/ (uint8v3 l, uint8v3 r);
	
	
	//// bitwise ops
	
	inline constexpr uint8v3 operator~ (uint8v3 v);
	
	inline constexpr uint8v3 operator& (uint8v3 l, uint8v3 r);
	
	inline constexpr uint8v3 operator| (uint8v3 l, uint8v3 r);
	
	inline constexpr uint8v3 operator^ (uint8v3 l, uint8v3 r);
	
	inline constexpr uint8v3 operator<< (uint8v3 l, uint8v3 r);
	
	inline constexpr uint8v3 operator>> (uint8v3 l, uint8v3 r);
	
	
	//// comparison ops
	
	// componentwise comparison returns a bool vector
	inline constexpr bool3 operator< (uint8v3 l, uint8v3 r);
	
	// componentwise comparison returns a bool vector
	inline constexpr bool3 operator<= (uint8v3 l, uint8v3 r);
	
	// componentwise comparison returns a bool vector
	inline constexpr bool3 operator> (uint8v3 l, uint8v3 r);
	
	// componentwise comparison returns a bool vector
	inline constexpr bool3 operator>= (uint8v3 l, uint8v3 r);
	
	// componentwise equality comparison, returns a bool vector
	inline constexpr bool3 equal (uint8v3 l, uint8v3 r);
	
	// componentwise inequality comparison, returns a bool vector
	inline constexpr bool3 nequal (uint8v3 l, uint8v3 r);
	
	// full equality comparison, returns true only if all components are equal
	inline constexpr bool operator== (uint8v3 l, uint8v3 r);
	
	// full inequality comparison, returns true if any components are inequal
	inline constexpr bool operator!= (uint8v3 l, uint8v3 r);
	
	// componentwise ternary (c ? l : r)
	inline constexpr uint8v3 select (bool3 c, uint8v3 l, uint8v3 r);
	
	
	//// misc ops
	// componentwise minimum
	inline constexpr uint8v3 min (uint8v3 l, uint8v3 r);
	
	// componentwise maximum
	inline constexpr uint8v3 max (uint8v3 l, uint8v3 r);
	
	// componentwise clamp into range [a,b]
	inline constexpr uint8v3 clamp (uint8v3 x, uint8v3 a, uint8v3 b);
	
	// componentwise clamp into range [0,1] also known as saturate in hlsl
	inline constexpr uint8v3 clamp (uint8v3 x);
	
	// get minimum component of vector, optionally get component index via min_index
	inline uint8 min_component (uint8v3 v, int* min_index=nullptr);
	
	// get maximum component of vector, optionally get component index via max_index
	inline uint8 max_component (uint8v3 v, int* max_index=nullptr);
	
	
	// componentwise wrap
	inline uint8v3 wrap (uint8v3 v, uint8v3 range);
	
	// componentwise wrap
	inline uint8v3 wrap (uint8v3 v, uint8v3 a, uint8v3 b);
	
	
	
	//// Vector math
	
}


#include "uint8v3.inl"
