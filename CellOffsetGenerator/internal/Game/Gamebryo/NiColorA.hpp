#pragma once

#include "BSMemObject.hpp"
#include "NiColor.hpp"

class NiColorA {
public:
	float r;
	float g;
	float b;
	float a;

	inline const float operator[] (uint32_t i) const { return ((float*)&r)[i]; };
	inline float operator[] (uint32_t i) { return ((float*)&r)[i]; };

	operator float* () const { return (float*)this; };

	NiColorA() : r(0.f), g(0.f), b(0.f), a(0.f) {};
	NiColorA(const NiColor& color) : r(color.r), g(color.g), b(color.b), a(1.0f) {};
	NiColorA(const NiColor& color, float a) : r(color.r), g(color.g), b(color.b), a(a) {};
	NiColorA(float r, float g, float b) : r(r), g(g), b(b), a(1.0f) {};
	NiColorA(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {};
	NiColorA(float f) : r(f), g(f), b(f), a(f) {};

	NiColorA operator*(const float f) const {
		return NiColorA(r * f, g * f, b * f, a * f);
	};

	NiColorA operator*(const NiColorA& color) const {
		return NiColorA(r * color.r, g * color.g, b * color.b, a * color.a);
	};

	bool operator==(const NiColorA& color) const {
		return r == color.r && g == color.g && b == color.b && a == color.a;
	};

	bool operator==(const float f) const {
		return r == f && g == f && b == f && a == f;
	};

	bool operator!=(const NiColorA& color) const {
		return r != color.r || g != color.g || b != color.b || a != color.a;
	};

	bool operator>(const float f) const {
		return r > f && g > f && b > f && a > f;
	}

	bool operator<(const float f) const {
		return r < f && g < f && b < f && a < f;
	}

	NiColorA operator*=(const float f) {
		r *= f;
		g *= f;
		b *= f;
		a *= f;
		return *this;
	};

	NiColorA operator=(const NiColor& color) {
		r = color.r;
		g = color.g;
		b = color.b;
		return *this;
	};

	NiColorA operator+(const NiColorA& color) const {
		return NiColorA(r + color.r, g + color.g, b + color.b, a + color.a);
	}

	bool IsVisible() const {
		return a > 0.f && (r > 0.f || g > 0.f || b > 0.f);
	}

	static const NiColorA Black;
	static const NiColorA White;
	static const NiColorA Red;
	static const NiColorA Green;
	static const NiColorA Blue;
	static const NiColorA Yellow;
	static const NiColorA Cyan;
	static const NiColorA Magenta;

	static NiColorA& GetRandomColor(float afAlpha = 1.f) {
		static NiColorA kColor;
		kColor.r = (float)(rand() % 255) / 255.f;
		kColor.g = (float)(rand() % 255) / 255.f;
		kColor.b = (float)(rand() % 255) / 255.f;
		kColor.a = afAlpha;
		return kColor;
	}
};

ASSERT_SIZE(NiColorA, 0x10);