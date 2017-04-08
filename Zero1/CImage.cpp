#include "CImage.h"

bool CImage::CreateImage(CWindow & window)
{
	this->window = &window;
	bitCount = 32;
	width = window.windowWidth;
	height = window.windowHeight;
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

void CImage::DrawLineEx(int x0, int y0, int x1, int y1, UINT color)
{
	int dx = x1 - x0;
	int dy = y1 - y0;

	int c;
	int m;
	int d = 0;
	int run;

	int xInc = 4;
	int yInc = pitch;

	if (dx < 0)
	{
		xInc = -xInc;
		dx = -dx;
	}

	if (dy < 0)
	{
		yInc = -yInc;
		dy = -dy;
	}

	UINT *dst;
	dst = (UINT*)((UCHAR*)memory + (y0 * pitch) + (x0 << 2));

	if (dy > dx)
	{
		swap(dx, dy);
		swap(xInc, yInc);
	}

	c = dx << 1;
	m = dy << 1;

	run = dx;
	do
	{
		*dst = color;

		dst = (UINT*)((UCHAR*)dst + xInc);	// x += xInc
		d += m;
		if (d > dx)
		{
			dst = (UINT*)((UCHAR*)dst + yInc);	// y += yInc
			d -= c;
		}
		run -= 1;
	} while (run >= 0);
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

CVector p0,p1, p2;
void CImage::DrawPrimitive(CVertex &v0, CVertex &v1, CVertex &v2)
{
	// No.1   Vertex stage
	// 1. local  -> global
	// 2. global -> camera
	// 3. camera -> CVV(NOD?) 
	// 4. cliping             剪裁
	// 5. backface culling    背面消除


	CTransform &trans = window->transform;
	// 1.2.
	p0 = v0.v * trans.getWorld();
	p1 = v1.v * trans.getWorld();
	p2 = v2.v * trans.getWorld();
	p0 *= trans.getView();
	p1 *= trans.getView();
	p2 *= trans.getView();

	// 5.
	if (window->backfaceCull)
	{
		CVector look = p0; // 三角形上的任意一个点，都可以作为视线点
		CVector t1 = p1 - p0, t2 = p2 - p0;
		CVector n = t1.crossProduct(t2);
		n.normalize();
		float lookn = look.dotProduct(n);

		if (lookn >= 0.0f)
			return;
	}

	p0 *= trans.getProj();
	p1 *= trans.getProj();
	p2 *= trans.getProj();

	//4.
	// 有一个点不在CVV内，就不画了。233
	if (trans.check_cvv(p0) != 0) return;
	if (trans.check_cvv(p1) != 0) return;
	if (trans.check_cvv(p2) != 0) return;

	// 归一化，但并不移动到屏幕坐标
	trans.homogenize(p0);
	trans.homogenize(p1);
	trans.homogenize(p2);

	// 转换为屏幕坐标
	/*CMatrix &screen = window->screen;
	p0 *= screen;
	p1 *= screen;
	p2 *= screen;*/

	float r = 1.0f;//1.0*window->WindowWidth / window->WindowHeight;
	int w = window->windowWidth;
	int h = window->windowHeight;
	p0.x = w*(p0.x + 1) / 2 + 0;
	p0.y = h*(1 - p0.y) / 2;
	p1.x = w*(p1.x + r) / (2 * r) + 0;
	p1.y = h*(1 - p1.y) / 2;
	p2.x = w*(p2.x + r) / (2 * r) + 0;
	p2.y = h*(1 - p2.y) / 2;

	// No.2   Pixel stage
	

	if (tmpLine == 1)
	{
		// wireframe
		DrawLine(int(p0.x), int(p0.y), int(p1.x), int(p1.y), 0);
		DrawLine(int(p0.x), int(p0.y), int(p2.x), int(p2.y), 0);
		DrawLine(int(p1.x), int(p1.y), int(p2.x), int(p2.y), 0);
	}
	else if (tmpLine == 2)
	{
		DrawLineEx(int(p0.x), int(p0.y), int(p1.x), int(p1.y), 0);
		DrawLineEx(int(p0.x), int(p0.y), int(p2.x), int(p2.y), 0);
		DrawLineEx(int(p1.x), int(p1.y), int(p2.x), int(p2.y), 0);
	}

}

/*
void CImage::CopyToDC(HDC hDC, int x, int y, int w, int h)
{
	BitBlt(hDC, x, y, w, h, screenDC, 0, 0, SRCCOPY);
}
*/
