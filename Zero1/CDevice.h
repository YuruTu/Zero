#pragma once

#ifndef __CDEVICE_H__
#define __CDEVICE_H__

#include <Windows.h>
#include <string>
#include "CWindow.h"

class CDevice
{
public:
	CDevice(int width, int height, std::wstring title);
	~CDevice();

public:
	bool run();
	void yield();
	void sleep(UINT timeMs);
	void setWindowCaption(std::wstring caption);


public:
	CWindow *pWindow;
	CImage  *pImage;
};

#endif // !__CDEVICE_H__
