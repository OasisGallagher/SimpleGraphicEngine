#pragma once
#include <vector>
#include "vertex.h"
#include "stateful.h"
#include "vector3.h"
#include "texture.h"

enum TriangleState {
	TriangleStateInactive,
	TriangleStateClipped,
	TriangleStateBackface,
};

class TriangleSelfContained : public Stateful<TriangleState>{
public:
	typedef std::vector<Vertex> VertexContainer;
	TriangleSelfContained(const Vertex& first, const Vertex& second, const Vertex& third, const Color& color, const Texture& text) 
		: texture(text), baseColor(color)
	{
		vertices.push_back(first);
		vertices.push_back(second);
		vertices.push_back(third);
	}

	TriangleSelfContained() : vertices(3) {
	}

	Vertex& operator[](size_t i) {
		return vertices[i];
	}

	const Vertex& operator[](size_t i) const {
		return vertices[i];
	}

	Vector3 Normal() const {
		return (Vector3(vertices[2]) - vertices[1]).Cross(Vector3(vertices[1]) - vertices[0]);
	}

	const Texture& GetTexture() const {
		return texture;
	}

	bool HasTexture() const {
		return texture;
	}

	float AverageZ() const;

	const Color& BaseColor() const {
		return baseColor;
	}

private:
	VertexContainer vertices;
	Texture texture;
	Color baseColor;
};

class Object;
class Triangle : public Stateful<TriangleState> {
	typedef std::vector<Vertex> VertexContainer;
public:
	typedef std::vector<size_t> VertexIndexContainer;

	Triangle(size_t first, size_t second, size_t third, const Texture& text, const Color& color = 0xffffff) 
		: vertexContainer(NULL), texture(text), baseColor(color) {
		vertices.push_back(first);
		vertices.push_back(second);
		vertices.push_back(third);
	}

	Triangle() : vertexContainer(NULL), vertices(3, 0xffffffff), texture(NULL) {
	}

	~Triangle() {
	}

	Vector3 Normal() const;

	operator TriangleSelfContained() const;

	Vertex& operator[](size_t i);

	const Vertex& operator[](size_t i) const;

	void Indices(size_t& first, size_t& second, size_t& third) const {
		first = vertices[0];
		second = vertices[1];
		third = vertices[2];
	}

	const Color& BaseColor() const {
		return baseColor;
	}

private:
	VertexIndexContainer vertices;
	VertexContainer* vertexContainer;
	Color baseColor;
	Vector3 averageNormal;
	Texture texture;
	friend class Object;
};

inline
Triangle::operator TriangleSelfContained() const {
	return TriangleSelfContained((*this)[0], (*this)[1], (*this)[2], baseColor, texture);
}

inline
Vector3 Triangle::Normal() const {
	return (Vector3((*this)[2]) - (*this)[1]).Cross(Vector3((*this)[1]) - (*this)[0]);
}

inline
Vertex& Triangle::operator[](size_t i) {
	return vertexContainer->at(vertices[i]);
}

inline
const Vertex& Triangle::operator[](size_t i) const {
	return vertexContainer->at(vertices[i]);
}
