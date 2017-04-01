#pragma once

#ifndef __CVECTOR_H__
#define __CVECTOR_H__

#include <math.h>
#include "CMatrix.h"

const float EPIROUNDING_ERROR_f32 = 0.000001f;

bool equals(const float &x, const float &y)
{
	return fabs(x - y) < EPIROUNDING_ERROR_f32;
}

class CVector
{
public:
	CVector() : x(0), y(0), z(0), w(0) {}
	CVector(float nx, float ny, float nz, float nw = 0) : x(nx), y(ny), z(nz), w(nw) {}
	explicit CVector(float n) : x(n), y(n), z(n), w(0) {}
	CVector(const CVector &oth) : x(oth.x), y(oth.y), z(oth.z), w(oth.w) {}

public:
	virtual void normalize()
	{
		float len = length();
		if (len != 0.0f)
		{
			len = 1 / len;
			x *= len;
			y *= len;
			z *= len;
			w *= len;
		}
	}
	float length() const
	{
		return sqrt(x*x + y*y + z*z);
	}
	float lengthSQ() const
	{
		return x*x + y*y + z*z;
	}
	float dotProduct(const CVector &oth) const
	{
		return x*oth.x + y*oth.y + z*oth.z;
	}
	CVector crossProduct(const CVector &oth) const
	{
		return CVector(y * oth.z - z * oth.y, z * oth.x - x * oth.z, x * oth.y - y * oth.x);
	}
	CVector projON(const CVector &n) const
	{
		return n * (dotProduct(n) / n.lengthSQ());
	}

	CVector operator-()
	{
		return CVector(-x, -y, -z);
	}
	void operator=(const CVector &oth)
	{
		x = oth.x, y = oth.y, z = oth.z, w = oth.w;
	}
	CVector operator+(const CVector &oth) const
	{
		return CVector(x + oth.x, y + oth.y, z + oth.z, w + oth.w);
	}
	CVector operator-(const CVector &oth) const
	{
		return CVector(x - oth.x, y - oth.y, z - oth.z, w - oth.w);
	}
	CVector operator+(const float T) const
	{
		return CVector(x + T, y + T, z + T);
	}
	bool operator==(const CVector &oth)
	{
		return equals(x, oth.x) && equals(y, oth.y) && equals(z, oth.z);
	}

	CVector operator*(const CMatrix &oth) const
	{
		return CVector(x*oth.m[0][0] + y*oth.m[1][0] + z*oth.m[2][0] + w*oth.m[3][0],
					   x*oth.m[0][1] + y*oth.m[1][1] + z*oth.m[2][1] + w*oth.m[3][1],
					   x*oth.m[0][2] + y*oth.m[1][2] + z*oth.m[2][2] + w*oth.m[3][2],
					   x*oth.m[0][3] + y*oth.m[1][3] + z*oth.m[2][3] + w*oth.m[3][3]);
	}
	CVector operator*(float T) const
	{
		return CVector(x*T, y*T, z*T);
	}

	float x, y, z, w;

};


#endif // !__CVECTOR_H__
