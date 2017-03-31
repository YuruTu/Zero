#pragma once

#ifndef __CIMAGE_H__
#define __CIMAGE_H__

#include <Windows.h>
#include "CWindow.h"

class CWindow;

class CImage
{
public:
	bool CreateImage(CWindow& window);

	int bitCount;
	int width, height, pitch,size;
	HDC screenDC;
	HBITMAP screenHB,oldHB;
	void *memory;

	unsigned char *frameBuffer;
};

#endif