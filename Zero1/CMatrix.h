#pragma once

#ifndef __CMATRIX_H__
#define __CMATRIX_H__

#include "CVector.h"

class CVector;

class CMatrix
{
public:
	CMatrix(bool SetZero = true) 
	{
		if (SetZero)
		{
			m[0][0] = 0; m[0][1] = 0; m[0][2] = 0; m[0][3] = 0;
			m[1][0] = 0; m[1][1] = 0; m[1][2] = 0; m[1][3] = 0;
			m[2][0] = 0; m[2][1] = 0; m[2][2] = 0; m[2][3] = 0;
			m[3][0] = 0; m[3][1] = 0; m[3][2] = 0; m[3][3] = 0;
		}
	}
	CMatrix(float f00, float f01, float f02, float f03,
			float f10, float f11, float f12, float f13,
			float f20, float f21, float f22, float f23,
			float f30, float f31, float f32, float f33)
	{
		m[0][0] = f00; m[0][1] = f01; m[0][2] = f02; m[0][3] = f03;
		m[1][0] = f10; m[1][1] = f11; m[1][2] = f12; m[1][3] = f13;
		m[2][0] = f20; m[2][1] = f21; m[2][2] = f22; m[2][3] = f23;
		m[3][0] = f30; m[3][1] = f31; m[3][2] = f32; m[3][3] = f33;
	}
	
	CMatrix(float T)
	{
		m[0][0] = T; m[0][1] = T; m[0][2] = T; m[0][3] = T;
		m[1][0] = T; m[1][1] = T; m[1][2] = T; m[1][3] = T;
		m[2][0] = T; m[2][1] = T; m[2][2] = T; m[2][3] = T;
		m[3][0] = T; m[3][1] = T; m[3][2] = T; m[3][3] = T;
	}

	CMatrix(const CMatrix &oth)
	{
		m[0][0] = oth.m[0][0]; m[0][1] = oth.m[0][1]; m[0][2] = oth.m[0][2]; m[0][3] = oth.m[0][3];
		m[1][0] = oth.m[1][0]; m[1][1] = oth.m[1][1]; m[1][2] = oth.m[1][2]; m[1][3] = oth.m[1][3];
		m[2][0] = oth.m[2][0]; m[2][1] = oth.m[2][1]; m[2][2] = oth.m[2][2]; m[2][3] = oth.m[2][3];
		m[3][0] = oth.m[3][0]; m[3][1] = oth.m[3][1]; m[3][2] = oth.m[3][2]; m[3][3] = oth.m[3][3];
	}

	void set_identity()
	{
		m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.0f;
		m[0][1] = m[0][2] = m[0][3] = 0.0f;
		m[1][0] = m[1][2] = m[1][3] = 0.0f;
		m[2][0] = m[2][1] = m[2][3] = 0.0f;
		m[3][0] = m[3][1] = m[3][2] = 0.0f;
	}

	void set_zero()
	{
		m[0][0] = m[0][1] = m[0][2] = m[0][3] = 0.0f;
		m[1][0] = m[1][1] = m[1][2] = m[1][3] = 0.0f;
		m[2][0] = m[2][1] = m[2][2] = m[2][3] = 0.0f;
		m[3][0] = m[3][1] = m[3][2] = m[3][3] = 0.0f;
	}

	// 平移变换
	void set_translate(float x, float y, float z)
	{
		set_identity();
		m[3][0] = x;
		m[3][1] = y;
		m[3][2] = z;
	}

	// 缩放变换
	void set_scale(float x, float y, float z)
	{
		set_identity();
		m[0][0] = x;
		m[1][1] = y;
		m[2][2] = z;
	}

	// 旋转变换
	void set_rotation(float x, float y, float z, float theta);

	// 设置相机变换
	void set_lookat(const CVector &eye, const CVector &at, const CVector &up);

	void matrix_set_perspective(float fovy, float aspect, float zn, float zf)
	{
		float fax = 1.0f / (float)tan(fovy * 0.5f);
		set_zero();

		m[0][0] = (fax / aspect);
		m[1][1] = fax;
		m[2][2] = zf / (zf - zn);
		m[3][2] = -zf * zf / (zf - zn);
		m[2][3] = 1;
	}

	CMatrix operator+(const CMatrix &oth)
	{
		CMatrix mat(false);
		int i, j;
		for (i = 0; i < 4; ++i)
			for (j = 0; j < 4; ++j)
				mat.m[i][j] = m[i][j] + oth.m[i][j];

		return mat;
	}

	CMatrix operator-(const CMatrix &oth)
	{
		CMatrix mat(false);
		int i, j;
		for (i = 0; i < 4; ++i)
			for (j = 0; j < 4; ++j)
				mat.m[i][j] = m[i][j] - oth.m[i][j];

		return mat;
	}

	CMatrix operator*(const CMatrix &oth)
	{
		CMatrix mat;
		int i, j;
		for(i=0; i < 4; ++i)
			for (j = 0; j < 4; ++j)
			{
				mat.m[j][i] = m[j][0] * oth.m[0][i] +
							  m[j][1] * oth.m[1][i] +
							  m[j][2] * oth.m[2][i] +
							  m[j][3] * oth.m[3][i];
			}
		return mat;
	}

	CMatrix operator*(const float T)
	{
		CMatrix mat(false);
		int i, j;
		for (i = 0; i < 4; ++i)
			for (j = 0; j < 4; ++j)
				mat.m[i][j] = m[i][j] + T;

		return mat;
	}

	float m[4][4];
};



#endif // !__CMATRIX_H__
