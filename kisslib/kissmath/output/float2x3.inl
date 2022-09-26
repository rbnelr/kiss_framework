// file was generated by kissmath.py at <TODO: add github link>

////// Inline definitions

#include "float2x2.hpp"
#include "float3x3.hpp"
#include "float4x4.hpp"
#include "float3x4.hpp"

namespace kissmath {
	
	//// Accessors
	
	
	// get cell with row, column indecies
	inline constexpr float const& float2x3::get (int r, int c) const {
		return arr[c][r];
	}
	
	// get matrix column
	inline constexpr float2 const& float2x3::get_column (int indx) const {
		return arr[indx];
	}
	
	// get matrix row
	inline constexpr float3 float2x3::get_row (int indx) const {
		return float3(arr[0][indx], arr[1][indx], arr[2][indx]);
	}
	
	//// Constructors
	
	
	// supply one value for all cells
	inline constexpr float2x3::float2x3 (float all): 
	arr{float2(all, all),
		float2(all, all),
		float2(all, all)} {
		
	}
	
	// supply all cells, in row major order for readability -> c<row><column>
	inline constexpr float2x3::float2x3 (float c00, float c01, float c02,
										 float c10, float c11, float c12): 
	arr{float2(c00, c10),
		float2(c01, c11),
		float2(c02, c12)} {
		
	}
	
	// static rows() and columns() methods are preferred over constructors, to avoid confusion if column or row vectors are supplied to the constructor
	
	// supply all row vectors
	inline constexpr float2x3 float2x3::rows (float3 row0, float3 row1) {
		return float2x3(row0[0], row0[1], row0[2],
						row1[0], row1[1], row1[2]);
	}
	
	// supply all cells in row major order
	inline constexpr float2x3 float2x3::rows (float c00, float c01, float c02,
											  float c10, float c11, float c12) {
		return float2x3(c00, c01, c02,
						c10, c11, c12);
	}
	
	// supply all column vectors
	inline constexpr float2x3 float2x3::columns (float2 col0, float2 col1, float2 col2) {
		return float2x3(col0[0], col1[0], col2[0],
						col0[1], col1[1], col2[1]);
	}
	
	// supply all cells in column major order
	inline constexpr float2x3 float2x3::columns (float c00, float c10,
												 float c01, float c11,
												 float c02, float c12) {
		return float2x3(c00, c01, c02,
						c10, c11, c12);
	}
	
	
	// identity matrix
	inline constexpr float2x3 float2x3::identity () {
		return float2x3(1,0,0,
						0,1,0);
	}
	
	// Casting operators
	
	
	// extend/truncate matrix of other size
	inline constexpr float2x3::operator float2x2 () const {
		return float2x2(arr[0][0], arr[1][0],
						arr[0][1], arr[1][1]);
	}
	
	// extend/truncate matrix of other size
	inline constexpr float2x3::operator float3x3 () const {
		return float3x3(arr[0][0], arr[1][0], arr[2][0],
						arr[0][1], arr[1][1], arr[2][1],
						        0,         0,         1);
	}
	
	// extend/truncate matrix of other size
	inline constexpr float2x3::operator float4x4 () const {
		return float4x4(arr[0][0], arr[1][0], arr[2][0],         0,
						arr[0][1], arr[1][1], arr[2][1],         0,
						        0,         0,         1,         0,
						        0,         0,         0,         1);
	}
	
	// extend/truncate matrix of other size
	inline constexpr float2x3::operator float3x4 () const {
		return float3x4(arr[0][0], arr[1][0], arr[2][0],         0,
						arr[0][1], arr[1][1], arr[2][1],         0,
						        0,         0,         1,         0);
	}
	
	// Componentwise operators; These might be useful in some cases
	
	
	// add scalar to all matrix cells
	inline float2x3& float2x3::operator+= (float r) {
		*this = *this + r;
		return *this;
	}
	
	// substract scalar from all matrix cells
	inline float2x3& float2x3::operator-= (float r) {
		*this = *this - r;
		return *this;
	}
	
	// multiply scalar with all matrix cells
	inline float2x3& float2x3::operator*= (float r) {
		*this = *this * r;
		return *this;
	}
	
	// divide all matrix cells by scalar
	inline float2x3& float2x3::operator/= (float r) {
		*this = *this / r;
		return *this;
	}
	
	// Matrix multiplication
	
	
	// matrix-matrix muliplication
	inline float2x3& float2x3::operator*= (float2x3 const& r) {
		*this = *this * r;
		return *this;
	}
	
	// Componentwise operators; These might be useful in some cases
	
	
	
	// componentwise matrix_cell + matrix_cell
	inline constexpr float2x3 operator+ (float2x3 const& l, float2x3 const& r) {
		return float2x3(l.arr[0][0] + r.arr[0][0], l.arr[1][0] + r.arr[1][0], l.arr[2][0] + r.arr[2][0],
						l.arr[0][1] + r.arr[0][1], l.arr[1][1] + r.arr[1][1], l.arr[2][1] + r.arr[2][1]);
	}
	
	// componentwise matrix_cell + scalar
	inline constexpr float2x3 operator+ (float2x3 const& l, float r) {
		return float2x3(l.arr[0][0] + r, l.arr[1][0] + r, l.arr[2][0] + r,
						l.arr[0][1] + r, l.arr[1][1] + r, l.arr[2][1] + r);
	}
	
	// componentwise scalar + matrix_cell
	inline constexpr float2x3 operator+ (float l, float2x3 const& r) {
		return float2x3(l + r.arr[0][0], l + r.arr[1][0], l + r.arr[2][0],
						l + r.arr[0][1], l + r.arr[1][1], l + r.arr[2][1]);
	}
	
	
	// componentwise matrix_cell - matrix_cell
	inline constexpr float2x3 operator- (float2x3 const& l, float2x3 const& r) {
		return float2x3(l.arr[0][0] - r.arr[0][0], l.arr[1][0] - r.arr[1][0], l.arr[2][0] - r.arr[2][0],
						l.arr[0][1] - r.arr[0][1], l.arr[1][1] - r.arr[1][1], l.arr[2][1] - r.arr[2][1]);
	}
	
	// componentwise matrix_cell - scalar
	inline constexpr float2x3 operator- (float2x3 const& l, float r) {
		return float2x3(l.arr[0][0] - r, l.arr[1][0] - r, l.arr[2][0] - r,
						l.arr[0][1] - r, l.arr[1][1] - r, l.arr[2][1] - r);
	}
	
	// componentwise scalar - matrix_cell
	inline constexpr float2x3 operator- (float l, float2x3 const& r) {
		return float2x3(l - r.arr[0][0], l - r.arr[1][0], l - r.arr[2][0],
						l - r.arr[0][1], l - r.arr[1][1], l - r.arr[2][1]);
	}
	
	
	// componentwise matrix_cell * matrix_cell
	inline constexpr float2x3 mul_componentwise (float2x3 const& l, float2x3 const& r) {
		return float2x3(l.arr[0][0] * r.arr[0][0], l.arr[1][0] * r.arr[1][0], l.arr[2][0] * r.arr[2][0],
						l.arr[0][1] * r.arr[0][1], l.arr[1][1] * r.arr[1][1], l.arr[2][1] * r.arr[2][1]);
	}
	
	// componentwise matrix_cell * scalar
	inline constexpr float2x3 operator* (float2x3 const& l, float r) {
		return float2x3(l.arr[0][0] * r, l.arr[1][0] * r, l.arr[2][0] * r,
						l.arr[0][1] * r, l.arr[1][1] * r, l.arr[2][1] * r);
	}
	
	// componentwise scalar * matrix_cell
	inline constexpr float2x3 operator* (float l, float2x3 const& r) {
		return float2x3(l * r.arr[0][0], l * r.arr[1][0], l * r.arr[2][0],
						l * r.arr[0][1], l * r.arr[1][1], l * r.arr[2][1]);
	}
	
	
	// componentwise matrix_cell / matrix_cell
	inline constexpr float2x3 div_componentwise (float2x3 const& l, float2x3 const& r) {
		return float2x3(l.arr[0][0] / r.arr[0][0], l.arr[1][0] / r.arr[1][0], l.arr[2][0] / r.arr[2][0],
						l.arr[0][1] / r.arr[0][1], l.arr[1][1] / r.arr[1][1], l.arr[2][1] / r.arr[2][1]);
	}
	
	// componentwise matrix_cell / scalar
	inline constexpr float2x3 operator/ (float2x3 const& l, float r) {
		return float2x3(l.arr[0][0] / r, l.arr[1][0] / r, l.arr[2][0] / r,
						l.arr[0][1] / r, l.arr[1][1] / r, l.arr[2][1] / r);
	}
	
	// componentwise scalar / matrix_cell
	inline constexpr float2x3 operator/ (float l, float2x3 const& r) {
		return float2x3(l / r.arr[0][0], l / r.arr[1][0], l / r.arr[2][0],
						l / r.arr[0][1], l / r.arr[1][1], l / r.arr[2][1]);
	}
	
	// Matrix ops
	
	
	// matrix-matrix multiply
	inline float2x3 operator* (float2x2 const& l, float2x3 const& r) {
		float2x3 ret;
		ret.arr[0] = l * r.arr[0];
		ret.arr[1] = l * r.arr[1];
		ret.arr[2] = l * r.arr[2];
		return ret;
	}
	
	// matrix-matrix multiply
	inline float2x3 operator* (float2x3 const& l, float3x3 const& r) {
		float2x3 ret;
		ret.arr[0] = l * r.arr[0];
		ret.arr[1] = l * r.arr[1];
		ret.arr[2] = l * r.arr[2];
		return ret;
	}
	
	// matrix-vector multiply
	inline float2 operator* (float2x3 const& l, float3 r) {
		float2 ret;
		ret[0] = l.arr[0].x * r.x + l.arr[1].x * r.y + l.arr[2].x * r.z;
		ret[1] = l.arr[0].y * r.x + l.arr[1].y * r.y + l.arr[2].y * r.z;
		return ret;
	}
	
	// vector-matrix multiply
	inline float3 operator* (float2 l, float2x3 const& r) {
		float3 ret;
		ret[0] = l.x * r.arr[0].x + l.y * r.arr[0].y;
		ret[1] = l.x * r.arr[1].x + l.y * r.arr[1].y;
		ret[2] = l.x * r.arr[2].x + l.y * r.arr[2].y;
		return ret;
	}
	
	// Matrix operation shortforms so that you can treat a 2x3 matrix as a 2x2 matrix plus translation
	
	
	// shortform for float2x3 * (float3x3)float2x2
	inline float2x3 operator* (float2x3 const& l, float2x2 const& r) {
		return l * (float3x3)r;
	}
	
	// shortform for float2x3 * (float3x3)float2x3
	inline float2x3 operator* (float2x3 const& l, float2x3 const& r) {
		return l * (float3x3)r;
	}
	
	// shortform for float2x3 * float3(float2, 1)
	inline float2 operator* (float2x3 const& l, float2 r) {
		return l * float3(r, 1);
	}
}

