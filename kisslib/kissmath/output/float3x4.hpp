// file was generated by kissmath.py at <TODO: add github link>
#pragma once

////// Forward declarations

#include "float4.hpp"
#include "float3.hpp"

namespace kissmath {
	
	//// matrix forward declarations
	struct float2x2;
	struct float3x3;
	struct float4x4;
	struct float2x3;
	
	struct float3x4 {
		float3 arr[4]; // column major for compatibility with OpenGL
		
		//// Accessors
		
		// get cell with row, column indecies
		inline constexpr float const& get (int r, int c) const;
		
		// get matrix column
		inline constexpr float3 const& get_column (int indx) const;
		
		// get matrix row
		inline constexpr float4 get_row (int indx) const;
		
		
		//// Constructors
		
		// uninitialized constructor
		inline float3x4 () = default;
		
		// supply one value for all cells
		inline constexpr explicit float3x4 (float all);
		
		// supply all cells, in row major order for readability -> c<row><column>
		inline constexpr explicit float3x4 (float c00, float c01, float c02, float c03,
											float c10, float c11, float c12, float c13,
											float c20, float c21, float c22, float c23);
		
		
		// static rows() and columns() methods are preferred over constructors, to avoid confusion if column or row vectors are supplied to the constructor
		// supply all row vectors
		static inline constexpr float3x4 rows (float4 row0, float4 row1, float4 row2);
		
		// supply all cells in row major order
		static inline constexpr float3x4 rows (float c00, float c01, float c02, float c03,
											   float c10, float c11, float c12, float c13,
											   float c20, float c21, float c22, float c23);
		
		// supply all column vectors
		static inline constexpr float3x4 columns (float3 col0, float3 col1, float3 col2, float3 col3);
		
		// supply all cells in column major order
		static inline constexpr float3x4 columns (float c00, float c10, float c20,
												  float c01, float c11, float c21,
												  float c02, float c12, float c22,
												  float c03, float c13, float c23);
		
		
		// identity matrix
		static inline constexpr float3x4 identity ();
		
		
		// Casting operators
		
		// extend/truncate matrix of other size
		inline constexpr explicit operator float2x2 () const;
		
		// extend/truncate matrix of other size
		inline constexpr explicit operator float3x3 () const;
		
		// extend/truncate matrix of other size
		inline constexpr explicit operator float4x4 () const;
		
		// extend/truncate matrix of other size
		inline constexpr explicit operator float2x3 () const;
		
		
		// Componentwise operators; These might be useful in some cases
		
		// add scalar to all matrix cells
		inline float3x4& operator+= (float r);
		
		// substract scalar from all matrix cells
		inline float3x4& operator-= (float r);
		
		// multiply scalar with all matrix cells
		inline float3x4& operator*= (float r);
		
		// divide all matrix cells by scalar
		inline float3x4& operator/= (float r);
		
		
		// Matrix multiplication
		
		// matrix-matrix muliplication
		inline float3x4& operator*= (float3x4 const& r);
		
	};
	
	// Componentwise operators; These might be useful in some cases
	
	
	// componentwise matrix_cell + matrix_cell
	inline constexpr float3x4 operator+ (float3x4 const& l, float3x4 const& r);
	
	// componentwise matrix_cell + scalar
	inline constexpr float3x4 operator+ (float3x4 const& l, float r);
	
	// componentwise scalar + matrix_cell
	inline constexpr float3x4 operator+ (float l, float3x4 const& r);
	
	
	// componentwise matrix_cell - matrix_cell
	inline constexpr float3x4 operator- (float3x4 const& l, float3x4 const& r);
	
	// componentwise matrix_cell - scalar
	inline constexpr float3x4 operator- (float3x4 const& l, float r);
	
	// componentwise scalar - matrix_cell
	inline constexpr float3x4 operator- (float l, float3x4 const& r);
	
	
	// componentwise matrix_cell * matrix_cell
	inline constexpr float3x4 mul_componentwise (float3x4 const& l, float3x4 const& r);
	
	// componentwise matrix_cell * scalar
	inline constexpr float3x4 operator* (float3x4 const& l, float r);
	
	// componentwise scalar * matrix_cell
	inline constexpr float3x4 operator* (float l, float3x4 const& r);
	
	
	// componentwise matrix_cell / matrix_cell
	inline constexpr float3x4 div_componentwise (float3x4 const& l, float3x4 const& r);
	
	// componentwise matrix_cell / scalar
	inline constexpr float3x4 operator/ (float3x4 const& l, float r);
	
	// componentwise scalar / matrix_cell
	inline constexpr float3x4 operator/ (float l, float3x4 const& r);
	
	
	// Matrix ops
	
	// matrix-matrix multiply
	inline float3x4 operator* (float3x3 const& l, float3x4 const& r);
	
	// matrix-matrix multiply
	inline float3x4 operator* (float3x4 const& l, float4x4 const& r);
	
	// matrix-vector multiply
	inline float3 operator* (float3x4 const& l, float4 r);
	
	// vector-matrix multiply
	inline float4 operator* (float3 l, float3x4 const& r);
	
	
	// Matrix operation shortforms so that you can treat a 3x4 matrix as a 3x3 matrix plus translation
	
	// shortform for float3x4 * (float4x4)float3x3
	inline float3x4 operator* (float3x4 const& l, float3x3 const& r);
	
	// shortform for float3x4 * (float4x4)float3x4
	inline float3x4 operator* (float3x4 const& l, float3x4 const& r);
	
	// shortform for float3x4 * float4(float3, 1)
	inline float3 operator* (float3x4 const& l, float3 r);
	
}


#include "float3x4.inl"
