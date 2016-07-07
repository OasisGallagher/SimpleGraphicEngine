#pragma once
#include <cmath>
#include <cassert>
#include <bitset>

#ifndef Min
#define Min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef Max
#define Max(a,b) (((a) > (b)) ? (a) : (b))
#endif

#define Between(x, left, right)		((x) <= (right) && (x) >= (left))

static inline bool Approximately(float f, float x = 0.f) {
	return fabs(f - x) < 0.000001f;
}

static inline float Round(float f) {
	return floorf(f + 0.5f);
}

static inline int Round2Integer(float f) {
	return (int)floorf(f + 0.5f);
}

template <class FwdIt, class Ty>
static inline bool All(FwdIt first, FwdIt last, Ty value) {
	return std::find_if(first, last, std::bind2nd(std::not_equal_to<Ty>(), value)) == last;
}

static inline void Assert(const wchar_t* msg, const wchar_t* file, unsigned line) {
	//__asm int 3;
	_wassert(msg, file, line);
}

#define AssertEx(_Expression, _Message)	\
	(void)( (!!(_Expression)) || (Assert(_CRT_WIDE(#_Expression) _CRT_WIDE(": ") _CRT_WIDE(_Message), _CRT_WIDE(__FILE__), __LINE__), 0) )


class RefCountable {
	unsigned refCount;
protected:
	RefCountable() : refCount(1) {
	}
	unsigned inc() {
		return ++refCount;
	}

	unsigned dec() {
		AssertEx(refCount > 0, "can't decrease reference count");
		return --refCount;
	}
};

template <class StateType, size_t StateCount = 8>
class Stateful {
	typedef std::bitset<StateCount> Container;
	Container cont;
public:
	void SetState(StateType state, bool f = true) {
		cont.set((size_t)state, f);
	}

	bool TestState(StateType state) const {
		return cont.test((size_t)state);
	}

	virtual void ResetState() {
		cont.reset();
	}

protected:
	Stateful() {
	}
};
