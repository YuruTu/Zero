

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
	
	CImage *pImage = wnd.pImage;
	wnd.DrawBackground(DB_MODE_GRADIENT);
	
	CMatrix s, r, r1, t, w;
	s.set_scale(1, 1, 1);
	r.set_identity();
	r.m[1][1] = -1;
	t.set_translate(1.0 * wnd.WindowWidth / 2, 1.0 * wnd.WindowHeight / 2, 0);

	w = s * r;
	w = w * t;

	float theta = 0;
	CVector v;
	v.w = 1;
	while (wnd.Run())
	{
		
		/*wnd.beginScence();
		wnd.DrawBackground(DB_MODE_GRADIENT);

		float x, y;
		x = -wnd.WindowWidth / 2;

		theta += 0.001;
		for (; x <= wnd.WindowWidth / 2; ++x)
		{
			y = 100 * cos((x - wnd.WindowWidth / 2.0)/ wnd.WindowWidth * 3.1415926 * 3.5 + theta);

			v.x = x;
			v.y = y;
			
			v = v * w;
			pImage->SetPixel(v.x, v.y-2,  (int(v.x) << 8) | int(v.y));
			pImage->SetPixel(v.x, v.y-1,  (int(v.x) << 8) | int(v.y));
			pImage->SetPixel(v.x, v.y, (int(v.x)<<8) | int(v.y));
			pImage->SetPixel(v.x, v.y+1, (int(v.x) << 8) | int(v.y));
			pImage->SetPixel(v.x, v.y+2, (int(v.x) << 8) | int(v.y));
		}

		wnd.endScence();*/

		//wnd.TestScence4();
		wnd.TestScence5();
		
		++fps;
		QueryPerformanceCounter(&now);
		double past = 1.0 * (now.LowPart - last.LowPart) / freq.QuadPart;
		if (past >= 1)
		{
			last = now;
			swprintf(Title, L"%s fps: %d", wnd.WindowName.c_str(), fps);
			SetWindowText(wnd.hWnd, Title);
			fps = 0;
		}

		//wnd.LockFPS(fps, 60);
	}

	return 0;
}