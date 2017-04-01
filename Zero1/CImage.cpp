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

inline void CImage::SetPixel(int x, int y, UINT color)
{
	if (x < 0 || x >= width || y < 0 || y >= height)
		return;
	frameBuffer[y*width + x] = color;
}
