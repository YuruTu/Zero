#pragma once

#ifndef __CWINDOW_H__
#define __CWINDOW_H__

#include <windows.h>
#include <string>
#include "CImage.h"

#define CWINDOW_CLASS_NAME TEXT("ZERO_WINDOW")

class CImage;

#define SCENCE_CLEAR true
#define SCENCE_NO_CLEAR false

#define DB_MODE_SINGLE   0
#define DB_MODE_GRADIENT 1

class CWindow
{
public:
	bool CreateCWindow(int width, int height, std::wstring title);

	bool Run();
	void LockFPS(int &fps, int maxfps);
	static LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void beginScence(bool clear = SCENCE_CLEAR);
	void endScence();

	void DrawBackground(int mode = 0, UINT color = 0);


public:
	void TestScence1(bool clear = SCENCE_NO_CLEAR);  // point
	void TestScence2(bool clear = SCENCE_NO_CLEAR);  // line
	void TestScence3();							     // triangle
	void TestScence4();
	void TestScence5();

public:
	std::wstring WindowName;
	int WindowWidth, WindowHeight;

	HINSTANCE hInstance;
	HWND      hWnd;
	CImage    *pImage;

};

inline void MemSetQuad(void *dest, UINT data, int count)
{
	__asm
	{
		mov edi, dest;
		mov ecx, count;
		mov eax, data;
		rep stosd;
	}
}

#endif // !__CWINDOW_H__