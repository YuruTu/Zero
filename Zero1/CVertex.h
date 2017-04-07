#pragma once

#ifndef __CVERTEX_H__
#define __CVERTEX_H__

#include <windef.h>
#include "CVector.h"


class CVertex
{
public:
	CVector v;     // ��������
	UINT    color; // ��ɫ

	CVertex operator*(CMatrix& mat)
	{
		CVertex ret;
		ret.v = v * mat;
		ret.color = color;
		return ret;
	}
};

#endif // !__CVERTEX_H__
