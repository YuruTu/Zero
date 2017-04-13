#pragma once

#ifndef __CTRIANGLE_H__

#include "CImage.h"
#include "CVector.h"

struct SEdge
{
	CVertex v[2], p;
};

class CTriangle
{
public:
	SEdge left, right;
	float top, bottom;
	float leftk, rightk;
	void interp(float y)
	{
		float h1, h2, t1, t2;
		h1 = left.v[1].v.y - left.v[0].v.y;
		h2 = right.v[1].v.y - right.v[0].v.y;
		t1 = (y - left.v[0].v.y) / h1;
		t2 = (y - right.v[0].v.y) / h2;
		left.p.interp(left.v[0], left.v[1], t1);
		right.p.interp(right.v[0], right.v[1], t2);
	}
};

inline int CMID(int in, int min, int max)
{
	if (in > max) return max;
	if (in < min) return min;
	return in;
}

struct SScanline
{
	int x, y, width;
	CVertex v,step;
	void scan(CTriangle &tr, int y)
	{
		float width = tr.right.p.v.x - tr.left.p.v.x;
		x = (tr.left.p.v.x + 0.5);
		this->y = y;
		this->width = int(tr.right.p.v.x + 0.5) - x;
		v = tr.left.p;
		step = (tr.right.p - tr.left.p) / width;
	}

	void draw(CTriangle &tr, CImage &image)
	{
		UINT *frameBuffer = &image.frameBuffer[y*image.width];
		for (; width > 0; x++, width--)
		{
			if (x >= 0 && x < image.width)
			{
				if (image.window->renderState & RD_STATE_COLOR) {
					float r = v.color.r ;
					float g = v.color.g ;
					float b = v.color.b ;
					int R = (int)(r * 255.0f);
					int G = (int)(g * 255.0f);
					int B = (int)(b * 255.0f);
					//printf("R:%d G:%d B:%d\n", R, G, B);
					R = CMID(R, 0, 255);
					G = CMID(G, 0, 255);
					B = CMID(B, 0, 255);
					frameBuffer[x] = (R << 16) | (G << 8) | (B);
				}
			}
			v += step;
			if (x >= image.width) break;
		}
	}
};



// out 至少要有2个 CTriangle 的空间
// 这里的坐标系是屏幕坐标， X正半轴指向右方， Y正半轴指向正下方, 坐标原点在窗口左上方
int GetTriangleList(CVertex &v0,CVertex &v1,CVertex &v2, CTriangle *tr)
{
	if (v0.v.y > v1.v.y) swap(v0, v1);
	if (v0.v.y > v2.v.y) swap(v0, v2);
	if (v1.v.y > v2.v.y) swap(v1, v2);

	if (v0.v.y == v1.v.y && v1.v.y == v2.v.y) return 0;
	if (v0.v.x == v1.v.x && v1.v.x == v2.v.x) return 0;

	if (int(v0.v.y) == int(v1.v.y))
	{
		// 使v0在底边的左边
		if (v0.v.x > v1.v.x) swap(v0, v1);
		tr[0].left.v[0] = v0; 
		tr[0].left.v[1] = v2;
		tr[0].right.v[0] = v1; 
		tr[0].right.v[1] = v2;
		tr[0].top = v0.v.y;
		tr[0].bottom = v2.v.y;
		return 1;
	}
	if (int(v1.v.y) == int(v2.v.y))
	{
		// 使v1在底边的左边
		if (v1.v.x > v2.v.x) swap(v1, v2);
		tr[0].left.v[0] = v0;
		tr[0].left.v[1] = v1;
		tr[0].right.v[0] = v0;
		tr[0].right.v[1] = v2;
		tr[0].top = v0.v.y;
		tr[0].bottom = v1.v.y;
		return 1;
	}

	// 要拆分为两个梯形
	// 上
	tr[0].top = v0.v.y;
	tr[0].bottom = v1.v.y;
	
	// 下
	tr[1].top = v1.v.y;
	tr[1].bottom = v2.v.y;

	// 判断v1 在 长边的哪一侧
	float k = (v2.v.x - v0.v.x)/(v2.v.y - v0.v.y);
	float testX = v0.v.x + k*(v1.v.y - v0.v.y);
	if (testX <= v1.v.x)
	{
		// v1 在长边右侧， 右三角形
		tr[0].left.v[0] = v0;
		tr[0].left.v[1] = v2;
		tr[0].right.v[0] = v0;
		tr[0].right.v[1] = v1;
		tr[1].left.v[0] = v0;
		tr[1].left.v[1] = v2;
		tr[1].right.v[0] = v1;
		tr[1].right.v[1] = v2;
	}
	else
	{
		// v1 在长边左侧， 左三角形
		tr[0].left.v[0] = v0;
		tr[0].left.v[1] = v1;
		tr[0].right.v[0] = v0;
		tr[0].right.v[1] = v2;
		tr[1].left.v[0] = v1;
		tr[1].left.v[1] = v2;
		tr[1].right.v[0] = v0;
		tr[1].right.v[1] = v2;
	}
	return 2;
}

void RenderTriangle(CImage *pImage, CTriangle &tr)
{
	// 从top -> bottom 方向，逐行扫描, y+ 轴方向
	int y;
	int top, bottom;
	top = static_cast<int>(tr.top + 0.5);
	bottom = static_cast<int>(tr.bottom + 0.5);
	SScanline scanline;
	for (y = top; y < bottom; ++y)
	{
		if (y >= 0 && y < pImage->height)
		{
			tr.interp(float(y) + 0.5);
			scanline.scan(tr,y);
			scanline.draw(tr, *pImage);
		}
		if (y >= pImage->height) break;
	}
}


#endif // !__CTRIANGLE_H__
