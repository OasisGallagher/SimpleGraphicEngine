#pragma once
#include <windows.h>
#include "vertex.h"
#include "color.h"

class LinePainter {
	Vertex position;
	RECT boundingRect;

	void DrawLine(HDC hdc, const Vertex& from, const Vertex& to, const Color& color);

	void DDA(HDC hdc, const Vertex& from, const Vertex& to, const Color& color);

public:
	LinePainter(const RECT& rect);
	void MoveTo(const Vertex& pos);
	void LineTo(HDC hdc, const Vertex& to, const Color& color);
	void DrawHorizontal(HDC hdc, float x1, float x2, float y, const Color& color);

	void DrawGradients(HDC hdc, int x1, int x2, int y, Color begin, ColorDiff increment);
};

inline
LinePainter::LinePainter(const RECT& rect) : boundingRect(rect) {
}

inline
void LinePainter::MoveTo(const Vertex& pos) {
	position = pos;
}

inline
void LinePainter::DrawLine(HDC hdc, const Vertex& from, const Vertex& to, const Color& color) {
	Vertex start = from, dest = to;
// 	start.x += (boundingRect.right - boundingRect.left) / 2;
// 	dest.x += (boundingRect.right - boundingRect.left) / 2;
// 	start.y = (boundingRect.bottom - boundingRect.top) / 2 - start.y;
// 	dest.y = (boundingRect.bottom - boundingRect.top) / 2 - dest.y;
// 
// 	start.x += boundingRect.left, start.y += boundingRect.top;
// 	dest.x += boundingRect.left, dest.y += boundingRect.top;

	DDA(hdc, start, dest, color);
}

inline
void LinePainter::DrawHorizontal(HDC hdc, float x1, float x2, float y, const Color& color) {
	DrawLine(hdc, Vertex(x1, y), Vertex(x2, y), color);
}
