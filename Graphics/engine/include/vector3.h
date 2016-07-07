#pragma once
#include <cmath>
#include "utility.h"

struct Vector3Base {
	float x, y, z;
};

struct Matrix;
struct Vertex;

struct Vector3 : Vector3Base {
	Vector3(float x = 0, float y = 0, float z = 0);

	Vector3 operator * (float other) const;
	Vector3 operator / (float other) const;
	Vector3 operator + (const Vector3& other) const;
	Vector3 operator - (const Vector3& other) const;

	Vector3& operator *= (float other);
	Vector3& operator /= (float other);
	Vector3& operator += (const Vector3& other);
	Vector3& operator -= (const Vector3& other);

	bool operator == (const Vector3& other) const;

	Vector3 operator - ()const;

	void Set(float x, float y, float z);

	float Dot(const Vector3& other) const;
	Vector3 Cross(const Vector3& other) const;

	float LengthSquared() const;
	float Length() const;

	Vector3 Normalized() const;
	void Normalize();

	static Vector3 up, right, forward;
};

inline
Vector3::Vector3(float x, float y, float z) {
	this->x = x, this->y = y, this->z = z;
}

inline
void Vector3::Set(float x, float y, float z) {
	this->x = x, this->y = y, this->z = z;
}

inline
Vector3 Vector3::operator * (float other) const {
	return Vector3(x * other, y * other, z * other);
}

inline
Vector3 Vector3::operator / (float other) const {
	AssertEx(!Approximately(other), "invalid divisor");
	return Vector3(x / other, y / other, z / other);
}

inline
Vector3 Vector3::operator + (const Vector3& other) const {
	return Vector3(x + other.x, y + other.y, z + other.z);
}

inline
Vector3 Vector3::operator - (const Vector3& other) const {
	return Vector3(x - other.x, y - other.y, z - other.z);
}

inline
Vector3& Vector3::operator *= (float other) {
	x *= other, y *= other, z *= other;
	return *this;
}

inline
Vector3& Vector3::operator /= (float other) {
	AssertEx(!Approximately(other), "invalid divisor");
	x /= other, y /= other, z /= other;
	return *this;
}

inline
Vector3& Vector3::operator += (const Vector3& other) {
	x += other.x, y += other.y, z += other.z;
	return *this;
}

inline
Vector3& Vector3::operator -= (const Vector3& other) {
	x -= other.x, y -= other.y, z -= other.z;
	return *this;
}

inline
Vector3 Vector3::operator - () const {
	return Vector3(-x, -y, -z);
}

inline
bool Vector3::operator == (const Vector3& other) const {
	return Approximately(x, other.x) && Approximately(y, other.y) && Approximately(z, other.z);
}

inline
float Vector3::Dot(const Vector3& other) const {
	return x * other.x + y * other.y + z * other.z;
}

inline
Vector3 Vector3::Cross(const Vector3& other) const {
	return Vector3(
		y * other.z - z * other.y,
		z * other.x - x * other.z,
		x * other.y - y * other.x
		);
}

inline
float Vector3::LengthSquared() const {
	return x * x + y * y + z * z;
}

inline
float Vector3::Length() const {
	return sqrtf(LengthSquared());
}

inline
Vector3 Vector3::Normalized() const {
	Vector3 ans = *this;
	ans.Normalize();
	return ans;
}

inline
void Vector3::Normalize() {
	float length = Length();
	*this /= length;
}
