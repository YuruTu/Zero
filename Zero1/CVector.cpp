
#include "CVector.h"

bool equals(const float &x, const float &y)
{
	return fabs(x - y) < EPIROUNDING_ERROR_f32;
}

void CVector::operator*=(CMatrix & oth)
{
	// copy self
	float X = x, Y = y, Z = z, W = w;
	// compute
	x = X*oth.m[0][0] + Y*oth.m[1][0] + Z*oth.m[2][0] + W*oth.m[3][0];
	y = X*oth.m[0][1] + Y*oth.m[1][1] + Z*oth.m[2][1] + W*oth.m[3][1];
	z = X*oth.m[0][2] + Y*oth.m[1][2] + Z*oth.m[2][2] + W*oth.m[3][2];
	w = X*oth.m[0][3] + Y*oth.m[1][3] + Z*oth.m[2][3] + W*oth.m[3][3];
}

CVector CVector::operator*(const CMatrix &oth) const
{
	return CVector(x*oth.m[0][0] + y*oth.m[1][0] + z*oth.m[2][0] + w*oth.m[3][0],
		x*oth.m[0][1] + y*oth.m[1][1] + z*oth.m[2][1] + w*oth.m[3][1],
		x*oth.m[0][2] + y*oth.m[1][2] + z*oth.m[2][2] + w*oth.m[3][2],
		x*oth.m[0][3] + y*oth.m[1][3] + z*oth.m[2][3] + w*oth.m[3][3]);
}