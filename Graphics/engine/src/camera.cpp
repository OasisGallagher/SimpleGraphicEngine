#include <cstring>
#include "camera.h"
#include "transformation.h"
#include "object.h"
#include "renderlist.h"
#include <functional>
#include "gdef.h"

#define ShadeModeGouraud

Camera::Camera(float ms, float rs, int viewportWidth) {
	attribute.farPlane = 1000;
	attribute.initPosition.Set(0, 12.f, -50);
	attribute.lookAt.Set(0, 12.f, 0);
	attribute.moveSpeed = ms;
	attribute.rotateSpeed = rs;
	attribute.nearPlane = 1;
	attribute.viewportWidth = viewportWidth;

	Reset();
}

Camera::~Camera() { 
}

void Camera::Pitch(float radian) {
	Transformation q(right, radian);
	up *= q;
	look *= q;
}

void Camera::Yaw(float radian) {
	Transformation q(up, radian);
	right *= q;
	look *= q;
}

void Camera::Roll(float radian) { 
	Transformation q(look, radian);
	up *= q;
	right *= q;
}

void Camera::Strafe(float units) {
	cameraPosition += Vector3(right.x * units, 0, right.z * units);
}

void Camera::Fly(float units) { 
	cameraPosition += Vector3(up.x * units, up.y * units, up.z * units);
}

void Camera::Walk(float units) {
	cameraPosition += Vector3(look.x * units, 0, look.z * units);
}

Transformation Camera::CameraTransformation() {
	look.Normalize();

	up = look.Cross(right);
	up.Normalize();

	right = up.Cross(look);
	right.Normalize();

	return Transformation(Matrix(right.x, up.x, look.x, 0,
		right.y, up.y, look.y, 0,
		right.z, up.z, look.z, 0,
		-cameraPosition.Dot(right), -cameraPosition.Dot(up), -cameraPosition.Dot(look), 1));
}

void Camera::RunPipeline(RenderList& renderList, ObjectContainer& container) {
	for (ObjectContainer::iterator ite = container.begin(); ite != container.end(); ++ite) {
		Object& object = **ite;
		object.ResetState();

		typedef Object& (Camera::*PilelineOnObject)(Object&);
		PilelineOnObject objPipeline[] = {
			&Camera::CullObject,
			&Camera::Model2World,
			&Camera::RefreshNormal,
		};

		size_t count = sizeof(objPipeline) / sizeof(PilelineOnObject);
		unsigned i = 0;
		for (; i < count; ++i) {
			(this->*objPipeline[i])(object);
			if (object.TestState(ObjectStateInactive)
				|| object.TestState(ObjectStateBackface)
				|| object.TestState(ObjectStateCulled)) {
				break;
			}
		}

		if (i >= count)
			renderList += object;
	}

	typedef RenderList& (Camera::*PilelineOnRenderList)(RenderList&);
	PilelineOnRenderList rendListPipeline[] {
		&Camera::BackfaceCulling,
		&Camera::World2Camera,
		&Camera::ClipRenderList,
		&Camera::LightRenderList,
		&Camera::SortRenderList,
		&Camera::Camera2Perspective,
		&Camera::Perspective2Screen
	};

	for (unsigned i = 0; i < sizeof(rendListPipeline) / sizeof(PilelineOnRenderList); ++i) {
		(this->*rendListPipeline[i])(renderList);
	}
}

void Camera::Reset() {
	cameraPosition = attribute.initPosition;

	up = Vector3::up;
	look = attribute.lookAt - cameraPosition;

	look.Normalize();
	right = up.Cross(look);
	right.Normalize();
}

float Camera::Distance(bool ctrlPressed, float speed) {
	return -2 * int(ctrlPressed) * speed + speed;
}

RenderList& Camera::Camera2Perspective(RenderList& list) {
	float Q = attribute.farPlane / (attribute.farPlane - attribute.nearPlane);
	Q = 1;// , farPlane = 0;
	return TransformRenderList(list,
		Matrix(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, Q, 1,
		0, 0, -Q * 0/*attribute.farPlane*/, 0
		));
}

Object& Camera::Model2World(Object& object) {
	Transformation transformation;
	object.WorldTransformation(transformation);
	return TransformObject(object, transformation);
}

Object& Camera::RefreshNormal(Object& object) {
	object.RefreshNormal();
	return object;
}

RenderList& Camera::World2Camera(RenderList& list) {
	TransformRenderList(list, CameraTransformation());
	return list;
}

RenderList& Camera::SortRenderList(RenderList& list) {
	list.SortOnZ();
	return list;
}

RenderList& Camera::ClipRenderList(RenderList& list) {
	const float factor = 1.f;

	RenderList::iterator listEnd = list.end();
	for (RenderList::iterator ite = list.begin(); ite != listEnd; ++ite) {
		TriangleSelfContained& triangle = *ite;
		if(triangle.TestState(TriangleStateInactive) 
			|| triangle.TestState(TriangleStateClipped)
			|| triangle.TestState(TriangleStateBackface))
			continue;

		// 
		int vertexClipCode[ClipCodeCount] = { 0, 1, 2 };
		for (size_t i = 0; i < 3; ++i) {
			float zt = triangle[i].z;
			if (triangle[i].x > zt)
				vertexClipCode[i] = ClipCodeGreater;
			else if (triangle[i].x < -zt)
				vertexClipCode[i] = ClipCodeLess;
			else if (triangle[i].y > zt)
				vertexClipCode[i] = ClipCodeGreater;
			else if (triangle[i].y < -zt)
				vertexClipCode[i] = ClipCodeLess;
			else
				vertexClipCode[i] = ClipCodeIn;
		}

		if (All(vertexClipCode, vertexClipCode + _countof(vertexClipCode), ClipCodeGreater)
			|| All(vertexClipCode, vertexClipCode + _countof(vertexClipCode), ClipCodeLess)) {
			triangle.SetState(TriangleStateClipped, true);
			continue;
		}

		// 
		int countVertexIn = 0;
		for (int i = 0; i < 3; ++i) {
			if (triangle[i].z > attribute.farPlane)
				vertexClipCode[i] = ClipCodeGreater;
			else if (triangle[i].z < attribute.nearPlane || Approximately(triangle[i].z, attribute.nearPlane))
				vertexClipCode[i] = ClipCodeLess;
			else {
				vertexClipCode[i] = ClipCodeIn;
				++countVertexIn;
			}
		}

		if (All(vertexClipCode, vertexClipCode + _countof(vertexClipCode), ClipCodeGreater)
			|| All(vertexClipCode, vertexClipCode + _countof(vertexClipCode), ClipCodeLess)) {
			triangle.SetState(TriangleStateClipped, true);
			continue;
		}

		if ((vertexClipCode[0] | vertexClipCode[1] | vertexClipCode[2]) & ClipCodeLess) {
			if (countVertexIn == 1)
				ClipNearPanel(vertexClipCode, triangle);
			else {
				AssertEx(countVertexIn == 2, "logic error");
				TriangleSelfContained newTriangle;
				if (ClipNearPanel2(newTriangle, vertexClipCode, triangle))
					list += newTriangle;
			}
		}
	}

	return list;
}

void Camera::ClipNearPanel(const int(&verxClipCode)[3], TriangleSelfContained& triangle) {
	int v0 = 0, v1 = 1, v2 = 2;
	if (verxClipCode[1] == ClipCodeIn) {
		v0 = 1, v1 = 2, v2 = 0;
	}
	else if (verxClipCode[2] == ClipCodeIn) {
		v0 = 2, v1 = 0, v2 = 1;
	}

	Vector3 v = Vector3(triangle[v1]) - triangle[v0];
	float t = (attribute.nearPlane - triangle[v0].z) / v.z;

	float x = triangle[v0].x + v.x * t;
	float y = triangle[v0].y + v.y * t;

	triangle[v1] = Vector3(x, y, attribute.nearPlane);

	v = Vector3(triangle[v2]) - triangle[v0];
	t = (attribute.nearPlane - triangle[v0].z) / v.z;

	x = triangle[v0].x + v.x * t;
	y = triangle[v0].y + v.y * t;
	triangle[v2] = Vector3(x, y, attribute.nearPlane);

	if (triangle.HasTexture()) {
		float ui = triangle[v0].u + (triangle[v1].u - triangle[v0].u) * t;
		float vi = triangle[v0].v + (triangle[v1].v - triangle[v0].v) * t;

		triangle[v1].u = ui, triangle[v1].v = vi;

		ui = triangle[v0].u + (triangle[v2].u - triangle[v0].u) * t;
		vi = triangle[v0].v + (triangle[v2].v - triangle[v0].v) * t;
		triangle[v2].u = ui, triangle[v2].v = vi;

		Vector3 normal = triangle.Normal().Normalized();
		triangle[v0].normal = normal;
		triangle[v1].normal = normal;
		triangle[v2].normal = normal;

		Color ci = triangle[v0].color + (triangle[v1].color - triangle[v0].color) * t;
		triangle[v1].color = ci;
		ci = triangle[v0].color + (triangle[v2].color - triangle[v0].color) * t;
		triangle[v1].color = ci;
	}
}

bool Camera::ClipNearPanel2(TriangleSelfContained& newTriangle, const int(&verxClipCode)[3], TriangleSelfContained& triangle) {
	int v0 = 0, v1 = 1, v2 = 2;
	newTriangle = triangle;

	if (verxClipCode[1] == ClipCodeLess)
		v0 = 1, v1 = 2, v2 = 0;
	else if (verxClipCode[2] == ClipCodeLess)
		v0 = 2, v1 = 0, v2 = 1;

	Vector3 v = Vector3(triangle[v1]) - triangle[v0];
	float t1 = (attribute.nearPlane - triangle[v0].z) / v.z;

	float x01i = triangle[v0].x + v.x * t1;
	float y01i = triangle[v0].y + v.y * t1;

	v = Vector3(triangle[v2]) - triangle[v0];
	float t2 = (attribute.nearPlane - triangle[v0].z) / v.z;

	float x02i = triangle[v0].x + v.x * t2;
	float y02i = triangle[v0].y + v.y * t2;

	bool newTriagleGenerated = !Approximately(x01i, x02i) || !Approximately(y01i, y02i);

	triangle[v0] = Vector3(x01i, y01i, attribute.nearPlane);

	newTriangle[v1] = Vector3(x01i, y01i, attribute.nearPlane);
	newTriangle[v0] = Vector3(x02i, y02i, attribute.nearPlane);

	if (triangle.HasTexture()) {
		float u01i = triangle[v0].u + (triangle[v1].u - triangle[v0].u) * t1;
		float v01i = triangle[v0].v + (triangle[v1].v - triangle[v0].v) * t1;

		float u02i = triangle[v0].u + (triangle[v2].u - triangle[v0].u) * t2;
		float v02i = triangle[v0].v + (triangle[v2].v - triangle[v0].v) * t2;

		triangle[v0].u = u01i, triangle[v0].v = v01i;
		newTriangle[v0].u = u02i, newTriangle[v0].v = v02i;
		newTriangle[v1].u = u01i, newTriangle[v1].v = v01i;

		Color c01i = triangle[v0].color + (triangle[v1].color - triangle[v0].color) * t1;
		Color c02i = triangle[v0].color + (triangle[v2].color - triangle[v0].color) * t2;
		newTriangle[v0].color = c01i, newTriangle[v1].color = c02i;
	}

	Vector3 normal = triangle.Normal().Normalized();
	triangle[v0].normal = normal, triangle[v1].normal = normal, triangle[v2].normal = normal;

	normal = newTriangle.Normal();
	if (!Approximately(normal.LengthSquared())) {
		normal.Normalize();
		newTriangle[v0].normal = normal, newTriangle[v1].normal = normal, newTriangle[v2].normal = normal;
		return true;
	}

	return false;
}

RenderList& Camera::LightRenderList(RenderList& renderList) {
	for (RenderList::iterator ite = renderList.begin(); ite != renderList.end(); ++ite) {
		TriangleSelfContained& triangle = *ite;
		if (triangle.TestState(TriangleStateInactive) || triangle.TestState(TriangleStateBackface)
			|| triangle.TestState(TriangleStateClipped))
			continue;

#ifdef ShadeModeGouraud
		unsigned rSum0 = 0, rSum1 = 0, rSum2 = 0;
		unsigned gSum0 = 0, gSum1 = 0, gSum2 = 0;
		unsigned bSum0 = 0, bSum1 = 0, bSum2 = 0;
#else
		unsigned rSum = 0, gSum = 0, bSum = 0;
#endif
		for (LightContainer::iterator ite = lightContainer.begin(); ite != lightContainer.end(); ++ite) {
			Light& light = *ite;
			if (!light.On())
				continue;

			const Color& baseColor = triangle.BaseColor();

#ifdef ShadeModeGouraud
			if (light.Type() == LightTypeAmbient) {
				unsigned ri = light.Ambient().r * baseColor.r / 256;
				unsigned gi = light.Ambient().g * baseColor.g / 256;
				unsigned bi = light.Ambient().b * baseColor.b / 256;

				rSum0 += ri, gSum0 += gi, bSum0 += bi;
				rSum1 += ri, gSum1 += gi, bSum1 += bi;
				rSum2 += ri, gSum2 += gi, bSum2 += bi;
			}
			else if (light.Type() == LightTypeDirection) {
				float dot = triangle[0].normal.Dot(light.Direction());
				if (dot > 0) {
					rSum0 += Round2Integer(light.Diffuse().r * baseColor.r * dot) / 256;
					gSum0 += Round2Integer(light.Diffuse().g * baseColor.g * dot) / 256;
					bSum0 += Round2Integer(light.Diffuse().b * baseColor.b * dot) / 256;
				}

				dot = triangle[1].normal.Dot(light.Direction());
				if (dot > 0) {
					rSum1 += Round2Integer(light.Diffuse().r * baseColor.r * dot) / 256;
					gSum1 += Round2Integer(light.Diffuse().g * baseColor.g * dot) / 256;
					bSum1 += Round2Integer(light.Diffuse().b * baseColor.b * dot) / 256;
				}

				dot = triangle[2].normal.Dot(light.Direction());
				if (dot > 0) {
					rSum2 += Round2Integer(light.Diffuse().r * baseColor.r * dot) / 256;
					gSum2 += Round2Integer(light.Diffuse().g * baseColor.g * dot) / 256;
					bSum2 += Round2Integer(light.Diffuse().b * baseColor.b * dot) / 256;
				}
			}
#else
			if (light.Type() == LightTypeAmbient) {
				rSum += light.Ambient().r * baseColor.r / 256;
				gSum += light.Ambient().g * baseColor.g / 256;
				bSum += light.Ambient().b * baseColor.b / 256;
			}
			else if (light.Type() == LightTypeDirection) {
				Vector3 normal = triangle.Normal();
				float nl = normal.Length(), dot = normal.Dot(light.Direction());
				if (dot > 0) {
					float f = 128.f * dot / nl;
					rSum += Round2Integer(light.Diffuse().r * baseColor.r * f / (256 * 128));
					gSum += Round2Integer(light.Diffuse().g * baseColor.g * f / (256 * 128));
					bSum += Round2Integer(light.Diffuse().b * baseColor.b * f / (256 * 128));
				}
			}
			else {
				AssertEx(false, "invalid light type");
				continue;
			}
#endif
		}
#ifdef ShadeModeGouraud
		triangle[0].color = Color(Min(255, rSum0), Min(255, gSum0), Min(255, bSum0));
		triangle[1].color = Color(Min(255, rSum1), Min(255, gSum1), Min(255, bSum1));
		triangle[2].color = Color(Min(255, rSum2), Min(255, gSum2), Min(255, bSum2));
#else
		for (size_t i = 0; i < 3; ++i) {
			triangle[i].color = Color(Min(255, rSum), Min(255, gSum), Min(255, bSum));
		}
#endif
	}

	return renderList;
}

Matrix& Camera::GetTransformMatrix(Matrix& result, const Vector3& pos, const Vector3& lookAt) {
	Vector3 zaxis = (lookAt - pos).Normalized();
	Vector3 xaxis = Vector3::up.Cross(zaxis).Normalized();
	Vector3 yaxis = zaxis.Cross(xaxis).Normalized();

	result(0, Vector3(xaxis.x, yaxis.x, zaxis.x));
	result(1, Vector3(xaxis.y, yaxis.y, zaxis.y));
	result(2, Vector3(xaxis.z, yaxis.z, zaxis.z));
	result(3, Vector3(-xaxis.Dot(pos), -yaxis.Dot(pos), -zaxis.Dot(pos)));
	result(3, 3) = 1;

	return result;
}

Object& Camera::TransformObject(Object& object, const Transformation& transformation) {
	for (size_t i = 0; i < object.VertexCount(); ++i) {
		Vertex& vertex = object[i];
		vertex = vertex * transformation;
	}

	return object;
}

RenderList& Camera::TransformRenderList(RenderList& list, const Transformation& transformation) {
	for (RenderList::iterator ite = list.begin(); ite != list.end(); ++ite) {
		TriangleSelfContained& triangle = *ite;
		if (triangle.TestState(TriangleStateInactive)
			|| triangle.TestState(TriangleStateBackface)
			|| triangle.TestState(TriangleStateClipped))
			continue;

		for (size_t i = 0; i < 3; ++i) {
			Vertex newVertex = triangle[i];
			newVertex = transformation * triangle[i];
			triangle[i] = newVertex;
		}
	}

	return list;
}

Object& Camera::CullObject(Object& object) {
	Vector3 spherePosition = object.WorldPosition() * CameraTransformation();
	if (spherePosition.z - object.MaxRadius() > attribute.farPlane 
		|| spherePosition.z + object.MaxRadius() < attribute.nearPlane
		|| spherePosition.x - object.MaxRadius() > spherePosition.z
		|| spherePosition.x + object.MaxRadius() < -spherePosition.z
		|| spherePosition.y - object.MaxRadius() > spherePosition.z
		|| spherePosition.y + object.MaxRadius() < -spherePosition.z) {
		object.SetState(ObjectStateCulled, true);
	}

	return object;
}

RenderList& Camera::BackfaceCulling(RenderList& list) {
	for (RenderList::iterator ite = list.begin(); ite != list.end(); ++ite) {
		TriangleSelfContained& triangle = *ite;

		if (triangle.TestState(TriangleStateInactive)
			|| triangle.TestState(TriangleStateBackface)
			|| triangle.TestState(TriangleStateClipped))
			continue;

		Vector3 look = Vector3(triangle[1]) - this->cameraPosition;
		
		if (triangle.Normal().Dot(look) <= 0)
			triangle.SetState(TriangleStateBackface, true);
	}

	return list;
}

RenderList& Camera::Perspective2Screen(RenderList& list) {
	for (RenderList::iterator ite = list.begin(); ite != list.end(); ++ite) {
		TriangleSelfContained& triangle = *ite;
		if (triangle.TestState(TriangleStateInactive)
			|| triangle.TestState(TriangleStateClipped) 
			|| triangle.TestState(TriangleStateBackface))
			continue;

		for (size_t i = 0; i < 3; ++i) {
			Vertex newVertex = triangle[i];
			int half = attribute.viewportWidth / 2;
			newVertex.x = triangle[i].x * half + half;
			newVertex.y = -triangle[i].y * half + half;
			triangle[i] = newVertex;
		}
	}

	return list;
}

void Camera::OnKeyboardCommand(int command) {
	switch (command) {
	case 'x':
		Pitch(attribute.rotateSpeed);
		break;
	case 'X':
		Pitch(-attribute.rotateSpeed);
		break;
	case 'y':
		Yaw(attribute.rotateSpeed);
		break;
	case 'Y':
		Yaw(-attribute.rotateSpeed);
		break;
	case 'z':
		Roll(attribute.rotateSpeed);
		break;
	case 'Z':
		Roll(-attribute.rotateSpeed);
		break;
	case 'l':
		Strafe(attribute.moveSpeed);
		break;
	case 'L':
		Strafe(-attribute.moveSpeed);
		break;
	case 'u':
		Fly(attribute.moveSpeed);
		break;
	case 'U':
		Fly(-attribute.moveSpeed);
		break;
	case 'm':
		Walk(attribute.moveSpeed);
		break;
	case 'M':
		Walk(-attribute.moveSpeed);
		break;
	case 'i':
	case 'I':
		Reset();
		break;
	}
}
