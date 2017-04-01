#pragma once

#ifndef __CDEVICE_H__
#define __CDEVICE_H__

#include "CWindow.h"
#include "CMatrix.h"

class CTransform
{
public:
	CMatrix world;
	CMatrix view;
	CMatrix projection;
	CMatrix transform;
};


class CDevice
{
	CTransform transform;
	int width, height;
};


#endif // !__CDEVICE_H__
