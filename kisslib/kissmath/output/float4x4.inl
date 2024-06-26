// file was generated by kissmath.py at <TODO: add github link>

////// Inline definitions

#include "float2x2.hpp"
#include "float3x3.hpp"
#include "float2x3.hpp"
#include "float3x4.hpp"

namespace kissmath {
	
	//// Accessors
	
	
	// get cell with row, column indecies
	inline constexpr float const& float4x4::get (int r, int c) const {
		return arr[c][r];
	}
	
	// get matrix column
	inline constexpr float4 const& float4x4::get_column (int indx) const {
		return arr[indx];
	}
	
	// get matrix row
	inline constexpr float4 float4x4::get_row (int indx) const {
		return float4(arr[0][indx], arr[1][indx], arr[2][indx], arr[3][indx]);
	}
	
	//// Constructors
	
	
	// supply one value for all cells
	inline constexpr float4x4::float4x4 (float all): 
	arr{float4(all, all, all, all),
		float4(all, all, all, all),
		float4(all, all, all, all),
		float4(all, all, all, all)} {
		
	}
	
	// supply all cells, in row major order for readability -> c<row><column>
	inline constexpr float4x4::float4x4 (float c00, float c01, float c02, float c03,
										 float c10, float c11, float c12, float c13,
										 float c20, float c21, float c22, float c23,
										 float c30, float c31, float c32, float c33): 
	arr{float4(c00, c10, c20, c30),
		float4(c01, c11, c21, c31),
		float4(c02, c12, c22, c32),
		float4(c03, c13, c23, c33)} {
		
	}
	
	// static rows() and columns() methods are preferred over constructors, to avoid confusion if column or row vectors are supplied to the constructor
	
	// supply all row vectors
	inline constexpr float4x4 float4x4::rows (float4 row0, float4 row1, float4 row2, float4 row3) {
		return float4x4(row0[0], row0[1], row0[2], row0[3],
						row1[0], row1[1], row1[2], row1[3],
						row2[0], row2[1], row2[2], row2[3],
						row3[0], row3[1], row3[2], row3[3]);
	}
	
	// supply all cells in row major order
	inline constexpr float4x4 float4x4::rows (float c00, float c01, float c02, float c03,
											  float c10, float c11, float c12, float c13,
											  float c20, float c21, float c22, float c23,
											  float c30, float c31, float c32, float c33) {
		return float4x4(c00, c01, c02, c03,
						c10, c11, c12, c13,
						c20, c21, c22, c23,
						c30, c31, c32, c33);
	}
	
	// supply all column vectors
	inline constexpr float4x4 float4x4::columns (float4 col0, float4 col1, float4 col2, float4 col3) {
		return float4x4(col0[0], col1[0], col2[0], col3[0],
						col0[1], col1[1], col2[1], col3[1],
						col0[2], col1[2], col2[2], col3[2],
						col0[3], col1[3], col2[3], col3[3]);
	}
	
	// supply all cells in column major order
	inline constexpr float4x4 float4x4::columns (float c00, float c10, float c20, float c30,
												 float c01, float c11, float c21, float c31,
												 float c02, float c12, float c22, float c32,
												 float c03, float c13, float c23, float c33) {
		return float4x4(c00, c01, c02, c03,
						c10, c11, c12, c13,
						c20, c21, c22, c23,
						c30, c31, c32, c33);
	}
	
	
	// identity matrix
	inline constexpr float4x4 float4x4::identity () {
		return float4x4(1,0,0,0,
						0,1,0,0,
						0,0,1,0,
						0,0,0,1);
	}
	
	// Casting operators
	
	
	// extend/truncate matrix of other size
	inline constexpr float4x4::operator float2x2 () const {
		return float2x2(arr[0][0], arr[1][0],
						arr[0][1], arr[1][1]);
	}
	
	// extend/truncate matrix of other size
	inline constexpr float4x4::operator float3x3 () const {
		return float3x3(arr[0][0], arr[1][0], arr[2][0],
						arr[0][1], arr[1][1], arr[2][1],
						arr[0][2], arr[1][2], arr[2][2]);
	}
	
	// extend/truncate matrix of other size
	inline constexpr float4x4::operator float2x3 () const {
		return float2x3(arr[0][0], arr[1][0], arr[2][0],
						arr[0][1], arr[1][1], arr[2][1]);
	}
	
	// extend/truncate matrix of other size
	inline constexpr float4x4::operator float3x4 () const {
		return float3x4(arr[0][0], arr[1][0], arr[2][0], arr[3][0],
						arr[0][1], arr[1][1], arr[2][1], arr[3][1],
						arr[0][2], arr[1][2], arr[2][2], arr[3][2]);
	}
	
	// Componentwise operators; These might be useful in some cases
	
	
	// add scalar to all matrix cells
	inline float4x4& float4x4::operator+= (float r) {
		*this = *this + r;
		return *this;
	}
	
	// substract scalar from all matrix cells
	inline float4x4& float4x4::operator-= (float r) {
		*this = *this - r;
		return *this;
	}
	
	// multiply scalar with all matrix cells
	inline float4x4& float4x4::operator*= (float r) {
		*this = *this * r;
		return *this;
	}
	
	// divide all matrix cells by scalar
	inline float4x4& float4x4::operator/= (float r) {
		*this = *this / r;
		return *this;
	}
	
	// Matrix multiplication
	
	
	// matrix-matrix muliplication
	inline float4x4& float4x4::operator*= (float4x4 const& r) {
		*this = *this * r;
		return *this;
	}
	
	// Componentwise operators; These might be useful in some cases
	
	
	
	// componentwise matrix_cell + matrix_cell
	inline constexpr float4x4 operator+ (float4x4 const& l, float4x4 const& r) {
		return float4x4(l.arr[0][0] + r.arr[0][0], l.arr[1][0] + r.arr[1][0], l.arr[2][0] + r.arr[2][0], l.arr[3][0] + r.arr[3][0],
						l.arr[0][1] + r.arr[0][1], l.arr[1][1] + r.arr[1][1], l.arr[2][1] + r.arr[2][1], l.arr[3][1] + r.arr[3][1],
						l.arr[0][2] + r.arr[0][2], l.arr[1][2] + r.arr[1][2], l.arr[2][2] + r.arr[2][2], l.arr[3][2] + r.arr[3][2],
						l.arr[0][3] + r.arr[0][3], l.arr[1][3] + r.arr[1][3], l.arr[2][3] + r.arr[2][3], l.arr[3][3] + r.arr[3][3]);
	}
	
	// componentwise matrix_cell + scalar
	inline constexpr float4x4 operator+ (float4x4 const& l, float r) {
		return float4x4(l.arr[0][0] + r, l.arr[1][0] + r, l.arr[2][0] + r, l.arr[3][0] + r,
						l.arr[0][1] + r, l.arr[1][1] + r, l.arr[2][1] + r, l.arr[3][1] + r,
						l.arr[0][2] + r, l.arr[1][2] + r, l.arr[2][2] + r, l.arr[3][2] + r,
						l.arr[0][3] + r, l.arr[1][3] + r, l.arr[2][3] + r, l.arr[3][3] + r);
	}
	
	// componentwise scalar + matrix_cell
	inline constexpr float4x4 operator+ (float l, float4x4 const& r) {
		return float4x4(l + r.arr[0][0], l + r.arr[1][0], l + r.arr[2][0], l + r.arr[3][0],
						l + r.arr[0][1], l + r.arr[1][1], l + r.arr[2][1], l + r.arr[3][1],
						l + r.arr[0][2], l + r.arr[1][2], l + r.arr[2][2], l + r.arr[3][2],
						l + r.arr[0][3], l + r.arr[1][3], l + r.arr[2][3], l + r.arr[3][3]);
	}
	
	
	// componentwise matrix_cell - matrix_cell
	inline constexpr float4x4 operator- (float4x4 const& l, float4x4 const& r) {
		return float4x4(l.arr[0][0] - r.arr[0][0], l.arr[1][0] - r.arr[1][0], l.arr[2][0] - r.arr[2][0], l.arr[3][0] - r.arr[3][0],
						l.arr[0][1] - r.arr[0][1], l.arr[1][1] - r.arr[1][1], l.arr[2][1] - r.arr[2][1], l.arr[3][1] - r.arr[3][1],
						l.arr[0][2] - r.arr[0][2], l.arr[1][2] - r.arr[1][2], l.arr[2][2] - r.arr[2][2], l.arr[3][2] - r.arr[3][2],
						l.arr[0][3] - r.arr[0][3], l.arr[1][3] - r.arr[1][3], l.arr[2][3] - r.arr[2][3], l.arr[3][3] - r.arr[3][3]);
	}
	
	// componentwise matrix_cell - scalar
	inline constexpr float4x4 operator- (float4x4 const& l, float r) {
		return float4x4(l.arr[0][0] - r, l.arr[1][0] - r, l.arr[2][0] - r, l.arr[3][0] - r,
						l.arr[0][1] - r, l.arr[1][1] - r, l.arr[2][1] - r, l.arr[3][1] - r,
						l.arr[0][2] - r, l.arr[1][2] - r, l.arr[2][2] - r, l.arr[3][2] - r,
						l.arr[0][3] - r, l.arr[1][3] - r, l.arr[2][3] - r, l.arr[3][3] - r);
	}
	
	// componentwise scalar - matrix_cell
	inline constexpr float4x4 operator- (float l, float4x4 const& r) {
		return float4x4(l - r.arr[0][0], l - r.arr[1][0], l - r.arr[2][0], l - r.arr[3][0],
						l - r.arr[0][1], l - r.arr[1][1], l - r.arr[2][1], l - r.arr[3][1],
						l - r.arr[0][2], l - r.arr[1][2], l - r.arr[2][2], l - r.arr[3][2],
						l - r.arr[0][3], l - r.arr[1][3], l - r.arr[2][3], l - r.arr[3][3]);
	}
	
	
	// componentwise matrix_cell * matrix_cell
	inline constexpr float4x4 mul_componentwise (float4x4 const& l, float4x4 const& r) {
		return float4x4(l.arr[0][0] * r.arr[0][0], l.arr[1][0] * r.arr[1][0], l.arr[2][0] * r.arr[2][0], l.arr[3][0] * r.arr[3][0],
						l.arr[0][1] * r.arr[0][1], l.arr[1][1] * r.arr[1][1], l.arr[2][1] * r.arr[2][1], l.arr[3][1] * r.arr[3][1],
						l.arr[0][2] * r.arr[0][2], l.arr[1][2] * r.arr[1][2], l.arr[2][2] * r.arr[2][2], l.arr[3][2] * r.arr[3][2],
						l.arr[0][3] * r.arr[0][3], l.arr[1][3] * r.arr[1][3], l.arr[2][3] * r.arr[2][3], l.arr[3][3] * r.arr[3][3]);
	}
	
	// componentwise matrix_cell * scalar
	inline constexpr float4x4 operator* (float4x4 const& l, float r) {
		return float4x4(l.arr[0][0] * r, l.arr[1][0] * r, l.arr[2][0] * r, l.arr[3][0] * r,
						l.arr[0][1] * r, l.arr[1][1] * r, l.arr[2][1] * r, l.arr[3][1] * r,
						l.arr[0][2] * r, l.arr[1][2] * r, l.arr[2][2] * r, l.arr[3][2] * r,
						l.arr[0][3] * r, l.arr[1][3] * r, l.arr[2][3] * r, l.arr[3][3] * r);
	}
	
	// componentwise scalar * matrix_cell
	inline constexpr float4x4 operator* (float l, float4x4 const& r) {
		return float4x4(l * r.arr[0][0], l * r.arr[1][0], l * r.arr[2][0], l * r.arr[3][0],
						l * r.arr[0][1], l * r.arr[1][1], l * r.arr[2][1], l * r.arr[3][1],
						l * r.arr[0][2], l * r.arr[1][2], l * r.arr[2][2], l * r.arr[3][2],
						l * r.arr[0][3], l * r.arr[1][3], l * r.arr[2][3], l * r.arr[3][3]);
	}
	
	
	// componentwise matrix_cell / matrix_cell
	inline constexpr float4x4 div_componentwise (float4x4 const& l, float4x4 const& r) {
		return float4x4(l.arr[0][0] / r.arr[0][0], l.arr[1][0] / r.arr[1][0], l.arr[2][0] / r.arr[2][0], l.arr[3][0] / r.arr[3][0],
						l.arr[0][1] / r.arr[0][1], l.arr[1][1] / r.arr[1][1], l.arr[2][1] / r.arr[2][1], l.arr[3][1] / r.arr[3][1],
						l.arr[0][2] / r.arr[0][2], l.arr[1][2] / r.arr[1][2], l.arr[2][2] / r.arr[2][2], l.arr[3][2] / r.arr[3][2],
						l.arr[0][3] / r.arr[0][3], l.arr[1][3] / r.arr[1][3], l.arr[2][3] / r.arr[2][3], l.arr[3][3] / r.arr[3][3]);
	}
	
	// componentwise matrix_cell / scalar
	inline constexpr float4x4 operator/ (float4x4 const& l, float r) {
		return float4x4(l.arr[0][0] / r, l.arr[1][0] / r, l.arr[2][0] / r, l.arr[3][0] / r,
						l.arr[0][1] / r, l.arr[1][1] / r, l.arr[2][1] / r, l.arr[3][1] / r,
						l.arr[0][2] / r, l.arr[1][2] / r, l.arr[2][2] / r, l.arr[3][2] / r,
						l.arr[0][3] / r, l.arr[1][3] / r, l.arr[2][3] / r, l.arr[3][3] / r);
	}
	
	// componentwise scalar / matrix_cell
	inline constexpr float4x4 operator/ (float l, float4x4 const& r) {
		return float4x4(l / r.arr[0][0], l / r.arr[1][0], l / r.arr[2][0], l / r.arr[3][0],
						l / r.arr[0][1], l / r.arr[1][1], l / r.arr[2][1], l / r.arr[3][1],
						l / r.arr[0][2], l / r.arr[1][2], l / r.arr[2][2], l / r.arr[3][2],
						l / r.arr[0][3], l / r.arr[1][3], l / r.arr[2][3], l / r.arr[3][3]);
	}
	
	// Matrix ops
	
	
	// matrix-matrix multiply
	inline float4x4 operator* (float4x4 const& l, float4x4 const& r) {
		float4x4 ret;
		ret.arr[0] = l * r.arr[0];
		ret.arr[1] = l * r.arr[1];
		ret.arr[2] = l * r.arr[2];
		ret.arr[3] = l * r.arr[3];
		return ret;
	}
	
	// matrix-vector multiply
	inline float4 operator* (float4x4 const& l, float4 r) {
		float4 ret;
		ret[0] = l.arr[0].x * r.x + l.arr[1].x * r.y + l.arr[2].x * r.z + l.arr[3].x * r.w;
		ret[1] = l.arr[0].y * r.x + l.arr[1].y * r.y + l.arr[2].y * r.z + l.arr[3].y * r.w;
		ret[2] = l.arr[0].z * r.x + l.arr[1].z * r.y + l.arr[2].z * r.z + l.arr[3].z * r.w;
		ret[3] = l.arr[0].w * r.x + l.arr[1].w * r.y + l.arr[2].w * r.z + l.arr[3].w * r.w;
		return ret;
	}
	
	// vector-matrix multiply
	inline float4 operator* (float4 l, float4x4 const& r) {
		float4 ret;
		ret[0] = l.x * r.arr[0].x + l.y * r.arr[0].y + l.z * r.arr[0].z + l.w * r.arr[0].w;
		ret[1] = l.x * r.arr[1].x + l.y * r.arr[1].y + l.z * r.arr[1].z + l.w * r.arr[1].w;
		ret[2] = l.x * r.arr[2].x + l.y * r.arr[2].y + l.z * r.arr[2].z + l.w * r.arr[2].w;
		ret[3] = l.x * r.arr[3].x + l.y * r.arr[3].y + l.z * r.arr[3].z + l.w * r.arr[3].w;
		return ret;
	}
	
	inline constexpr float4x4 transpose (float4x4 const& m) {
		return float4x4::rows(m.arr[0], m.arr[1], m.arr[2], m.arr[3]);
	}
	
	#define LETTERIFY \
	float a = mat.arr[0][0]; \
	float b = mat.arr[0][1]; \
	float c = mat.arr[0][2]; \
	float d = mat.arr[0][3]; \
	float e = mat.arr[1][0]; \
	float f = mat.arr[1][1]; \
	float g = mat.arr[1][2]; \
	float h = mat.arr[1][3]; \
	float i = mat.arr[2][0]; \
	float j = mat.arr[2][1]; \
	float k = mat.arr[2][2]; \
	float l = mat.arr[2][3]; \
	float m = mat.arr[3][0]; \
	float n = mat.arr[3][1]; \
	float o = mat.arr[3][2]; \
	float p = mat.arr[3][3];
	
	inline float determinant (float4x4 const& mat) {
		LETTERIFY
		
		return +a*(+f*(k*p - l*o) -g*(j*p - l*n) +h*(j*o - k*n))
			   -b*(+e*(k*p - l*o) -g*(i*p - l*m) +h*(i*o - k*m))
			   +c*(+e*(j*p - l*n) -f*(i*p - l*m) +h*(i*n - j*m))
			   -d*(+e*(j*o - k*n) -f*(i*o - k*m) +g*(i*n - j*m));
	}
	
	inline float4x4 inverse (float4x4 const& mat) {
		LETTERIFY
		
		float det;
		{ // clac determinate
			det = +a*(+f*(k*p - l*o) -g*(j*p - l*n) +h*(j*o - k*n))
				  -b*(+e*(k*p - l*o) -g*(i*p - l*m) +h*(i*o - k*m))
				  +c*(+e*(j*p - l*n) -f*(i*p - l*m) +h*(i*n - j*m))
				  -d*(+e*(j*o - k*n) -f*(i*o - k*m) +g*(i*n - j*m));
		}
		// calc cofactor matrix
		
		float cofac_00 = +f*(k*p - l*o) -g*(j*p - l*n) +h*(j*o - k*n);
		float cofac_01 = +e*(k*p - l*o) -g*(i*p - l*m) +h*(i*o - k*m);
		float cofac_02 = +e*(j*p - l*n) -f*(i*p - l*m) +h*(i*n - j*m);
		float cofac_03 = +e*(j*o - k*n) -f*(i*o - k*m) +g*(i*n - j*m);
		float cofac_10 = +b*(k*p - l*o) -c*(j*p - l*n) +d*(j*o - k*n);
		float cofac_11 = +a*(k*p - l*o) -c*(i*p - l*m) +d*(i*o - k*m);
		float cofac_12 = +a*(j*p - l*n) -b*(i*p - l*m) +d*(i*n - j*m);
		float cofac_13 = +a*(j*o - k*n) -b*(i*o - k*m) +c*(i*n - j*m);
		float cofac_20 = +b*(g*p - h*o) -c*(f*p - h*n) +d*(f*o - g*n);
		float cofac_21 = +a*(g*p - h*o) -c*(e*p - h*m) +d*(e*o - g*m);
		float cofac_22 = +a*(f*p - h*n) -b*(e*p - h*m) +d*(e*n - f*m);
		float cofac_23 = +a*(f*o - g*n) -b*(e*o - g*m) +c*(e*n - f*m);
		float cofac_30 = +b*(g*l - h*k) -c*(f*l - h*j) +d*(f*k - g*j);
		float cofac_31 = +a*(g*l - h*k) -c*(e*l - h*i) +d*(e*k - g*i);
		float cofac_32 = +a*(f*l - h*j) -b*(e*l - h*i) +d*(e*j - f*i);
		float cofac_33 = +a*(f*k - g*j) -b*(e*k - g*i) +c*(e*j - f*i);
		
		float4x4 ret;
		
		float inv_det = float(1) / det;
		float ninv_det = -inv_det;
		
		ret.arr[0][0] = cofac_00 *  inv_det;
		ret.arr[0][1] = cofac_10 * ninv_det;
		ret.arr[0][2] = cofac_20 *  inv_det;
		ret.arr[0][3] = cofac_30 * ninv_det;
		ret.arr[1][0] = cofac_01 * ninv_det;
		ret.arr[1][1] = cofac_11 *  inv_det;
		ret.arr[1][2] = cofac_21 * ninv_det;
		ret.arr[1][3] = cofac_31 *  inv_det;
		ret.arr[2][0] = cofac_02 *  inv_det;
		ret.arr[2][1] = cofac_12 * ninv_det;
		ret.arr[2][2] = cofac_22 *  inv_det;
		ret.arr[2][3] = cofac_32 * ninv_det;
		ret.arr[3][0] = cofac_03 * ninv_det;
		ret.arr[3][1] = cofac_13 *  inv_det;
		ret.arr[3][2] = cofac_23 * ninv_det;
		ret.arr[3][3] = cofac_33 *  inv_det;
		
		return ret;
	}
	
	#undef LETTERIFY
	
}

