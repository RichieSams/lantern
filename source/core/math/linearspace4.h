// ======================================================================== //
// Copyright 2009-2016 Intel Corporation                                    //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#pragma once

#include <math/vec4.h>

#include <iostream>



namespace embree {
struct LinearSpace4;
LinearSpace4 operator*(const float &a, const LinearSpace4 &b);

////////////////////////////////////////////////////////////////////////////////
/// 3D Linear Transform (4x4 Matrix)
////////////////////////////////////////////////////////////////////////////////
struct LinearSpace4 {
	/*! default matrix constructor */
	__forceinline LinearSpace4() {}
	__forceinline LinearSpace4(const LinearSpace4 &other) { vx = other.vx; vy = other.vy; vz = other.vz; vw = other.vw; }
	__forceinline LinearSpace4 &operator=(const LinearSpace4 &other) { vx = other.vx; vy = other.vy; vz = other.vz; vw = other.vw; return *this; }

	/*! matrix construction from column Vec4fs */
	__forceinline LinearSpace4(const Vec4f &vx, const Vec4f &vy, const Vec4f &vz, const Vec4f &vw)
		: vx(vx), vy(vy), vz(vz), vw(vw) {}

	/*! matrix construction from row mayor data */
	__forceinline LinearSpace4(const float &m00, const float &m01, const float &m02, const float &m03,
	                           const float &m10, const float &m11, const float &m12, const float &m13,
	                           const float &m20, const float &m21, const float &m22, const float &m23,
	                           const float &m30, const float &m31, const float &m32, const float &m33)
		: vx(m00, m10, m20, m30), vy(m01, m11, m21, m31), vz(m02, m12, m22, m32), vw(m03, m13, m23, m33) {}

	/*! compute the determinant of the matrix */
	__forceinline const float det() const {
		return vw.x * vz.y * vy.z * vx.w - vz.x * vw.y * vy.z * vx.w -
		       vw.x * vy.y * vz.z * vx.w + vy.x * vw.y * vz.z * vx.w +
		       vz.x * vy.y * vw.z * vx.w - vy.x * vz.y * vw.z * vx.w -
		       vw.x * vz.y * vx.z * vy.w + vz.x * vw.y * vx.z * vy.w +
		       vw.x * vx.y * vz.z * vy.w - vx.x * vw.y * vz.z * vy.w -
		       vz.x * vx.y * vw.z * vy.w + vx.x * vz.y * vw.z * vy.w +
		       vw.x * vy.y * vx.z * vz.w - vy.x * vw.y * vx.z * vz.w -
		       vw.x * vx.y * vy.z * vz.w + vx.x * vw.y * vy.z * vz.w +
		       vy.x * vx.y * vw.z * vz.w - vx.x * vy.y * vw.z * vz.w -
		       vz.x * vy.y * vx.z * vw.w + vy.x * vz.y * vx.z * vw.w +
		       vz.x * vx.y * vy.z * vw.w - vx.x * vz.y * vy.z * vw.w -
		       vy.x * vx.y * vz.z * vw.w + vx.x * vy.y * vz.z * vw.w;
	}

	/*! compute inverse matrix */
	__forceinline const LinearSpace4 inverse() const {
		LinearSpace4 inv(
			 vy.y * vz.z * vw.w - vy.y * vz.w * vw.z - vz.y * vy.z * vw.w + vz.y * vy.w * vw.z + vw.y * vy.z * vz.w - vw.y * vy.w * vz.z,
			-vy.x * vz.z * vw.w + vy.x * vz.w * vw.z + vz.x * vy.z * vw.w - vz.x * vy.w * vw.z - vw.x * vy.z * vz.w + vw.x * vy.w * vz.z,
			 vy.x * vz.y * vw.w - vy.x * vz.w * vw.y - vz.x * vy.y * vw.w + vz.x * vy.w * vw.y + vw.x * vy.y * vz.w - vw.x * vy.w * vz.y,
			-vy.x * vz.y * vw.z + vy.x * vz.z * vw.y + vz.x * vy.y * vw.z - vz.x * vy.z * vw.y - vw.x * vy.y * vz.z + vw.x * vy.z * vz.y,
			-vx.y * vz.z * vw.w + vx.y * vz.w * vw.z + vz.y * vx.z * vw.w - vz.y * vx.w * vw.z - vw.y * vx.z * vz.w + vw.y * vx.w * vz.z,
			 vx.x * vz.z * vw.w - vx.x * vz.w * vw.z - vz.x * vx.z * vw.w + vz.x * vx.w * vw.z + vw.x * vx.z * vz.w - vw.x * vx.w * vz.z,
			-vx.x * vz.y * vw.w + vx.x * vz.w * vw.y + vz.x * vx.y * vw.w - vz.x * vx.w * vw.y - vw.x * vx.y * vz.w + vw.x * vx.w * vz.y,
			 vx.x * vz.y * vw.z - vx.x * vz.z * vw.y - vz.x * vx.y * vw.z + vz.x * vx.z * vw.y + vw.x * vx.y * vz.z - vw.x * vx.z * vz.y,
			 vx.y * vy.z * vw.w - vx.y * vy.w * vw.z - vy.y * vx.z * vw.w + vy.y * vx.w * vw.z + vw.y * vx.z * vy.w - vw.y * vx.w * vy.z,
			-vx.x * vy.z * vw.w + vx.x * vy.w * vw.z + vy.x * vx.z * vw.w - vy.x * vx.w * vw.z - vw.x * vx.z * vy.w + vw.x * vx.w * vy.z,
			 vx.x * vy.y * vw.w - vx.x * vy.w * vw.y - vy.x * vx.y * vw.w + vy.x * vx.w * vw.y + vw.x * vx.y * vy.w - vw.x * vx.w * vy.y,
			-vx.x * vy.y * vw.z + vx.x * vy.z * vw.y + vy.x * vx.y * vw.z - vy.x * vx.z * vw.y - vw.x * vx.y * vy.z + vw.x * vx.z * vy.y,
			-vx.y * vy.z * vz.w + vx.y * vy.w * vz.z + vy.y * vx.z * vz.w - vy.y * vx.w * vz.z - vz.y * vx.z * vy.w + vz.y * vx.w * vy.z,
			 vx.x * vy.z * vz.w - vx.x * vy.w * vz.z - vy.x * vx.z * vz.w + vy.x * vx.w * vz.z + vz.x * vx.z * vy.w - vz.x * vx.w * vy.z,
			-vx.x * vy.y * vz.w + vx.x * vy.w * vz.y + vy.x * vx.y * vz.w - vy.x * vx.w * vz.y - vz.x * vx.y * vy.w + vz.x * vx.w * vy.y,
			 vx.x * vy.y * vz.z - vx.x * vy.z * vz.y - vy.x * vx.y * vz.z + vy.x * vx.z * vz.y + vz.x * vx.y * vy.z - vz.x * vx.z * vy.y);

		float det = vx.x * inv.vx.x + vx.y * inv.vy.x + vx.z * inv.vz.x + vx.w * inv.vw.x;

		if (det == 0)
			return LinearSpace4(zero);

		det = 1.f / det;

		return det * inv;
	}

	/*! compute transposed matrix */
	__forceinline const LinearSpace4 transpose() const { return LinearSpace4(vx.x, vx.y, vx.z, vx.w, vy.x, vy.y, vy.z, vy.w, vz.x, vz.y, vz.z, vz.w, vw.x, vw.y, vw.z, vw.z); }

	/*! returns first row of matrix */
	__forceinline const Vec4f row0() const { return Vec4f(vx.x, vy.x, vz.x, vw.x); }

	/*! returns second row of matrix */
	__forceinline const Vec4f row1() const { return Vec4f(vx.y, vy.y, vz.y, vw.y); }

	/*! returns third row of matrix */
	__forceinline const Vec4f row2() const { return Vec4f(vx.z, vy.z, vz.z, vw.z); }

	/*! returns fourth row of matrix */
	__forceinline const Vec4f row3() const { return Vec4f(vx.w, vy.w, vz.w, vw.w); }

	////////////////////////////////////////////////////////////////////////////////
	/// Constants
	////////////////////////////////////////////////////////////////////////////////

	__forceinline LinearSpace4(ZeroTy) : vx(zero), vy(zero), vz(zero), vw(zero) {}
	__forceinline LinearSpace4(OneTy) : vx(one, zero, zero, zero), vy(zero, one, zero, zero), vz(zero, zero, one, zero), vw(zero, zero, zero, one) {}

	/*! return matrix for scaling */
	static __forceinline LinearSpace4 scale(const Vec4f &s) {
		return LinearSpace4(s.x, 0, 0, 0,
		                    0, s.y, 0, 0,
		                    0, 0, s.z, 0,
		                    0, 0, 0, 1);
	}

	/*! return matrix for rotation around arbitrary axis */
	static __forceinline LinearSpace4 rotate(const Vec4f &_u, const float &r) {
		Vec4f u = normalize(_u);
		float s = sin(r), c = cos(r);
		return LinearSpace4(u.x * u.x + (1 - u.x * u.x) * c, u.x * u.y * (1 - c) - u.z * s, u.x * u.z * (1 - c) + u.y * s, 0,
		                    u.x * u.y * (1 - c) + u.z * s, u.y * u.y + (1 - u.y * u.y) * c, u.y * u.z * (1 - c) - u.x * s, 0, 
		                    u.x * u.z * (1 - c) - u.y * s, u.y * u.z * (1 - c) + u.x * s, u.z * u.z + (1 - u.z * u.z) * c, 0,
		                    0, 0, 0, 1);
	}

	/*! return matrix for translation vs the origin */
	static __forceinline LinearSpace4 translate(const Vec4f &t) {
		return LinearSpace4(zero, zero, zero, t);
	}

public:
	/*! the column Vec4fs of the matrix */
	Vec4f vx, vy, vz, vw;
};

__forceinline const LinearSpace4 transpose(const LinearSpace4 &xfm) {
	return xfm.transpose();
}

////////////////////////////////////////////////////////////////////////////////
// Unary Operators
////////////////////////////////////////////////////////////////////////////////

__forceinline LinearSpace4 operator-(const LinearSpace4 &a) { return LinearSpace4(-a.vx, -a.vy, -a.vz, -a.vw); }
__forceinline LinearSpace4 operator+(const LinearSpace4 &a) { return LinearSpace4(+a.vx, +a.vy, +a.vz, +a.vw); }
__forceinline LinearSpace4 rcp(const LinearSpace4 &a) { return a.inverse(); }

////////////////////////////////////////////////////////////////////////////////
// Binary Operators
////////////////////////////////////////////////////////////////////////////////

__forceinline LinearSpace4 operator+(const LinearSpace4 &a, const LinearSpace4 &b) { return LinearSpace4(a.vx + b.vx, a.vy + b.vy, a.vz + b.vz, a.vw + b.vw); }
__forceinline LinearSpace4 operator-(const LinearSpace4 &a, const LinearSpace4 &b) { return LinearSpace4(a.vx - b.vx, a.vy - b.vy, a.vz - b.vz, a.vw - b.vw); }

__forceinline LinearSpace4 operator*(const float &a, const LinearSpace4 &b) { return LinearSpace4(a * b.vx, a * b.vy, a * b.vz, a * b.vw); }
__forceinline Vec4f        operator*(const LinearSpace4 &a, const Vec4f &b) { return b.x * a.vx + b.y * a.vy + b.z * a.vz + b.w * a.vw; }
__forceinline LinearSpace4 operator*(const LinearSpace4 &a, const LinearSpace4 &b) { return LinearSpace4(a * b.vx, a * b.vy, a * b.vz, a * b.vw); }

__forceinline LinearSpace4 operator/(const LinearSpace4 &a, const float &b) { return LinearSpace4(a.vx / b, a.vy / b, a.vz / b, a.vw / b); }
__forceinline LinearSpace4 operator/(const LinearSpace4 &a, const LinearSpace4 &b) { return a * rcp(b); }

__forceinline LinearSpace4 &operator*=(LinearSpace4 &a, const LinearSpace4 &b) { return a = a * b; }
__forceinline LinearSpace4 &operator/=(LinearSpace4 &a, const LinearSpace4 &b) { return a = a / b; }

////////////////////////////////////////////////////////////////////////////////
/// Comparison Operators
////////////////////////////////////////////////////////////////////////////////

__forceinline bool operator ==(const LinearSpace4 &a, const LinearSpace4 &b) { return a.vx == b.vx && a.vy == b.vy && a.vz == b.vz && a.vw == b.vw; }
__forceinline bool operator !=(const LinearSpace4 &a, const LinearSpace4 &b) { return a.vx != b.vx || a.vy != b.vy || a.vz != b.vz || a.vw != b.vw; }

////////////////////////////////////////////////////////////////////////////////
/// Output Operators
////////////////////////////////////////////////////////////////////////////////

static std::ostream &operator<<(std::ostream &cout, const LinearSpace4 &m) {
	return cout << "{ vx = " << m.vx << ", vy = " << m.vy << ", vz = " << m.vz << ", vw = " << m.vw << "}";
}

}
