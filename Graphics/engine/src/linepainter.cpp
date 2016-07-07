#include "linepainter.h"
#include <cmath>
#include "utility.h"

void LinePainter::DDA(HDC hdc, const Vertex& from, const Vertex& to, const Color& color) {
	float dx = to.x - from.x, dy = to.y - from.y;
	int steps = (int)Max(fabs(dx), fabs(dy));

	for (int k = 0; k <= steps; ++k) {
		float x = k * dx / steps + from.x;
		float y = k * dy / steps + from.y;

		// ...
		// 临时的裁剪方法.
		if (x < boundingRect.left || x > boundingRect.right
			|| y < boundingRect.top || y > boundingRect.bottom)
			continue;

		SetPixel(hdc, (int)Round(x), (int)Round(y), color);
	}
}

void LinePainter::LineTo(HDC hdc, const Vertex& to, const Color& color) {
	
	position = to;

	// 需要裁剪线段, 临时在画线时处理.
	DrawLine(hdc, position, to, color);
	/*
	char buffer[16] = { 0 };
	sprintf_s(buffer, sizeof(buffer), "(%d, %d)", (int)dest.x, (int)dest.y);
	//TextOut(hdc, dest.x, dest.y, buffer, strlen(buffer));
	*/
}

void LinePainter::DrawGradients(HDC hdc, int x1, int x2, int y, Color begin, ColorDiff increment) {
	x1 += (boundingRect.right - boundingRect.left) / 2;
	x2 += (boundingRect.right - boundingRect.left) / 2;
	y = (boundingRect.bottom - boundingRect.top) / 2 - y;

	//x1 += boundingRect.left, y += boundingRect.top;
	//x2 += boundingRect.left;

	for (; x1 < x2; ++x1) {
		SetPixel(hdc, x1, y, begin);
		begin += increment;
	}
	/*
	int steps = (int)max(fabs(dx), 0);

	for (int k = 0; k <= steps; ++k) {
		float x = k * dx / steps + x1;

		// ...
		// 临时的裁剪方法.
		
		if (x < boundingRect.left || x > boundingRect.right
			|| y < boundingRect.top || y > boundingRect.bottom)
			continue;
		SetPixel(hdc, (int)Round(x), (int)Round(y), color);
	}
*/
}

