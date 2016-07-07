#pragma once
#include <windows.h>
#include <vector>
#include "vertex.h"
#include "triangle.h"
#include "object.h"
#include "types.h"

struct Matrix;
struct Vector3;
struct Transformation;
class RenderList;
class ZBuffer;
class Texture;

class Graphics {
	RECT stage;
	class Camera* camera;
	class LinePainter* linePainter;
	class Object* street;

	ZBuffer* zBuffer;
	ObjectContainer container;

	int xmin, xmax, ymin, ymax;

	void Plot(HDC hdc, RenderList& renderList);
	void PlotTriangle(HDC hdc, const TriangleSelfContained& triangle);

	void CreatePlane(VertexContainer& vertices, IndexedTriangleContainer& container, const Texture& texture, int width, int height);

	struct ScreenPoint {
		float x, y;
		float z;
		float u, v;
		Color color;
		ScreenPoint(float x, float y, float z, float u, float v, const Color& color) {
			this->x = x, this->y = y;
			this->z = z;
			this->u = u, this->v = v;
			this->color = color;
		}

		ScreenPoint(const Vertex& other) {
			x = other.x, y = other.y;
			z = other.z;
			u = other.u, v = other.v;
			color = other.color;
		}
	};

	void SortPoints(ScreenPoint(&points)[3]);

	void PlotFlatTopTriangle(HDC hdc, ScreenPoint p1, ScreenPoint p2, ScreenPoint p3, const Texture& texture);
	void PlotFlatBottomTriangle(HDC hdc, ScreenPoint p1, ScreenPoint p2, ScreenPoint p3, const Texture& texture);

	void PlotPixel(HDC hdc, int x, int y, int z, const Color& color);

	void PlotGradientLine(HDC hdc, const Color& cl, const Color& cr, 
		float zl, float zr, float ul, float ur, float vl, float vr, 
		float xl, float xr, int y, const Texture& texture
		);
public:
	Graphics(const RECT& rect);
	~Graphics();
	void AddObject(Object* object);
	void Render(HDC hdc);
	void Update(HWND hwnd, float deltaTime);

	void SendKeyboardCommand(int command);
	const RECT& GetStage() const;
};

inline
const RECT& Graphics::GetStage() const {
	return stage;
}
