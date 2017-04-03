
#include "CVector.h"

bool equals(const float &x, const float &y)
{
	return fabs(x - y) < EPIROUNDING_ERROR_f32;
}

CVector CVector::operator*(const CMatrix &oth) const
{
	return CVector(x*oth.m[0][0] + y*oth.m[1][0] + z*oth.m[2][0] + w*oth.m[3][0],
		x*oth.m[0][1] + y*oth.m[1][1] + z*oth.m[2][1] + w*oth.m[3][1],
		x*oth.m[0][2] + y*oth.m[1][2] + z*oth.m[2][2] + w*oth.m[3][2],
		x*oth.m[0][3] + y*oth.m[1][3] + z*oth.m[2][3] + w*oth.m[3][3]);
}