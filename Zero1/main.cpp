

#include "CWindow.h"


int main()
{
	CWindow wnd;

	wnd.CreateCWindow(800, 600, TEXT("233"));

	while (wnd.Run())
	{
		HDC hDC = GetDC(wnd.hWnd);
		BitBlt(hDC, 0, 0, wnd.WindowWidth, wnd.WindowHeight, wnd.pImage->screenDC, 0, 0, SRCCOPY);
		ReleaseDC(wnd.hWnd, hDC);
	}

	return 0;
}