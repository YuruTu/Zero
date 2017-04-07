#pragma once

#ifndef __CVERTEX_H__
#define __CVERTEX_H__

#include <windef.h>
#include "CVector.h"


class CVertex
{
public:
	CVector v;     // ·ÂÉä×ø±ê
	UINT    color; // ÑÕÉ«

	CVertex operator*(CMatrix& mat)
	{
		CVertex ret;
		ret.v = v * mat;
		ret.color = color;
		return ret;
	}
};

#endif // !__CVERTEX_H__
