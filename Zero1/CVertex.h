#pragma once

#ifndef __CVERTEX_H__
#define __CVERTEX_H__

#include <windef.h>
#include "CVector.h"

struct CColor
{
	float r, g, b;
};

class CVertex
{
public:
	CVector v;     // ·ÂÉä×ø±ê
	CColor color;

	CVertex operator*(CMatrix& mat)
	{
		CVertex ret;
		ret.v = v * mat;
		ret.color.r = color.r;
		ret.color.g = color.g;
		ret.color.b = color.b;
		return ret;
	}

	CVertex operator-(CVertex &oth)
	{
		CVertex ret;
		ret.v = this->v - oth.v;
		ret.color.r = this->color.r - oth.color.r;
		ret.color.g = this->color.g - oth.color.g;
		ret.color.b = this->color.b - oth.color.b;
		return ret;
	}

	CVertex operator/(float n)
	{
		CVertex ret;
		n = 1 / n;
		ret.v = v* n;
		ret.color.r = color.r * n;
		ret.color.g = color.g * n;
		ret.color.b = color.b * n;
		return ret;
	}

	void operator+=(CVertex &oth)
	{
		v.x += oth.v.x;
		v.y += oth.v.y;
		v.z += oth.v.z;
		v.w += oth.v.w;
		color.r += oth.color.r;
		color.g += oth.color.g;
		color.b += oth.color.b;
	}

	void interp(const CVertex &v0, const CVertex &v1, float t)
	{
		v.interp(v0.v, v1.v, t);
		color.r = ::interp(v0.color.r, v1.color.r, t);
		color.g = ::interp(v0.color.g, v1.color.g, t);
		color.b = ::interp(v0.color.b, v1.color.b, t);
	}
};

#endif // !__CVERTEX_H__
