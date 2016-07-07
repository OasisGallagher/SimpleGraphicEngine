#include "transformation.h"
#include "linepainter.h"
#include "camera.h"
#include "graphics.h"
#include "gdef.h"
#include "vector3.h"
#include "matrix.h"
#include "comparer.h"
#include "renderlist.h"
#include "zbuffer.h"
#include "texture.h"
#include "memdc.h"

#pragma warning(disable: 4244)

Graphics::Graphics(const RECT& rect) : stage(rect) {
	linePainter = new LinePainter(rect);

	AssertEx(rect.bottom - rect.top == rect.right - rect.left, "only square stage is supported");

	camera = new Camera(moveSpeed, rotateSpeed, rect.bottom - rect.top);

	LightAttribute lightAttr;
	lightAttr.diffuse = Color(255, 255, 255);
	lightAttr.direction = Vector3::right;
	lightAttr.lightOn = true;
	lightAttr.lightType = LightTypeDirection;
	lightAttr.position.Set(200, 0, 0);
	camera->AddLight(Light(lightAttr));

	lightAttr.ambient = Color(90, 90, 90);
	lightAttr.lightType = LightTypeAmbient;
	lightAttr.lightOn = true;
	camera->AddLight(Light(lightAttr));

	zBuffer = new ZBuffer(stage.right - stage.left, stage.bottom - stage.top);

	VertexContainer vertices;
	IndexedTriangleContainer triangles;
	CreatePlane(vertices, triangles, Texture("bmp/metal.bmp"), 2, 3);
	street = new Object();
	//street->Initialize(vertices, triangles, 5.f, 12.f, Vector3(), Vector3());
	AddObject(street);

	xmin = stage.left + 1;
	xmax = stage.right - 2;
	ymin = stage.top + 1;
	ymax = stage.bottom - 2;
}

Graphics::~Graphics() {
	delete camera;
	delete linePainter;
	delete zBuffer;
	delete street;
}

void Graphics::CreatePlane(VertexContainer& vertices, 
	IndexedTriangleContainer& container,  const Texture& texture, int width, int height) {
	vertices.clear();
	container.clear();

	Vertex cell[] = {
		Vertex(-0.5f, 0, -0.5f, 0, 0.99f), Vertex(-0.5f, 0, 0.5f, 0, 0), Vertex(0.5f, 0, 0.5f, 0.99f, 0),
		Vertex(0.5f, 0, -0.5f, 0.99f, 0.99f),
	};

	for (int x = -width; x <= width; ++x) {
		for (int z = -height; z <= height; ++z) {
			Vertex lb = cell[0], lt = cell[1], rt = cell[2], rb = cell[3];
			lb.x += x, lb.z += z;
			lt.x += x, lt.z += z;
			rt.x += x, rt.z += z;
			rb.x += x, rb.z += z;
			vertices.push_back(lb);
			vertices.push_back(lt);
			vertices.push_back(rt);
			vertices.push_back(rb);
		}
	}

	for (size_t i = 0; i < vertices.size(); i += 4) {
		Triangle lt(i + 0, i + 1, i + 2, texture);
		Triangle rb(i + 0, i + 2, i + 3, texture);
		container.push_back(lt);
		container.push_back(rb);
	}
}

void Graphics::Plot(HDC hdc, RenderList& renderList) {
	for (RenderList::iterator ite = renderList.begin(); ite != renderList.end(); ++ite) {
		if (!ite->TestState(TriangleStateInactive) && !ite->TestState(TriangleStateBackface) && !ite->TestState(TriangleStateClipped))
			PlotTriangle(hdc, *ite);
	}
}

void Graphics::PlotPixel(HDC hdc, int x, int y, int z, const Color& color) {
	// 临时裁剪.
	AssertEx(x >= xmin && x <= xmax && y >= ymin && y <= ymax, "invalid coordinate");

	if ((*zBuffer)(x - stage.left, y - stage.top) < z) {
		SetPixel(hdc, x, y, color);
		(*zBuffer)(x - stage.left, y - stage.top) = float(z);
	}
}

#define UPDATE_VAR(M) \
	if(true) \
	{ \
		float multiplier = M; \
		xl += dxdyl * multiplier; \
		xr += dxdyr * multiplier; \
		ul += dudyl * multiplier; \
		ur += dudyr * multiplier; \
		vl += dvdyl * multiplier; \
		vr += dvdyr * multiplier; \
		cl += dcdyl * multiplier; \
		cr += dcdyr * multiplier; \
		zl += dzdyl * multiplier; \
		zr += dzdyr * multiplier; \
	} else (void)0

#define SHIFT_RIGHT_SCALE(M) \
	if (true) \
	{ \
		float multiplier = M; \
		tul += (ur - ul) * multiplier; \
		tvl += (vr - vl) * multiplier; \
		tcl += (cr - cl) * multiplier; \
		tzl += (zr - zl) * multiplier; \
	} else (void)0

void Graphics::PlotFlatTopTriangle(HDC hdc, ScreenPoint p1, ScreenPoint p2, ScreenPoint p3, const Texture& texture) {
	AssertEx(Approximately(p1.y, p2.y) && p1.y <= p3.y, "invalid flat top triangle");

	if (p1.x > p2.x) std::swap(p1, p2);

	float dy = float(p3.y - p1.y);

	float dxdyl = float(p3.x - p1.x) / dy;
	float dxdyr = float(p3.x - p2.x) / dy;
	ColorDiff dcdyl = (p3.color - p1.color) / dy;
	ColorDiff dcdyr = (p3.color - p2.color) / dy;
	float dzdyl = float(p3.z - p1.z) / dy;
	float dzdyr = float(p3.z - p2.z) / dy;

	float dudyl = float(p3.u / p3.z - p1.u / p1.z) / dy;
	float dudyr = float(p3.u / p3.z - p2.u / p2.z) / dy;
	float dvdyl = float(p3.v / p3.z - p1.v / p1.z) / dy;
	float dvdyr = float(p3.v / p3.z - p2.v / p2.z) / dy;

	float xl = (float)p1.x, xr = (float)p2.x;
	float zl = p1.z, zr = p2.z;
	Color cl = p1.color, cr = p2.color;

	float ul = (float)p1.u / p1.z;
	float vl = (float)p1.v / p1.z;
	float ur = (float)p2.u / p2.z;
	float vr = (float)p2.v / p2.z;

	int iy1 = 0, iy3 = 0;

	if (p1.y < ymin) {
		UPDATE_VAR(ymin - p1.y);
		p1.y = ymin;
		iy1 = p1.y;
	}
	else {
		iy1 = ceilf(p1.y);
		UPDATE_VAR(iy1 - p1.y);
	}

	if (p3.y > ymax) {
		p3.y = ymax;
		iy3 = p3.y - 1;
	}
	else {
		iy3 = ceilf(p3.y) - 1;
	}

	if (Between(p1.x, xmin, xmax) && Between(p2.x, xmin, xmax) && Between(p3.x, xmin, xmax)) {
		for (int y = iy1; y <= iy3; ++y) {
			PlotGradientLine(hdc, cl, cr, zl, zr, ul, ur, vl, vr, xl, xr, y, texture);
			UPDATE_VAR(1.f);
		}
	}
	else {
		for (int y = iy1; y <= iy3; ++y) {
			float left = xl, right = xr;
			float tzl = zl, tul = ul, tvl = vl;
			Color tcl = cl;
			if (right >= xmin && left <= xmax) {
				if (left < xmin) {
					SHIFT_RIGHT_SCALE((xmin - left) / (right - left));
					left = xmin;
				}

				right = Min(right, xmax);
				PlotGradientLine(hdc, tcl, cr, tzl, zr, tul, ur, tvl, vr, left, right, y, texture);
			}

			UPDATE_VAR(1.f);
		}
	}
}

void Graphics::PlotFlatBottomTriangle(HDC hdc, ScreenPoint p1, ScreenPoint p2, ScreenPoint p3, const Texture& texture) {
	AssertEx(Approximately(p1.y, p2.y) && p1.y >= p3.y, "invalid flat bottom triangle");

	if (p1.x > p2.x) std::swap(p1, p2);
	float dy = -float(p3.y - p1.y);

	float dxdyl = float(p1.x - p3.x) / dy;
	float dxdyr = float(p2.x - p3.x) / dy;

	ColorDiff dcdyl = (p1.color - p3.color) / dy;
	ColorDiff dcdyr = (p2.color - p3.color) / dy;

	float dzdyl = float(1.f / p1.z - 1.f / p3.z) / dy;
	float dzdyr = float(1.f / p2.z - 1.f / p3.z) / dy;

	float dudyl = float(p1.u / p1.z - p3.u / p3.z) / dy;
	float dudyr = float(p2.u / p2.z - p3.u / p3.z) / dy;
	float dvdyl = float(p1.v / p1.z - p3.v / p3.z) / dy;
	float dvdyr = float(p2.v / p2.z - p3.v / p3.z) / dy;

	float xl = (float)p3.x, xr = (float)p3.x;
	float zl = 1.f / p3.z, zr = 1.f / p3.z;

	Color cl = p3.color;
	Color cr = p3.color;

	float ul = (float)p3.u / p3.z;
	float vl = (float)p3.v / p3.z;
	float ur = (float)p3.u / p3.z;
	float vr = (float)p3.v / p3.z;

	int iy1 = 0, iy3 = 0;

	if (p3.y < ymin) {
		UPDATE_VAR(ymin - p3.y);

		p3.y = ymin;
		iy3 = p3.y;
	}
	else {
		iy3 = (int)ceilf(p3.y);
		UPDATE_VAR(iy3 - p3.y);
	}

	if (p1.y > ymax) {
		p1.y = ymax;
		iy1 = p1.y - 1;
	}
	else {
		iy1 = ceilf(p1.y) - 1;
	}

	if (Between(p1.x, xmin, xmax) && Between(p2.x, xmin, xmax) && Between(p3.x, xmin, xmax)) {
		for (int y = iy3; y <= iy1; ++y) {
			PlotGradientLine(hdc, cl, cr, zl, zr, ul, ur, vl, vr, xl, xr, y, texture);
			UPDATE_VAR(1.f);
		}
	}
	else {
		for (int y = iy3; y <= iy1; ++y) {
			float left = xl, right = xr;
			Color tcl = cl;
			float tzl = zl, tul = ul, tvl = vl;
			if(right >= xmin && left <= xmax) {
				if (left < xmin) {
					SHIFT_RIGHT_SCALE((xmin - left) / (right - left));
					left = xmin;
				}
				right = Min(right, xmax);
				PlotGradientLine(hdc, tcl, cr, tzl, zr, tul, ur, tvl, vr, left, right, y, texture);
			}

			UPDATE_VAR(1.f);
		}
	}
}

void Graphics::PlotGradientLine(HDC hdc, const Color& cl,
	const Color& cr, float zl, float zr, float ul, float ur, 
	float vl, float vr, float xl, float xr, int y, const Texture& texture) {
	ColorDiff dc = (cr - cl) / (xr - xl);
	float dz = (zr - zl) / (xr - xl);
	float du = (ur - ul) / (xr - xl);
	float dv = (vr - vl) / (xr - xl);

	Color ci = cl;
	float ui = ul, vi = vl;
	float zi = zl;
	for (int xi = (int)floorf(xl); xi < (int)ceilf(xr); ++xi) {
		Color textel = ci;
		if (ui >= 0 && vi >= 0)
			textel *= texture.Rgb(ui / zi, vi / zi);

		PlotPixel(hdc, xi, y, Round2Integer(zl), textel);

		ci += dc;
		zi += dz;

		ui += du;
		vi += dv;
	}
}

void Graphics::AddObject(Object* object) {
	container.push_back(object);
}

void Graphics::Render(HDC hdc) {
	zBuffer->Reset();

	MemDC memDC(hdc, &stage);

	const char* info = "按F1获取帮助";
	TextOut(memDC, 0, 0, info, strlen(info));

	RenderList renderList;
	camera->RunPipeline(renderList, container);

	Plot(memDC, renderList);
}

void Graphics::Update(HWND hwnd, float deltaTime) {
	ObjectContainer::iterator first = container.begin();
	for (ObjectContainer::iterator ite = container.begin(); ite != container.end(); ++ite) {
		if ((*ite)->Update(deltaTime)) {
			if(first != ite)
				*first = *ite;
			++first;
		}
	}

	container.erase(first, container.end());

	HDC hdc = GetDC(hwnd);
	Render(hdc);
	ReleaseDC(hwnd, hdc);
}

void Graphics::SendKeyboardCommand(int command) {
	camera->OnKeyboardCommand(command);
}

void Graphics::SortPoints(ScreenPoint(&points)[3]) {
	if (points[1].y < points[0].y) {
		std::swap(points[1], points[0]);
	}

	if (points[2].y < points[0].y) {
		std::swap(points[2], points[0]);
	}

	if (points[2].y < points[1].y) {
		std::swap(points[2], points[1]);
	}
}

void Graphics::PlotTriangle(HDC hdc, const TriangleSelfContained& triangle) {
	ScreenPoint points[3] = { triangle[0], triangle[1], triangle[2] };

	if ((Approximately(points[0].x, points[1].x) && Approximately(points[1].x, points[2].x))
		|| (Approximately(points[0].y, points[1].y) && Approximately(points[1].y, points[2].y)))
		return;

	SortPoints(points);

	if (Approximately(points[0].y, points[1].y)) {
		PlotFlatTopTriangle(hdc, points[0], points[1], points[2], triangle.GetTexture());
	}
	else if (Approximately(points[1].y, points[2].y)) {
		PlotFlatBottomTriangle(hdc, points[1], points[2], points[0], triangle.GetTexture());
	}
	else {
		float tx = points[0].x + (points[1].y - points[0].y) * (points[2].x - points[0].x) / float(points[2].y - points[0].y);
		Color color = (points[0].color + (points[2].color - points[0].color) * ((points[1].y - points[0].y) / float(points[2].y - points[0].y)));
		float tz = points[0].z + (points[1].y - points[0].y) * (points[2].z - points[0].z) / float(points[2].y - points[0].y);
		float tu = points[0].u + (points[1].y - points[0].y) * (points[2].u - points[0].u) / float(points[2].y - points[0].y);
		float tv = points[0].v + (points[1].y - points[0].y) * (points[2].v - points[0].v) / float(points[2].y - points[0].y);

		ScreenPoint tp(tx, points[1].y, tz, tu, tv, color);

		PlotFlatBottomTriangle(hdc, tp, points[1], points[0], triangle.GetTexture());
		PlotFlatTopTriangle(hdc, points[1], tp, points[2], triangle.GetTexture());
	}
}
