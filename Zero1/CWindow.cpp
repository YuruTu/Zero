#include "CWindow.h"

bool CWindow::CreateCWindow(int width, int height, std::wstring lpName)
{
	HINSTANCE hInst = GetModuleHandle(NULL);
	HRESULT retsult = S_OK;

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

	HWND hWnd = CreateWindow(CWINDOW_CLASS_NAME, lpName.c_str(),
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

void CWindow::beginScence()
{
	for(int y=0; y < WindowHeight; ++y)
	{
		UINT cc = (WindowHeight - 1 - y) * 230 / (WindowHeight - 1);
		cc = (cc << 16) | (cc << 8) | (cc);
		for (int x = 0; x < WindowWidth; ++x)
			pImage->SetPixel(x, y, cc);
	}
}

void CWindow::endScence()
{
	HDC hDC = GetDC(hWnd);
	BitBlt(hDC, 0, 0, WindowWidth, WindowHeight, pImage->screenDC, 0, 0, SRCCOPY);
	ReleaseDC(hWnd, hDC);
}
