#pragma once
#include <windows.h>
#include <string>
#include "color.h"
#include "utility.h"
#include "gdef.h"

class TextureImpl : RefCountable{
	HBITMAP bitmap;
	BITMAP bmp;
	LPBYTE ptr;
	std::string bmpName;

	friend class Texture;
	TextureImpl(const char* fileName) {
		bmpName = fileName;
		ZeroMemory(&bmp, sizeof(bmp));
		bitmap = (HBITMAP)LoadImage(NULL, fileName, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);
		AssertEx(bitmap != NULL, "load bitmap failed");
		GetObject(bitmap, sizeof(bmp), &bmp);
		LPBYTE pixel = (LPBYTE)bmp.bmBits;
		ptr = pixel + (bmp.bmHeight - 1) * bmp.bmWidthBytes;
	}

	Color Rgb(float x, float y) const {
 		AssertEx(x >= 0 && x <= 1.f && y >= 0 && y <= 1.f, "invalid coordinate");
		LPBYTE offset = (ptr - int(y * bmp.bmHeight) * bmp.bmWidthBytes) + int(x * bmp.bmWidth) * bmp.bmBitsPixel / 8;

		DWORD rgb = 0;
		CopyMemory(&rgb, offset, bmp.bmBitsPixel / 8);
		return Color(rgb);
	}

	~TextureImpl() {
		DeleteObject(bitmap);
	}

	const std::string& BmpName() const {
		return bmpName;
	}
};

class Texture {
	TextureImpl* texture;
public:
	Texture() : texture(NULL) {
	}

	Texture(const char* bmpPath) : texture(NULL) {
		if (bmpPath != NULL)
			texture = new TextureImpl(bmpPath);
	}

	~Texture() {
		if (texture != NULL && texture->dec() == 0)
			delete texture;
	}

	Texture(const Texture& other) {
		texture = other.texture;
		if (texture != NULL)
			texture->inc();
	}

	Texture& operator = (const Texture& other) {
		if (other.texture != NULL)
			other.texture->inc();

		if (texture != NULL && texture->dec() == 0)
			delete texture;

		texture = other.texture;

		return *this;
	}

	operator bool() const {
		return texture != NULL;
	}

	Color Rgb(float x, float y) const {
		x = Min(0.999f, x), x = Max(0, x);
		y = Min(0.999f, y), y = Max(0, y);
		return texture->Rgb(x, y);
	}

	const std::string& BmpName() const {
		return texture->BmpName();
	}
};
