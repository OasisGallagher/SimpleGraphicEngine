#pragma once
#include "utility.h"

struct MatrixBase {
	union {
		float buffer[4][4];
		struct {
			float p00, p01, p02, p03;
			float p10, p11, p12, p13;
			float p20, p21, p22, p23;
			float p30, p31, p32, p33;
		};
	};
};

struct Vector3;

struct Matrix : MatrixBase {
	Matrix(float x00 = 0, float x01 = 0, float x02 = 0, float x03 = 0,
	float x10 = 0, float x11 = 0, float x12 = 0, float x13 = 0,
	float x20 = 0, float x21 = 0, float x22 = 0, float x23 = 0,
	float x30 = 0, float x31 = 0, float x32 = 0, float x33 = 0
	);

	float& operator()(size_t row, size_t col);
	float operator()(size_t row, size_t col) const;
	void operator()(size_t row, const Vector3& other);

	Matrix operator * (const Matrix& matrix) const;
	Matrix& operator *= (const Matrix& matrix);

	static Matrix identity;
	static Matrix zero;
};

inline
Matrix::Matrix(float x00, float x01, float x02, float x03,
float x10, float x11, float x12, float x13,
float x20, float x21, float x22, float x23,
float x30, float x31, float x32, float x33) {
	p00 = x00, p01 = x01, p02 = x02, p03 = x03;
	p10 = x10, p11 = x11, p12 = x12, p13 = x13;
	p20 = x20, p21 = x21, p22 = x22, p23 = x23;
	p30 = x30, p31 = x31, p32 = x32, p33 = x33;
}

inline
float& Matrix::operator()(size_t row, size_t col) {
	AssertEx(row < 4 && col < 4, "invalid index");
	return buffer[row][col];
}

inline
float Matrix::operator()(size_t row, size_t col) const {
	AssertEx(row < 4 && col < 4, "invalid index");
	return buffer[row][col];
}
