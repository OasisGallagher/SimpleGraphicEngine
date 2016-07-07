#pragma once
#include <vector>
#include "vector3.h"
#include "vertex.h"
#include "triangle.h"

class Object;

typedef std::vector<Object*> ObjectContainer;
typedef std::vector<Vertex> VertexContainer;
typedef std::vector<size_t> VertexIndexContainer;

typedef std::vector<Triangle> IndexedTriangleContainer;
typedef std::vector<TriangleSelfContained> SelfContainedTriangleContainer;
