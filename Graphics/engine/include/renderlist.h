#pragma once
#include <vector>
#include <algorithm>
#include "stateful.h"
#include "types.h"
#include "utility.h"

class Object;
class RenderList {
	SelfContainedTriangleContainer triangles;
	typedef std::vector<size_t> OrderContainer;
	OrderContainer zOrder;
	
	class RenderListIterator {
		RenderList& renderList;
		size_t index;
	public:
		RenderListIterator(RenderList& list, size_t from);
		bool operator == (const RenderListIterator& iter) const;
		bool operator != (const RenderListIterator& iter) const;
		TriangleSelfContained& operator *();
		TriangleSelfContained* operator ->();
		RenderListIterator& operator ++();
	};

	struct Sorter {
		const SelfContainedTriangleContainer& cont;
		Sorter(const SelfContainedTriangleContainer& container) : cont(container) {
		}

		bool operator ()(size_t lhs, size_t rhs) const {
			return cont[lhs].AverageZ() > cont[rhs].AverageZ();
		}
	};

public:
	RenderList(Object& object);
	RenderList() {
	}

	RenderList& operator += (Object& object);
	RenderList& operator += (const TriangleSelfContained& triangle);

	typedef RenderListIterator iterator;

	iterator begin();
	iterator end();

	void SortOnZ();
};

inline
RenderList::RenderList(Object& object) {
	*this += object;
}

inline
RenderList::RenderListIterator RenderList::begin() {
	return RenderListIterator(*this, 0);
}

inline
RenderList::RenderListIterator RenderList::end() {
	return RenderListIterator(*this, zOrder.size());
}

inline
RenderList& RenderList::operator +=(const TriangleSelfContained& triangle) {
	triangles.push_back(triangle);
	zOrder.push_back(zOrder.size());
	return *this;
}

inline
void RenderList::SortOnZ() {
	AssertEx(zOrder.size() == triangles.size(), "mismatch between order container and triangle container");
	std::sort(zOrder.begin(), zOrder.end(), Sorter(triangles));
}

inline
RenderList::RenderListIterator::RenderListIterator(RenderList& list, size_t from)
: renderList(list), index(from) {
}

inline
bool RenderList::RenderListIterator::operator == (const RenderListIterator& other) const {
	AssertEx(&renderList == &other.renderList, "incompatible iterators");
	return index == other.index;
}

inline
bool RenderList::RenderListIterator::operator != (const RenderListIterator& other) const {
	AssertEx(&renderList == &other.renderList, "incompatible iterators");
	return index != other.index;
}

inline
TriangleSelfContained& RenderList::RenderListIterator::operator *() {
	return renderList.triangles[renderList.zOrder[index]];
}

inline
TriangleSelfContained* RenderList::RenderListIterator::operator ->() {
	return &renderList.triangles[renderList.zOrder[index]];
}

inline
RenderList::RenderListIterator& RenderList::RenderListIterator::operator ++() {
	++index;
	return *this;
}
