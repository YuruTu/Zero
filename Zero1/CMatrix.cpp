
#include "CMatrix.h"

void CMatrix::set_rotation(float x, float y, float z, float theta)
{
	CVector vec(x, y, z);
	vec.normalize();

	float fsin, fcos;
	fsin = (float)sin(theta);
	fcos = (float)cos(theta);

	m[0][0] = fcos + (1 - fcos) * vec.x * vec.x;
	m[0][1] = (1 - fcos) * vec.x * vec.y + fsin * vec.z;
	m[0][2] = (1 - fcos) * vec.x * vec.z - fsin * vec.y;
	m[0][3] = 0;
	m[1][0] = (1 - fcos) * vec.x * vec.y - fsin * vec.z;
	m[1][1] = fcos + (1 - fcos) * vec.y * vec.y;
	m[1][2] = (1 - fcos) * vec.y * vec.z + fsin * vec.x;
	m[1][3] = 0;
	m[2][0] = (1 - fcos) * vec.x * vec.z + fsin * vec.y;
	m[2][1] = (1 - fcos) * vec.y * vec.z - fsin * vec.x;
	m[2][2] = fcos + (1 - fcos) * vec.z * vec.z;
	m[2][3] = 0;
	m[3][0] = m[3][1] = m[3][2] = 0;
	m[3][3] = 1;
}

void CMatrix::set_lookat(const CVector &eye, const CVector &at, const CVector &up)
{
	CVector xaxis, yaxis, zaxis;

	zaxis = at - eye;
	zaxis.normalize();

	xaxis = up.crossProduct(zaxis);
	xaxis.normalize();

	yaxis = zaxis.crossProduct(xaxis);
	yaxis.normalize();

	m[0][0] = xaxis.x;
	m[1][0] = xaxis.y;
	m[2][0] = xaxis.z;
	m[3][0] = -xaxis.dotProduct(eye);

	m[0][1] = yaxis.x;
	m[1][1] = yaxis.y;
	m[2][1] = yaxis.z;
	m[3][1] = -yaxis.dotProduct(eye);

	m[0][2] = zaxis.x;
	m[1][2] = zaxis.y;
	m[2][2] = zaxis.z;
	m[3][2] = -zaxis.dotProduct(eye);

	m[0][3] = m[1][3] = m[2][3] = 0;
	m[3][3] = 1;
}