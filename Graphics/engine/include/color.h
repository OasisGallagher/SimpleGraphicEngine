#pragma once
#include "utility.h"

struct ColorBase {
	unsigned r, g, b;
};

struct ColorDiff {
	float dr, dg, db;
	ColorDiff() :dr(0), dg(0), db(0) {
	}
	ColorDiff operator / (float x) const;
	ColorDiff operator * (float x) const;
};

struct Color : ColorBase {
	Color(unsigned char R, unsigned G, unsigned B) {
		r = R, g = G, b = B;
	}

	Color(unsigned u = 0) {
		r = (u & 0xff0000) >> 16;
		g = (u & 0xff00) >> 8;
		b = u & 0xff;
	}

	Color& operator = (unsigned u) {
		r = (u & 0xff0000) >> 16;
		g = (u & 0xff00) >> 8;
		b = u & 0xff;
		return *this;
	}

	ColorDiff operator - (const Color& other) const {
		ColorDiff diff;
		diff.dr =  float(r - other.r), diff.dg = float(g - other.g), diff.db = float(b - other.b);
		return diff;
	}

	Color operator + (const ColorDiff& other) const {
		return Color(r + (int)other.dr, g + (int)other.dg, b + (int)other.db);
	}

	operator unsigned() const {
		return r | ((unsigned)g) << 8 | ((unsigned)b) << 16;
	}

	Color& operator += (const ColorDiff& diff) {
		r = Min(255, r + (int)diff.dr);
		g = Min(255, g + (int)diff.dg);
		b = Min(255, b + (int)diff.db);
		return *this;
	}

	Color& operator *= (const Color& other) {
		r = (int)r * other.r / 256;
		g = (int)g * other.g / 256;
		b = (int)b * other.b / 256;
		return *this;
	}
};

inline
ColorDiff ColorDiff::operator / (float x) const {
	ColorDiff diff(*this);
	diff.dr /= x, diff.dg /= x, diff.db /= x;
	return diff;
}

inline
ColorDiff ColorDiff::operator * (float x) const {
	ColorDiff diff;
	diff.dr *= x, diff.dg *= x, diff.db *= x;
	return diff;
}
