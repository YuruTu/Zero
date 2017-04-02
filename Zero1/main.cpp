

#include "CWindow.h"
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
	
	while (wnd.Run())
	{
		
		/*wnd.beginScence(SCENCE_NO_CLEAR);
		
		for (int x = 0; x < 10; ++x)
			pImage->DrawRectangle(rand() % pImage->width, rand() % pImage->height,
								  rand() % pImage->width, rand() % pImage->height,
								 (rand() % 255) << 16 | (rand() % 255) << 8 | rand() % 255);

		wnd.endScence();*/

		wnd.TestScence3();

		
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

		//wnd.LockFPS(fps, 10000);
	}

	return 0;
}