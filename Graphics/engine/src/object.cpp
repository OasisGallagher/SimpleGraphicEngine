#include "object.h"

Object::Object() {
}

void Object::Initialize(const VertexContainer& vertices, const IndexedTriangleContainer& container,
						float radius, float scale, const Vector3& position, const Vector3& eulerAngles) {
	this->vertices = vertices;
	this->triangles = container;
	this->maxRadius = radius;
	this->scale = scale;
	this->worldPosition = position;
	this->eulerAngles = eulerAngles;

	for (iterator ite = triangles.begin(); ite != triangles.end(); ++ite) {
		ite->vertexContainer = &transformedVertices;
	}

	for (VertexContainer::iterator ite = this->vertices.begin(); ite != this->vertices.end(); ++ite) {
		Vector3 tmp = *ite;
		tmp *= scale;
		*ite = tmp;
	}
}

void Object::ResetState() {
	BaseType::ResetState();
	transformedVertices = vertices;
}

void Object::RefreshNormal() {
	std::vector<size_t> count(VertexCount());

	for (iterator ite = triangles.begin(); ite != triangles.end(); ++ite) {
		size_t first, second, third;
		ite->Indices(first, second, third);

		++count[first], ++count[second], ++count[third];

		Vector3 n = ite->Normal();

		transformedVertices[first].normal += n;
		transformedVertices[second].normal += n;
		transformedVertices[third].normal += n;
	}

	for (size_t i = 0; i < vertices.size(); ++i) {
		if (count[i] >= 1) {
			transformedVertices[i].normal /= float(count[i]);
			transformedVertices[i].normal.Normalize();
		}
	}
}
