/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once


namespace Lantern {

template<typename T>
struct Vec4 {
	Vec4() {};
	Vec4(T s)
		: X(s), 
		  Y(s), 
		  Z(s), 
		  W(s) {
	}
	Vec4(T x, T y, T z, T w)
		: X(x), 
		  Y(y), 
		  Z(z), 
		  W(w) {
	}
	T X, Y, Z, W;

	Vec4 operator*(T s) const {
		return Vec4(X*s, Y*s, Z*s, W*s);
	}

	Vec4 operator+(const Vec4& a) const {
		return Vec4(X + a.X, Y + a.Y, Z + a.Z, W + a.W);
	}
};




template<typename T>
struct Vec3 {
	Vec3() {
	};
	Vec3(T s)
		: X(s),
		  Y(s),
		  Z(s) {
	}
	Vec3(T x, T y, T z)
		: X(x),
		  Y(y),
		  Z(z) {
	}
	T X, Y, Z;

	Vec3 operator*(T s) const {
		return Vec3(X*s, Y*s, Z*s);
	}

	Vec3 operator+(const Vec3& a) const {
		return Vec3(X + a.X, Y + a.Y, Z + a.Z);
	}
};


template<typename T>
struct Vec2 {
	Vec2() {
	};
	Vec2(T s)
		: X(s),
		  Y(s) {
	}
	Vec2(T x, T y)
		: X(x),
		  Y(y) {
	}
	T X, Y;

	Vec2 operator*(T s) const {
		return Vec2(X*s, Y*s);
	}

	Vec2 operator+(const Vec2& a) const {
		return Vec2(X + a.X, Y + a.Y);
	}
};


// float vectors
typedef Vec2<float> float2;
typedef Vec3<float> float3;
typedef Vec4<float> float4;


// Dot products
inline float dot(const float2& a, const float2& b) {
	return a.X * b.X + a.Y * b.Y;
}

inline float dot(const float3& a, const float3& b) {
	return a.X * b.X + a.Y * b.Y + a.Z * b.Z;
}

inline float dot(const float4& a, const float4& b) {
	return a.X * b.X + a.Y * b.Y + a.Z * b.Z + a.W * b.W;
}

} // End of namespace Lantern
