#include "CWindow.h"

bool CWindow::CreateCWindow(int width, int height, std::wstring title)
{
	HINSTANCE hInst = GetModuleHandle(NULL);
	HRESULT retsult = S_OK;
	WindowName = title;

	// ����������
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
		MessageBox(NULL, TEXT("ע�ᴰ����ʧ��"), TEXT("����"), NULL);
		return false;
	}

	// ��������

	HWND hWnd = CreateWindow(CWINDOW_CLASS_NAME, title.c_str(),
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		0, 0, 0, 0, NULL, NULL, hInst, NULL);

	if (!hWnd)
	{
		MessageBox(NULL, TEXT("��������ʧ��"), TEXT("����"), NULL);
		return false;
	}

	// �������ڴ�С
	RECT rect = { 0,0, width, height };
	AdjustWindowRect(&rect, GetWindowLong(hWnd, GWL_STYLE), 0);

	int wx, wy, sx, sy;
	// ��һ�´��ڵ�ʵ�ʴ�С
	wx = rect.right - rect.left;
	wy = rect.bottom - rect.top;
	// ����ʵ�ʴ�С�������������Ͻǵ���ʼλ��
	sx = (GetSystemMetrics(SM_CXSCREEN) - wx) / 2;
	sy = (GetSystemMetrics(SM_CYSCREEN) - wy) / 2;

	// SWP_NOCOPYBITS������ͻ������������ݡ����δ���øñ�־���ͻ�������Ч���ݱ����沢���ڴ��ڳߴ���º��ض�λ�󿽱��ؿͻ�����
	// SWP_NOZORDER��ά�ֵ�ǰZ�򣨺���hWndlnsertAfter��������
	// SWP_SHOWWINDOW����ʾ���ڡ�
	SetWindowPos(hWnd, NULL, sx, sy, wx, wy, (SWP_NOCOPYBITS | SWP_NOZORDER | SWP_SHOWWINDOW));
	ShowWindow(hWnd, SW_NORMAL);

	this->hInstance = hInst;
	this->hWnd = hWnd;
	this->WindowWidth = wx;
	this->WindowHeight = wy;
	
	// ����ͼ�񻺴�
	pImage = new CImage();
	if (!pImage->CreateImage(*this))
	{
		MessageBox(NULL, TEXT("����ͼ�񻺴�ʱʧ��"), TEXT("����"), NULL);
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

		// ����1 �����䣬����Ϊ��λ  2500 fps
		// Release �£� �������䣬���� 2500 fps
		MemSetQuad(dst, cc, WindowWidth);

		// ����2, ѭ����䣬�ƶ�ָ��   500 fps
		// Release �£� 2300-2500 fps
		/*for (int x = WindowWidth; x > 0; dst++, x--)
		dst[0] = cc;*/

		// ����3 ѭ�����ú������      60 fps
		// ʹ����������, �� Release�� �ɴﵽ 800-900fps
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

