#include "transformation.h"

Transformation::Transformation(float rotX, float rotY, float rotZ) {
	matrix = Matrix::identity;

	float sf, cf;
	if (!Approximately(rotX)) {
		sf = sinf(rotX), cf = cosf(rotX);
		matrix *= Matrix(
			1, 0, 0, 0,
			0, cf, sf, 0,
			0, -sf, cf, 0,
			0, 0, 0, 1
			);
	}

	if (!Approximately(rotY)) {
		sf = sinf(rotY), cf = cosf(rotY);
		matrix *= Matrix(
			cf, 0, -sf, 0,
			0, 1, 0, 0,
			sf, 0, cf, 0,
			0, 0, 0, 1
			);
	}

	if (!Approximately(rotZ)) {
		sf = sinf(rotZ), cf = cosf(rotZ);
		matrix *= Matrix(
			cf, sf, 0, 0,
			-sf, cf, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
			);
	}
}

Vector3 Transformation::operator * (const Vector3& other) const {
	float w = other.x * matrix.p03 + other.y * matrix.p13 + other.z * matrix.p23 + matrix.p33;
	AssertEx(!Approximately(w), "invalid homogeneous coordinates");

	return Vector3((other.x * matrix.p00 + other.y * matrix.p10 + other.z * matrix.p20 + matrix.p30) / w,
				(other.x * matrix.p01 + other.y * matrix.p11 + other.z * matrix.p21 + matrix.p31) / w,
				(other.x * matrix.p02 + other.y * matrix.p12 + other.z * matrix.p22 + matrix.p32) / w);
}

Transformation Transformation::operator * (const Transformation& other) const {
	return Transformation(matrix * other.matrix);
}

Transformation& Transformation::operator *= (const Transformation& other) {
	matrix *= other.matrix;
	return *this;
}
