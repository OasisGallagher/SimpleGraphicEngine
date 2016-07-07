#pragma once
#include "color.h"
#include "vector3.h"

struct VertexBase {
	float x, y, z;
	Color color;
	float u, v;
	Vector3 normal;
};

struct Vertex : VertexBase {
	Vertex(float x = 0, float y = 0, float z = 0, float u = -1, float v = -1);
	operator Vector3() const;
	Vertex& operator = (const Vector3& other);
};

inline
Vertex::Vertex(float x, float y, float z, float u, float v) {
	this->x = x, this->y = y, this->z = z;
	this->u = u, this->v = v;
}
