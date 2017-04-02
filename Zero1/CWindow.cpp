#include "CWindow.h"

bool CWindow::CreateCWindow(int width, int height, std::wstring title)
{
	HINSTANCE hInst = GetModuleHandle(NULL);
	HRESULT retsult = S_OK;
	WindowName = title;

	// 创建窗口类
	WNDCLASSEX wndclass;
	ZeroMemory(&wndclass, sizeof(WNDCLASSEX));
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_BYTEALIGNCLIENT;
	wndclass.lpfnWndProc = (WNDPROC)CWindow::WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInst;
	wndclass.hIcon = NULL;
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = CWINDOW_CLASS_NAME;

	retsult = RegisterClassEx(&wndclass);
	if (!retsult)
	{
		MessageBox(NULL, TEXT("注册窗口类失败"), TEXT("错误"), NULL);
		return false;
	}

	// 创建窗口

	HWND hWnd = CreateWindow(CWINDOW_CLASS_NAME, title.c_str(),
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		0, 0, 0, 0, NULL, NULL, hInst, NULL);

	if (!hWnd)
	{
		MessageBox(NULL, TEXT("创建窗口失败"), TEXT("错误"), NULL);
		return false;
	}

	// 调整窗口大小
	RECT rect = { 0,0, width, height };
	AdjustWindowRect(&rect, GetWindowLong(hWnd, GWL_STYLE), 0);

	int wx, wy, sx, sy;
	// 算一下窗口的实际大小
	wx = rect.right - rect.left;
	wy = rect.bottom - rect.top;
	// 根据实际大小来调整窗口左上角的起始位置
	sx = (GetSystemMetrics(SM_CXSCREEN) - wx) / 2;
	sy = (GetSystemMetrics(SM_CYSCREEN) - wy) / 2;

	// SWP_NOCOPYBITS：清除客户区的所有内容。如果未设置该标志，客户区的有效内容被保存并且在窗口尺寸更新和重定位后拷贝回客户区。
	// SWP_NOZORDER：维持当前Z序（忽略hWndlnsertAfter参数）。
	// SWP_SHOWWINDOW：显示窗口。
	SetWindowPos(hWnd, NULL, sx, sy, wx, wy, (SWP_NOCOPYBITS | SWP_NOZORDER | SWP_SHOWWINDOW));
	ShowWindow(hWnd, SW_NORMAL);

	this->hInstance = hInst;
	this->hWnd = hWnd;
	this->WindowWidth = wx;
	this->WindowHeight = wy;
	
	// 创建图像缓存
	pImage = new CImage();
	if (!pImage->CreateImage(*this))
	{
		MessageBox(NULL, TEXT("创建图像缓存时失败"), TEXT("错误"), NULL);
		return false;
	}


	return true;
}

bool CWindow::Run()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
			return false;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return true;
}

void CWindow::LockFPS(int &fps, int maxfps)
{
	if (fps > maxfps)
	{
		fps--;
		Sleep(1);
	}
}

LRESULT CWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return 0;
}



void CWindow::beginScence(bool clear /*= true*/)
{
	if (!clear)
		return;
	memset(pImage->memory, 0, pImage->width * pImage->height * pImage->bitCount / 8);
}

void CWindow::endScence()
{
	HDC hDC = GetDC(hWnd);
	pImage->CopyToDC(hDC, 0, 0, WindowWidth, WindowHeight);
	//BitBlt(hDC, 0, 0, WindowWidth, WindowHeight, pImage->screenDC, 0, 0, SRCCOPY);
	ReleaseDC(hWnd, hDC);
}

//#define DB_MODE_SINGLE   0
//#define DB_MODE_GRADIENT 1

void CWindow::DrawBackground(int mode /*= DB_MODE_SINGLE*/, UINT color /*= 0*/)
{
	int y;
	UINT cc;
	for (y = 0; y < WindowHeight; ++y)
	{
		if (mode == 1)
		{
			cc = (WindowHeight - 1 - y) * 230 / (WindowHeight - 1);
			cc = (cc << 16) | (cc << 8) | (cc);
		}
		else
			cc = color;

		UINT *dst = pImage->frameBuffer + (y * pImage->width);

		// 方法1 汇编填充，以行为单位  2500 fps
		// Release 下， 基本不变，还是 2500 fps
		MemSetQuad(dst, cc, WindowWidth);

		// 方法2, 循环填充，移动指针   500 fps
		// Release 下， 2300-2500 fps
		/*for (int x = WindowWidth; x > 0; dst++, x--)
		dst[0] = cc;*/

		// 方法3 循环调用函数填充      60 fps
		// 使用内联函数, 在 Release下 可达到 800-900fps
		/*for (int x = 0; x < WindowWidth; ++x)
		{
		pImage->SetPixel(x,y,cc);
		}*/
	}
}

void CWindow::TestScence1(bool clear)
{
	static bool first = true;
	beginScence(clear);
	if (clear)
		DrawBackground(DB_MODE_SINGLE, 0);
	else
	{
		if (first)
		{
			DrawBackground(DB_MODE_GRADIENT);
			first = false;
		}
	}

	for(int x=0; x < 10; ++x)
		pImage->SetPixel(rand() % pImage->width, rand() % pImage->height, (rand() % 255) << 16 | (rand()%255)<<8 | rand()%255);

	endScence();
}

void CWindow::TestScence2(bool clear)
{
	static bool first = true;
	beginScence(clear);
	if (clear)
		DrawBackground(DB_MODE_SINGLE, 0);
	else
	{
		if (first)
		{
			DrawBackground(DB_MODE_GRADIENT);
			first = false;
		}
	}

	for (int x = 0; x < 100; ++x)
		pImage->DrawLine(rand() % pImage->width, rand() % pImage->height,
				         rand() % pImage->width, rand() % pImage->height,
			            (rand() % 255) << 16 | (rand() % 255) << 8 | rand() % 255);


	endScence();
}

void CWindow::TestScence3()
{
	beginScence(SCENCE_NO_CLEAR);

	for (int x = 0; x < 1; ++x)
		pImage->DrawRectangle(rand() % pImage->width, rand() % pImage->height,
			rand() % pImage->width, rand() % pImage->height,
			(rand() % 255) << 16 | (rand() % 255) << 8 | rand() % 255);

	endScence();
}

