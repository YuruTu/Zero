#include "CWindow.h"
#include "CMatrix.h"

bool CWindow::CreateCWindow(int width, int height, std::wstring title)
{
	HINSTANCE hInst = GetModuleHandle(NULL);
	HRESULT retsult = S_OK;
	windowName = title;
	this->windowWidth = width;
	this->windowHeight = height;
	backfaceCull = false;

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
	pImage->CopyToDC(hDC, 0, 0, windowWidth, windowHeight);
	//BitBlt(hDC, 0, 0, WindowWidth, WindowHeight, pImage->screenDC, 0, 0, SRCCOPY);
	ReleaseDC(hWnd, hDC);
}

//#define DB_MODE_SINGLE   0
//#define DB_MODE_GRADIENT 1

void CWindow::DrawBackground(int mode /*= DB_MODE_SINGLE*/, UINT color /*= 0*/)
{
	int y;
	UINT cc;
	for (y = 0; y < windowHeight; ++y)
	{
		if (mode == 1)
		{
			cc = (windowHeight - 1 - y) * 230 / (windowHeight - 1);
			cc = (cc << 16) | (cc << 8) | (cc);
		}
		else
			cc = color;

		UINT *dst = pImage->frameBuffer + (y * pImage->width);

		// ����1 �����䣬����Ϊ��λ  2500 fps
		// Release �£� �������䣬���� 2500 fps
		MemSetQuad(dst, cc, windowWidth);

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

	for (int x = 0; x < 10; ++x)
		pImage->DrawLine(rand() % pImage->width, rand() % pImage->height,
				         rand() % pImage->width, rand() % pImage->height,
			            (rand() % 255) << 16 | (rand() % 255) << 8 | rand() % 255);


	endScence();
}

void CWindow::TestScence3()
{
	beginScence(SCENCE_NO_CLEAR);

	for (int x = 0; x < 10; ++x)
		pImage->DrawRectangle(rand() % pImage->width, rand() % pImage->height,
			rand() % pImage->width, rand() % pImage->height,
			(rand() % 255) << 16 | (rand() % 255) << 8 | rand() % 255);

	endScence();
}

CMatrix s(false), r(false), t(false), w(false);
bool first4 = true;
void CWindow::TestScence4()
{
	static float theta = 0;
	beginScence();
	DrawBackground(DB_MODE_GRADIENT);

	
	if (first4)
	{
		s.set_scale(1, 1, 1);
		r.set_identity();
		r.m[1][1] = -1;
		t.set_translate(1.0 * windowWidth / 2, 1.0 * windowHeight / 2, 0);

		w = s * r;
		w = w * t;
		first4 = false;
	}

	CVector v[4] = { { -100,-100,0,1 },
	{ 100,-100,0,1 },
	{ 100,100,0,1 },
	{ -100,100,0,1 },
	};

	theta += 0.01;

	CMatrix r0(false);
	r0.set_rotation(0, 0, 1, theta * 3.1415926 / 180);

	for (int i = 0; i < 4; ++i)
	{
		v[i] *= r0;
		v[i] *= w;
	}

	for (int i = 0; i < 3; ++i)
		pImage->DrawLine(v[i].x, v[i].y, v[i + 1].x, v[i + 1].y, 0);
	pImage->DrawLine(v[0].x, v[0].y, v[3].x, v[3].y, 0);

	pImage->DrawLine(v[0].x, v[0].y, v[2].x, v[2].y, 0);
	pImage->DrawLine(v[1].x, v[1].y, v[3].x, v[3].y, 0);

	endScence();
}

void CWindow::TestScence5()
{
	CMatrix s, r, r1, t, w;
	s.set_scale(1, 1, 1);
	r.set_identity();
	r.m[1][1] = -1;
	t.set_translate(1.0 * windowWidth / 2, 1.0 * windowHeight / 2, 0);

	w = s * r;
	w = w * t;

	static float theta = 0;
	CVector v;
	v.w = 1;

	beginScence();
	DrawBackground(DB_MODE_GRADIENT);

	float x, y;
	x = -windowWidth / 2;

	theta += 0.001;
	for (; x <= windowWidth / 2; ++x)
	{
		y = 100 * cos((x - windowWidth / 2.0) / windowWidth * 3.1415926 * 3.5 + theta);

		v.x = x;
		v.y = y;

		v = v * w;
		pImage->SetPixel(v.x, v.y - 2, (int(v.x) << 8) | int(v.y));
		pImage->SetPixel(v.x, v.y - 1, (int(v.x) << 8) | int(v.y));
		pImage->SetPixel(v.x, v.y, (int(v.x) << 8) | int(v.y));
		pImage->SetPixel(v.x, v.y + 1, (int(v.x) << 8) | int(v.y));
		pImage->SetPixel(v.x, v.y + 2, (int(v.x) << 8) | int(v.y));
	}



	endScence();
}


CVertex mesh[8] = 
{
	{ { 1, -1,  1, 1 }, },
	{ { -1, -1,  1, 1 },},
	{ { -1,  1,  1, 1 },},
	{ { 1,  1,  1, 1 }, },
	{ { 1, -1, -1, 1 }, },
	{ { -1, -1, -1, 1 },},
	{ { -1,  1, -1, 1 },},
	{ { 1,  1, -1, 1 }, },
};


void DrawPlane(CImage *pImage, int a, int b, int c, int d)
{
	CVertex p0 = mesh[a], p1 = mesh[b], p2 = mesh[c], p3 = mesh[d];
	pImage->DrawPrimitive(p0, p1, p2);
	pImage->DrawPrimitive(p0, p2, p3);
}

void DrawBox(CImage *pImage)
{
	DrawPlane(pImage, 0, 3, 2, 1);
	DrawPlane(pImage, 5, 6, 7, 4);
	DrawPlane(pImage, 0, 1, 5, 4);
	DrawPlane(pImage, 1, 2, 6, 5);
	DrawPlane(pImage, 2, 3, 7, 6);
	DrawPlane(pImage, 0, 4, 7, 3);
}

bool boxfirst = true;
void CWindow::TestBox()
{
	if (boxfirst)
	{
		transform.getWorld().set_identity();
		transform.getView().set_lookat(CVector(40, 0, 0, 1), CVector(0, 0, 0, 1), CVector(0, 0, 1));
		transform.getProj().set_perspective(3.1415926f*0.5,
			float(windowWidth) / float(windowHeight),
			1.0, 500.0f);
		boxfirst = false;
	}

	beginScence();
	DrawBackground(DB_MODE_GRADIENT);
	static float theta = 0;
	CMatrix m,t,ww;
	m.set_rotation(0, 0, 1, theta);
	theta += 0.0005;

	// �Ƿ���������
	backfaceCull = true;

	int w = windowWidth;
	int h = windowHeight;
	int z, y;

	/*
	for (int i = 0; i < 3; ++i)
	{
		z = -3 + i * 3;
		if (i > 1)
			pImage->tmpLine = 2;
		for (int j = 0; j < 3; ++j)
		{
			y = -3 + j * 3;
			t.set_translate(0, y, z);
			ww = m*t;
			transform.getWorld() = ww;
			DrawBox(pImage);
		}
	}
	*/

	pImage->tmpLine = 1;
	for (int i = 0; i < 20; ++i)
	{
		z = -25 + i * 3;
		
		for (int j = 0; j < 20; ++j)
		{
			y = -25 + j * 3;
			t.set_translate(0, y, z);
			ww = m*t;
			transform.getWorld() = ww;
			DrawBox(pImage);
		}
	}
	

	endScence();
}