#pragma once
#include <vector>
#include "stateful.h"
#include "transformation.h"
#include "types.h"

enum ObjectState {
	ObjectStateInactive,
	ObjectStateCulled,
	ObjectStateBackface,
};

class Object : public Stateful<ObjectState> {
	typedef Stateful<ObjectState> BaseType;
	
public:
	Object();

	void Initialize(const VertexContainer& vertices,
					const IndexedTriangleContainer& container,
					float radius, float scale,
					const Vector3& position,
					const Vector3& eulerAngles);

	virtual bool Update(float deltaTime) {
		return true;
	}

	void WorldTransformation(Transformation& transformation) const;
	Vector3 WorldPosition() const;

	float MaxRadius() const;

	typedef IndexedTriangleContainer::iterator iterator;
	iterator begin() {
		return triangles.begin();
	}

	iterator end() {
		return triangles.end();
	}

	void RefreshNormal();

	virtual void ResetState();

	Vertex& operator[](size_t pos) {
		return transformedVertices[pos];
	}

	const Vertex& operator[](size_t pos) const {
		return transformedVertices[pos];
	}

	size_t VertexCount() const {
		return transformedVertices.size();
	}

protected:
	Vector3 worldPosition;
	Vector3 eulerAngles;

private:
	Object& operator = (const Object&);
	Object(const Object&);

	VertexContainer vertices;
	VertexContainer transformedVertices;

	IndexedTriangleContainer triangles;

	float maxRadius;

	float scale;
};

inline
float Object::MaxRadius() const {
	return maxRadius;
}

inline
void Object::WorldTransformation(Transformation& transformation) const {
	transformation *= Transformation(eulerAngles.x, eulerAngles.y, eulerAngles.z);
	transformation *= worldPosition;
}

inline
Vector3 Object::WorldPosition() const {
	return worldPosition;
}
