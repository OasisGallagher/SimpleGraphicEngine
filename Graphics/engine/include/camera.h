#pragma once
#include <windows.h>
#include <vector>
#include "vector3.h"
#include "light.h"
#include "types.h"

struct Transformation;
class RenderList;
class Object;

// UVN相机模型.
class Camera {
	struct CameraAttribute {
		Vector3 initPosition;
		Vector3 lookAt;
		float nearPlane, farPlane;
		int viewportWidth;
		float rotateSpeed;
		float moveSpeed;
	};

	// 重置.
	void Reset();

	// 绕x轴.
	void Pitch(float radian);
	// 绕y轴.
	void Yaw(float radian);
	// 绕z轴.
	void Roll(float radian);

	// 左右.
	void Strafe(float units);

	// 上下.
	void Fly(float units);

	// 前后.
	void Walk(float units);

	float Distance(bool ctrlPressed, float speed);

	// 获得相机在pos处, 且朝向lookAt位置时的变换矩阵.
	Matrix& GetTransformMatrix(Matrix& result, const Vector3& pos, const Vector3& lookAt);

	Object& TransformObject(Object& object, const Transformation& transformation);

	RenderList& TransformRenderList(RenderList& list, const Transformation& transformation);

	Object& Model2World(Object& object);

	Object& RefreshNormal(Object& object);

	RenderList& World2Camera(RenderList& list);

	RenderList& SortRenderList(RenderList& list);

	RenderList& Camera2Perspective(RenderList& list);

	RenderList& Perspective2Screen(RenderList& list);

	RenderList& BackfaceCulling(RenderList& list);

	Object& CullObject(Object& object);

	Transformation CameraTransformation();

	RenderList& LightRenderList(RenderList& list);

	RenderList& ClipRenderList(RenderList& list);

	void ClipNearPanel(const int(&verxClipCode)[3], TriangleSelfContained& triangle);

	bool ClipNearPanel2(TriangleSelfContained& newTriangle, const int(&verxClipCode)[3], TriangleSelfContained& triangle);

	typedef std::vector<Light> LightContainer;
	LightContainer lightContainer;

	CameraAttribute attribute;
	Vector3 cameraPosition;
	Vector3 right, up, look;
public:

	Camera(float ms, float rs, int viewportWidth);
	~Camera();

	void RunPipeline(RenderList& renderList, ObjectContainer& container);

	void OnKeyboardCommand(int command);

	void AddLight(const Light& light);
};

inline
void Camera::AddLight(const Light& light) {
	lightContainer.push_back(light);
}
