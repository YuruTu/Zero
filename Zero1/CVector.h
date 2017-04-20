#pragma once

#ifndef __CVECTOR_H__
#define __CVECTOR_H__

#include <math.h>
#include "CMatrix.h"

const float EPIROUNDING_ERROR_f32 = 0.000001f;

bool equals(const float &x, const float &y);
template<typename T>
T interp(T x0, T x1, float t)
{
	return x0 + (x1 - x0)*t;
}

class CMatrix;

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
		return (float)sqrt(x*x + y*y + z*z);
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
	void set(float x, float y, float z, float w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}
	void interp(const CVector &v0, const CVector &v1, float t)
	{
		x = ::interp(v0.x, v1.x, t);
		y = ::interp(v0.y, v1.y, t);
		z = ::interp(v0.z, v1.z, t);
	}

	void operator=(const CVector &oth)
	{
		x = oth.x, y = oth.y, z = oth.z, w = oth.w;
	}
	void operator*=(float T)
	{
		x *= T;
		y *= T;
		z *= T;
	}
	void operator*=(CMatrix &oth);
	void operator+=(const CVector &oth)
	{
		x += oth.x;
		y += oth.y;
		z += oth.z;
		w += oth.w;
	}
	void operator+=(const float &oth)
	{
		x += oth, y += oth, z += oth, w += oth;
	}

	CVector operator-()
	{
		return CVector(-x, -y, -z);
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

	CVector operator*(const CMatrix &oth) const;
	CVector operator*(float T) const
	{
		return CVector(x*T, y*T, z*T);
	}

	float x, y, z, w;

};


#endif // !__CVECTOR_H__
