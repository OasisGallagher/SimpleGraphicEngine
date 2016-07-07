#pragma once
#include <windows.h>

class MemDC {  
public:  
	MemDC(HDC dc, LPRECT rect) {
		originalDC = dc;
		memoryDC = CreateCompatibleDC(dc);

		bitmap = CreateCompatibleBitmap(dc, rect->right - rect->left, rect->bottom - rect->top);

		oldObject = SelectObject(memoryDC, bitmap);

		RECT r = { 0, 0, rect->right - rect->left, rect->bottom - rect->top };
		FillRect(memoryDC, &r, (HBRUSH)GetStockObject(WHITE_BRUSH));

		dcRect = *rect;
	}
	
	~MemDC() {
		Render();
		SelectObject(memoryDC, oldObject);

		DeleteDC(memoryDC);
		DeleteObject(bitmap);
	}
	
	operator HDC() {
		return memoryDC;
	}

private:
	void Render() {
		BitBlt(originalDC, dcRect.left, dcRect.top, dcRect.right - dcRect.left,
			dcRect.bottom - dcRect.top, memoryDC, 0, 0, SRCCOPY);
	}

	RECT dcRect;
	HDC originalDC; 
	HDC memoryDC;
	HGDIOBJ oldObject;

	HBITMAP bitmap;
};
