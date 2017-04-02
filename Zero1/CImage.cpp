#include "CImage.h"

bool CImage::CreateImage(CWindow & window)
{
	bitCount = 32;
	width = window.WindowWidth;
	height = window.WindowHeight;
	size = width * height * 4;

	BITMAPINFO bi;
	ZeroMemory(&bi, sizeof(BITMAPINFO));
	BITMAPINFOHEADER &header = bi.bmiHeader;
	header.biSize = sizeof(BITMAPINFOHEADER);
	header.biWidth = width;
	header.biHeight = -height;
	header.biPlanes = 1;
	header.biBitCount = bitCount;
	header.biCompression = BI_RGB;
	header.biSizeImage = size;     // 这个size应该是以字节为单位
	header.biXPelsPerMeter = 0;
	header.biYPelsPerMeter = 0;
	header.biClrUsed = 0;
	header.biClrImportant = 0;

	HDC hDC = GetDC(window.hWnd);
	screenDC = CreateCompatibleDC(hDC);
	ReleaseDC(window.hWnd, hDC);

	screenHB = CreateDIBSection(screenDC, &bi, DIB_RGB_COLORS, &memory, NULL, NULL);
	if (!screenHB)
	{
		//MessageBox(NULL, TEXT("创建设备无关位图时失败"), TEXT("错误"), NULL);
		return false;
	}

	oldHB = (HBITMAP)SelectObject(screenDC, screenHB);
	frameBuffer = (UINT*)memory;
	pitch = width * 4;

	memset(frameBuffer, 255, pitch*200);

	return true;
}

/*
inline void CImage::SetPixel(int x, int y, UINT color)
{
	if (x < 0 || x >= width || y < 0 || y >= height)
		return;
	frameBuffer[y*width + x] = color;
}
*/
void CImage::DrawLine(int x0, int y0, int x1, int y1, UINT color)
{
	int x,y, step, rem = 0;
	if (x0 == x1 && y0 == y1)
	{
		SetPixel(x0, y0, color);
	}
	else if (x0 == x1)
	{
		int step = y1 > y0 ? 1 : -1;
		for (y = y0; y != y1; y += step)
			SetPixel(x0, y, color);
	}
	else if (y0 == y1)
	{
		int step = x1 > x0 ? 1 : -1;
		for (x = x0; x != x1; x += step)
			SetPixel(x, y0, color);
	}
	else
	{
		int dx = (x0 < x1) ? x1 - x0 : x0 - x1;
		int dy = (y0 < y1) ? y1 - y0 : y0 - y1;
		if (dx >= dy)
		{
			if (x1 < x0)
			{
				swap(x0, x1);
				swap(y0, y1);
			}
			for (x = x0, y = y0; x <= x1; x++)
			{
				SetPixel(x, y, color);
				rem += dy;
				if (rem >= dx)
				{
					rem -= dx;
					y += (y1 >= y0) ? 1 : -1;
					SetPixel(x, y, color);
				}
			}
			SetPixel(x1, y1, color);
		}
		else
		{
			if (y1 < y0)
			{
				swap(x0, x1);
				swap(y0, y1);
			}
			for (x = x0, y = y0; y <= y1; ++y)
			{
				SetPixel(x, y, color);
				rem += dx;
				if (rem >= dy)
				{
					rem -= dy;
					x += (x1 >= x0) ? 1 : -1;
					SetPixel(x, y, color);
				}
			}
			SetPixel(x1, y1, color);
		}
	}
}

void CImage::DrawRectangle(int x0, int y0, int x1, int y1, UINT color)
{
	int x, y;
	x = min(x0, x1);
	y = min(y0, y1);
	x1 = max(x0, x1);
	y1 = max(y0, y1);

	/*for (; y <= y1; ++y)
		for (x0 = x; x0 <= x1; ++x0)
			SetPixel(x0, y, color);*/

	for (; y <= y1; ++y)
	{
		UINT *dst = frameBuffer + y * width + x;
		MemSetQuad(dst, color, x1 - x);
	}
}

void CImage::CopyToDC(HDC hDC, int x, int y, int w, int h)
{
	BitBlt(hDC, x, y, w, h, screenDC, 0, 0, SRCCOPY);
}
