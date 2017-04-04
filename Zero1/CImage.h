#pragma once

#ifndef __CIMAGE_H__
#define __CIMAGE_H__

#include <Windows.h>
#include "CWindow.h"

template <typename T>
void swap(T &a, T&b)
{
	T tmp(a);
	a = b;
	b = tmp;
}

class CWindow;

class CImage
{
public:
	bool CreateImage(CWindow& window);
	inline void CImage::SetPixel(int x, int y, UINT color)
	{
		if (x < 0 || x >= width || y < 0 || y >= height)
			return;
		frameBuffer[y*width + x] = color;
	}
	void DrawLine(int x0, int y0, int x1, int y2, UINT color);
	void DrawRectangle(int x0, int y0, int x1, int y1, UINT color);

	inline void CopyToDC(HDC hDC, int x, int y, int w, int h)
	{
		BitBlt(hDC, x, y, w, h, screenDC, 0, 0, SRCCOPY);
	}

	int bitCount;
	int width, height, pitch,size;
	HDC screenDC;
	HBITMAP screenHB,oldHB;
	void *memory;

	UINT *frameBuffer;
};

#endif