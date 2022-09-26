// file was generated by kissmath.py at <TODO: add github link>
#pragma once

////// Forward declarations

#include "float2.hpp"

namespace kissmath {
	
	//// matrix forward declarations
	struct float3x3;
	struct float4x4;
	struct float2x3;
	struct float3x4;
	
	struct float2x2 {
		float2 arr[2]; // column major for compatibility with OpenGL
		
		//// Accessors
		
		// get cell with row, column indecies
		inline constexpr float const& get (int r, int c) const;
		
		// get matrix column
		inline constexpr float2 const& get_column (int indx) const;
		
		// get matrix row
		inline constexpr float2 get_row (int indx) const;
		
		
		//// Constructors
		
		// uninitialized constructor
		inline float2x2 () = default;
		
		// supply one value for all cells
		inline constexpr explicit float2x2 (float all);
		
		// supply all cells, in row major order for readability -> c<row><column>
		inline constexpr explicit float2x2 (float c00, float c01,
											float c10, float c11);
		
		
		// static rows() and columns() methods are preferred over constructors, to avoid confusion if column or row vectors are supplied to the constructor
		// supply all row vectors
		static inline constexpr float2x2 rows (float2 row0, float2 row1);
		
		// supply all cells in row major order
		static inline constexpr float2x2 rows (float c00, float c01,
											   float c10, float c11);
		
		// supply all column vectors
		static inline constexpr float2x2 columns (float2 col0, float2 col1);
		
		// supply all cells in column major order
		static inline constexpr float2x2 columns (float c00, float c10,
												  float c01, float c11);
		
		
		// identity matrix
		static inline constexpr float2x2 identity ();
		
		
		// Casting operators
		
		// extend/truncate matrix of other size
		inline constexpr explicit operator float3x3 () const;
		
		// extend/truncate matrix of other size
		inline constexpr explicit operator float4x4 () const;
		
		// extend/truncate matrix of other size
		inline constexpr explicit operator float2x3 () const;
		
		// extend/truncate matrix of other size
		inline constexpr explicit operator float3x4 () const;
		
		
		// Componentwise operators; These might be useful in some cases
		
		// add scalar to all matrix cells
		inline float2x2& operator+= (float r);
		
		// substract scalar from all matrix cells
		inline float2x2& operator-= (float r);
		
		// multiply scalar with all matrix cells
		inline float2x2& operator*= (float r);
		
		// divide all matrix cells by scalar
		inline float2x2& operator/= (float r);
		
		
		// Matrix multiplication
		
		// matrix-matrix muliplication
		inline float2x2& operator*= (float2x2 const& r);
		
	};
	
	// Componentwise operators; These might be useful in some cases
	
	
	// componentwise matrix_cell + matrix_cell
	inline constexpr float2x2 operator+ (float2x2 const& l, float2x2 const& r);
	
	// componentwise matrix_cell + scalar
	inline constexpr float2x2 operator+ (float2x2 const& l, float r);
	
	// componentwise scalar + matrix_cell
	inline constexpr float2x2 operator+ (float l, float2x2 const& r);
	
	
	// componentwise matrix_cell - matrix_cell
	inline constexpr float2x2 operator- (float2x2 const& l, float2x2 const& r);
	
	// componentwise matrix_cell - scalar
	inline constexpr float2x2 operator- (float2x2 const& l, float r);
	
	// componentwise scalar - matrix_cell
	inline constexpr float2x2 operator- (float l, float2x2 const& r);
	
	
	// componentwise matrix_cell * matrix_cell
	inline constexpr float2x2 mul_componentwise (float2x2 const& l, float2x2 const& r);
	
	// componentwise matrix_cell * scalar
	inline constexpr float2x2 operator* (float2x2 const& l, float r);
	
	// componentwise scalar * matrix_cell
	inline constexpr float2x2 operator* (float l, float2x2 const& r);
	
	
	// componentwise matrix_cell / matrix_cell
	inline constexpr float2x2 div_componentwise (float2x2 const& l, float2x2 const& r);
	
	// componentwise matrix_cell / scalar
	inline constexpr float2x2 operator/ (float2x2 const& l, float r);
	
	// componentwise scalar / matrix_cell
	inline constexpr float2x2 operator/ (float l, float2x2 const& r);
	
	
	// Matrix ops
	
	// matrix-matrix multiply
	inline float2x2 operator* (float2x2 const& l, float2x2 const& r);
	
	// matrix-vector multiply
	inline float2 operator* (float2x2 const& l, float2 r);
	
	// vector-matrix multiply
	inline float2 operator* (float2 l, float2x2 const& r);
	
	inline constexpr float2x2 transpose (float2x2 const& m);
	
	
	inline float determinant (float2x2 const& mat);
	
	inline float2x2 inverse (float2x2 const& mat);
	
}


#include "float2x2.inl"
