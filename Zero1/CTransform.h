#pragma once

#ifndef __CDEVICE_H__
#define __CDEVICE_H__

#include "CWindow.h"
#include "CMatrix.h"
/*
	一个 Primitive 的所有变换矩阵
*/
class CTransform
{
private:
	CMatrix world;
	CMatrix view;
	CMatrix projection;
	CMatrix transform;

	bool bUpdate;
	void update()
	{
		bUpdate = true;
		transform = (world * view) * projection;
	}

public:
	CMatrix& getWorld() { bUpdate = false; return world; }
	CMatrix& getView() { bUpdate = false; return view; }
	CMatrix& getProj() { bUpdate = false; return projection; }
	CMatrix& getTrans() 
	{ 
		if (!bUpdate)
			update();
		return transform; 
	}
	
	CTransform() : bUpdate(false) {}

	static int check_cvv(CVector &v)
	{
		float w = v.w;
		int ret = 0;
		if (v.z < 0.0f) ret |= 0x01;
		if (v.z >  w)   ret |= 0x02;
		if (v.x < -w)   ret |= 0x04;
		if (v.x >  w)   ret |= 0x08;
		if (v.y < -w)   ret |= 0x10;
		if (v.y > w)    ret |= 0x20;
		return ret;
	}
	static void homogenize(CVector &v)
	{
		float rhw = 1.0f / v.w;
		v *= rhw;
		// keep w
		//v.w = 1.0f;
	}
	CVector getLookat()
	{
		return CVector(view.m[0][2], view.m[1][2], view.m[2][2]);
	}

	CVector getEye()
	{
		return CVector(-view.m[3][0], -view.m[3][1], -view.m[3][2], 1.0f);
	}
};


#endif // !__CDEVICE_H__
