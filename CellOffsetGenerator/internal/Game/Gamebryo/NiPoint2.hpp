#pragma once

class NiPoint2 {
public:
	float x;
	float y;

	NiPoint2() : x(0.0f), y(0.0f) {};
	NiPoint2(float x, float y) : x(x), y(y) {};
	NiPoint2(float xy) : x(xy), y(xy) {};

	auto operator<=>(const NiPoint2&) const = default;

	inline const float operator[] (uint32_t i) const { return ((float*)&x)[i]; };
	inline float operator[] (uint32_t i) { return ((float*)&x)[i]; };

	inline operator float* () const { return (float*)this; };

	// Point operations
	inline NiPoint2 operator+(const NiPoint2& arPoint) const { return NiPoint2(x + arPoint.x, y + arPoint.y); }

	inline NiPoint2 operator-(const NiPoint2& arPoint) const { return NiPoint2(x - arPoint.x, y - arPoint.y); }

	inline NiPoint2 operator*(const NiPoint2& arPoint) const { return NiPoint2(x * arPoint.x, y * arPoint.y); }

	inline NiPoint2 operator/(const NiPoint2& arPoint) const { return NiPoint2(x / arPoint.x, y / arPoint.y); }

	inline NiPoint2 operator-() const { return NiPoint2(-x, -y); }

	inline NiPoint2& operator+=(const NiPoint2& arPoint) {
		x += arPoint.x;
		y += arPoint.y;
		return *this;
	}

	inline NiPoint2& operator-=(const NiPoint2& arPoint) {
		x -= arPoint.x;
		y -= arPoint.y;
		return *this;
	}

	inline NiPoint2& operator*=(const NiPoint2& arPoint) {
		x *= arPoint.x;
		y *= arPoint.y;
		return *this;
	}

	inline NiPoint2& operator/=(const NiPoint2& arPoint) {
		x /= arPoint.x;
		y /= arPoint.y;
		return *this;
	}

	inline NiPoint2 operator+(float afScalar) const { return NiPoint2(x + afScalar, y + afScalar); }

	inline NiPoint2 operator-(float afScalar) const { return NiPoint2(x - afScalar, y - afScalar); }

	inline NiPoint2 operator*(float afScalar) const { return NiPoint2(x * afScalar, y * afScalar); }

	inline NiPoint2 operator/(float afScalar) const { return NiPoint2(x / afScalar, y / afScalar); }

	inline NiPoint2& operator+=(float afScalar) {
		x += afScalar;
		y += afScalar;
		return *this;
	}

	inline NiPoint2& operator-=(float afScalar) {
		x -= afScalar;
		y -= afScalar;
		return *this;
	}

	inline NiPoint2& operator*=(float afScalar) {
		x *= afScalar;
		y *= afScalar;
		return *this;
	}

	inline NiPoint2& operator/=(float afScalar) {
		x /= afScalar;
		y /= afScalar;
		return *this;
	}

	float Length() const {
		return sqrt(x * x + y * y);
	}

	float SqrLength() const {
		return x * x + y * y;
	}

	__forceinline float Unitize() {
		float fLength = Length();

		if (fLength > 1e-06f) {
			float fRecip = 1.0f / fLength;
			x *= fRecip;
			y *= fRecip;
		}
		else
		{
			x = 0.0f;
			y = 0.0f;
			fLength = 0.0f;
		}
		return fLength;
	}
};

typedef __declspec(align(8)) NiPoint2 NiPoint2A;