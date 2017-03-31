#pragma once

#ifndef __CWINDOW_H__
#define __CWINDOW_H__

#include <Windows.h>
#include <string>
#include "CImage.h"

#define CWINDOW_CLASS_NAME TEXT("ZERO_WINDOW")

class CImage;

class CWindow
{
public:
	bool CreateCWindow(int width, int height, std::wstring lpName);

	bool Run();
	static LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
	std::string WindowName;
	int WindowWidth, WindowHeight;

	HINSTANCE hInstance;
	HWND      hWnd;
	CImage    *pImage;

};

#endif // !__CWINDOW_H__