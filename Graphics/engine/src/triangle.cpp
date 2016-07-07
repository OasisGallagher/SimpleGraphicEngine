#include "triangle.h"
#include "object.h"

float TriangleSelfContained::AverageZ() const {
	float sum = 0.f;
	for (VertexContainer::const_iterator ite = vertices.begin(); ite != vertices.end(); ++ite)
		sum += ite->z;
	return sum / 3;
}
