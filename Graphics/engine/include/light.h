#pragma once
#include "color.h"
#include "vector3.h"
#include "transformation.h"

enum LightType {
	LightTypeAmbient,
	LightTypeDirection,
	LightTypeCount,
};

struct LightAttribute {
	bool lightOn;
	LightType lightType;
	Color ambient, diffuse, specular;

	Vector3 position;
	Vector3 direction;
};

class Object;

class Light {
	LightAttribute attribute;
public:
	Light(const LightAttribute& attr) : attribute(attr) {
	}

	bool On() const {
		return attribute.lightOn;
	}

	LightType Type() const {
		return attribute.lightType;
	}

	const Vector3& Direction() {
		AssertEx(attribute.lightType == LightTypeDirection, "");
		return attribute.direction;
	}

	const Color& Ambient() const {
		return attribute.ambient;
	}

	const Color& Diffuse() const {
		return attribute.diffuse;
	}

	const Color& Specular() const {
		return attribute.specular;
	}
};
