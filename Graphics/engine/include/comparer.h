#pragma once
struct Comparer {
	virtual bool compare(float other) const = 0;
	Comparer(float value) : f(value) {
	}

protected:
	float f;
};

struct Less : Comparer {
	Less(float value) :Comparer(value) {

	}

	virtual bool compare(float other) const {
		return f < other;
	}
};

struct Greater : Comparer {
	Greater(float value) :Comparer(value) {

	}

	virtual bool compare(float other) const {
		return f > other;
	}
};
