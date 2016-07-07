#include "vertex.h"

Vertex::operator Vector3() const {
	return Vector3(x, y, z);
}

Vertex& Vertex::operator = (const Vector3& other) {
	x = other.x, y = other.y, z = other.z;
	return *this;
}