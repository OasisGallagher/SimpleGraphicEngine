#pragma once
#include "matrix.h"
#include "vector3.h"
#include "utility.h"

struct TransformationBase {
protected:
	Matrix matrix;
};

struct Transformation : TransformationBase {
	Transformation();
	Transformation(const Vector3& dist);
	Transformation(float rotX, float rotY, float rotZ);
	Transformation(Vector3& axis, float rot);
	Transformation(const Matrix& matrix);

	Transformation& operator = (const Matrix& matrix);

	Vector3 operator * (const Vector3& other) const;
	Transformation operator * (const Transformation& other) const;
	Transformation& operator *= (const Transformation& other);
};

inline
Vector3 operator * (const Vector3& lhs, const Transformation& rhs) {
	return rhs * lhs;
}

inline
Vector3& operator *= (Vector3& lhs, const Transformation& rhs) {
	return lhs = rhs * lhs;
}

inline
Transformation::Transformation() {
	matrix = Matrix::identity;
}

inline
Transformation::Transformation(const Matrix& matrix) {
	this->matrix = matrix;
}

inline
Transformation::Transformation(const Vector3& dist) {
	matrix = Matrix::identity;
	matrix(3, dist);
}

inline
Transformation::Transformation(Vector3& axis, float rot) {
	if (!Approximately(axis.LengthSquared(), 1))
		axis.Normalize();

	float u = axis.x, v = axis.y, w = axis.z;
	float cf = cosf(rot), sf = sinf(rot);

	matrix(0, Vector3(cf + u * u * (1 - cf), u * v * (1 - cf) + w * sf, u * w * (1 - cf) - v * sf));
	matrix(1, Vector3(u * v * (1 - cf) - w * sf, cf + v * v * (1 - cf), w * v * (1 - cf) + u * sf));
	matrix(2, Vector3(u * w * (1 - cf) + v * sf, v * w * (1 - cf) - u * sf, cf + w * w * (1 - cf)));
	matrix(3, 3) = 1;
}

inline
Transformation& Transformation::operator = (const Matrix& matrix) {
	this->matrix = matrix;
	return *this;
}
