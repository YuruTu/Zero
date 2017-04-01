

#include "CWindow.h"


int main()
{
	CWindow wnd;

	wnd.CreateCWindow(800, 600, TEXT("233"));

	while (wnd.Run())
	{
		wnd.beginSence();
		wnd.endSence();
	}

	return 0;
}