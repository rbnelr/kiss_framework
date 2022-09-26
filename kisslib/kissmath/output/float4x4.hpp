// file was generated by kissmath.py at <TODO: add github link>
#pragma once

////// Forward declarations

#include "float4.hpp"

namespace kissmath {
	
	//// matrix forward declarations
	struct float2x2;
	struct float3x3;
	struct float2x3;
	struct float3x4;
	
	struct float4x4 {
		float4 arr[4]; // column major for compatibility with OpenGL
		
		//// Accessors
		
		// get cell with row, column indecies
		inline constexpr float const& get (int r, int c) const;
		
		// get matrix column
		inline constexpr float4 const& get_column (int indx) const;
		
		// get matrix row
		inline constexpr float4 get_row (int indx) const;
		
		
		//// Constructors
		
		// uninitialized constructor
		inline float4x4 () = default;
		
		// supply one value for all cells
		inline constexpr explicit float4x4 (float all);
		
		// supply all cells, in row major order for readability -> c<row><column>
		inline constexpr explicit float4x4 (float c00, float c01, float c02, float c03,
											float c10, float c11, float c12, float c13,
											float c20, float c21, float c22, float c23,
											float c30, float c31, float c32, float c33);
		
		
		// static rows() and columns() methods are preferred over constructors, to avoid confusion if column or row vectors are supplied to the constructor
		// supply all row vectors
		static inline constexpr float4x4 rows (float4 row0, float4 row1, float4 row2, float4 row3);
		
		// supply all cells in row major order
		static inline constexpr float4x4 rows (float c00, float c01, float c02, float c03,
											   float c10, float c11, float c12, float c13,
											   float c20, float c21, float c22, float c23,
											   float c30, float c31, float c32, float c33);
		
		// supply all column vectors
		static inline constexpr float4x4 columns (float4 col0, float4 col1, float4 col2, float4 col3);
		
		// supply all cells in column major order
		static inline constexpr float4x4 columns (float c00, float c10, float c20, float c30,
												  float c01, float c11, float c21, float c31,
												  float c02, float c12, float c22, float c32,
												  float c03, float c13, float c23, float c33);
		
		
		// identity matrix
		static inline constexpr float4x4 identity ();
		
		
		// Casting operators
		
		// extend/truncate matrix of other size
		inline constexpr explicit operator float2x2 () const;
		
		// extend/truncate matrix of other size
		inline constexpr explicit operator float3x3 () const;
		
		// extend/truncate matrix of other size
		inline constexpr explicit operator float2x3 () const;
		
		// extend/truncate matrix of other size
		inline constexpr explicit operator float3x4 () const;
		
		
		// Componentwise operators; These might be useful in some cases
		
		// add scalar to all matrix cells
		inline float4x4& operator+= (float r);
		
		// substract scalar from all matrix cells
		inline float4x4& operator-= (float r);
		
		// multiply scalar with all matrix cells
		inline float4x4& operator*= (float r);
		
		// divide all matrix cells by scalar
		inline float4x4& operator/= (float r);
		
		
		// Matrix multiplication
		
		// matrix-matrix muliplication
		inline float4x4& operator*= (float4x4 const& r);
		
	};
	
	// Componentwise operators; These might be useful in some cases
	
	
	// componentwise matrix_cell + matrix_cell
	inline constexpr float4x4 operator+ (float4x4 const& l, float4x4 const& r);
	
	// componentwise matrix_cell + scalar
	inline constexpr float4x4 operator+ (float4x4 const& l, float r);
	
	// componentwise scalar + matrix_cell
	inline constexpr float4x4 operator+ (float l, float4x4 const& r);
	
	
	// componentwise matrix_cell - matrix_cell
	inline constexpr float4x4 operator- (float4x4 const& l, float4x4 const& r);
	
	// componentwise matrix_cell - scalar
	inline constexpr float4x4 operator- (float4x4 const& l, float r);
	
	// componentwise scalar - matrix_cell
	inline constexpr float4x4 operator- (float l, float4x4 const& r);
	
	
	// componentwise matrix_cell * matrix_cell
	inline constexpr float4x4 mul_componentwise (float4x4 const& l, float4x4 const& r);
	
	// componentwise matrix_cell * scalar
	inline constexpr float4x4 operator* (float4x4 const& l, float r);
	
	// componentwise scalar * matrix_cell
	inline constexpr float4x4 operator* (float l, float4x4 const& r);
	
	
	// componentwise matrix_cell / matrix_cell
	inline constexpr float4x4 div_componentwise (float4x4 const& l, float4x4 const& r);
	
	// componentwise matrix_cell / scalar
	inline constexpr float4x4 operator/ (float4x4 const& l, float r);
	
	// componentwise scalar / matrix_cell
	inline constexpr float4x4 operator/ (float l, float4x4 const& r);
	
	
	// Matrix ops
	
	// matrix-matrix multiply
	inline float4x4 operator* (float4x4 const& l, float4x4 const& r);
	
	// matrix-vector multiply
	inline float4 operator* (float4x4 const& l, float4 r);
	
	// vector-matrix multiply
	inline float4 operator* (float4 l, float4x4 const& r);
	
	inline constexpr float4x4 transpose (float4x4 const& m);
	
	
	inline float determinant (float4x4 const& mat);
	
	inline float4x4 inverse (float4x4 const& mat);
	
}


#include "float4x4.inl"
