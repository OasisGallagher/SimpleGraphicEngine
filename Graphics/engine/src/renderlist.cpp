#include <numeric>
#include "renderlist.h"
#include "object.h"

RenderList& RenderList::operator += (Object& object) {
	for (Object::iterator ite = object.begin(); ite != object.end(); ++ite) {
		const Triangle& current = *ite;
		if (!current.TestState(TriangleStateInactive)
			&& !current.TestState(TriangleStateBackface)
			&& !current.TestState(TriangleStateClipped)) {
			triangles.push_back(current);
		}
	}

	if (!triangles.empty()) {
		size_t oldSize = zOrder.size();
		zOrder.resize(triangles.size(), oldSize);
		for (size_t i = oldSize; i < zOrder.size(); ++i) {
			zOrder[i] = i;
		}
	}

	return *this;
}

