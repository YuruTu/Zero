

#include "CWindow.h"
#include "CMatrix.h"
#include "CVector.h"
#include <iostream>
using namespace std;

int main()
{
	WCHAR Title[1024];
	CWindow wnd;
	int fps = 0;

	wnd.CreateCWindow(800, 600, TEXT("Zero 01"));


	LARGE_INTEGER freq, last, now;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&last);
	now = last;
	
	CImage *pImage = wnd.pBackBuffer;
	wnd.DrawBackground(DB_MODE_GRADIENT);
	
	CMatrix s, r, t, w;
	s.set_scale(10, 10, 10);
	r.set_identity();
	r.m[1][1] = -1;
	t.set_translate(static_cast<float>(1.0 * wnd.windowWidth / 2), float(1.0 * wnd.windowHeight / 2), 0.0f);

	w = s * r;
	w = w * t;

	wnd.screen = w;

	wnd.transform.getWorld().set_identity();
	wnd.transform.getView().set_lookat(CVector(10, 0, 0,1), CVector(0,0,0,1), CVector(0, 0, 1));
	wnd.transform.getProj().set_perspective(3.1415926f*0.5,
		float(wnd.windowWidth) / float(wnd.windowHeight),
		1.0, 500.0f);

	pImage->LineMode = 1;
	wnd.renderState = /*RD_STATE_WIREFRAME |*/ RD_STATE_COLOR;

	while (wnd.Run())
	{

		//wnd.TestScence4();
		wnd.TestBox();
		
		++fps;
		QueryPerformanceCounter(&now);
		double past = 1.0 * (now.LowPart - last.LowPart) / freq.QuadPart;
		if (past >= 1)
		{
			last = now;
			swprintf(Title, L"%s fps: %d", wnd.windowName.c_str(), fps);
			SetWindowText(wnd.hWnd, Title);
			fps = 0;
		}

		//wnd.LockFPS(fps, 60);
	}

	return 0;
}