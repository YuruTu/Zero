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
	renderState = RD_STATE_COLOR;

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
	
	// 创建图像缓存
	pBackBuffer = new CImage();
	if (!pBackBuffer->CreateImage(*this))
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



void CWindow::BeginScence(bool clear /*= true*/)
{
	if (!clear)
		return;
	memset(pBackBuffer->memory, 0, pBackBuffer->width * pBackBuffer->height * pBackBuffer->bitCount / 8);
}

void CWindow::EndScence()
{
	HDC hDC = GetDC(hWnd);
	pBackBuffer->CopyToDC(hDC, 0, 0, windowWidth, windowHeight);
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

		UINT *dst = pBackBuffer->frameBuffer + (y * pBackBuffer->width);

		// 方法1 汇编填充，以行为单位  2500 fps
		// Release 下， 基本不变，还是 2500 fps
		MemSetQuad(dst, cc, windowWidth);

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
	BeginScence(clear);
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
		pBackBuffer->DrawPixel(rand() % pBackBuffer->width, rand() % pBackBuffer->height, (rand() % 255) << 16 | (rand()%255)<<8 | rand()%255);

	EndScence();
}

void CWindow::TestScence2(bool clear)
{
	static bool first = true;
	BeginScence(clear);
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
		pBackBuffer->DrawLine(rand() % pBackBuffer->width, rand() % pBackBuffer->height,
				         rand() % pBackBuffer->width, rand() % pBackBuffer->height,
			            (rand() % 255) << 16 | (rand() % 255) << 8 | rand() % 255);


	EndScence();
}

void CWindow::TestScence3()
{
	BeginScence(SCENCE_NO_CLEAR);

	for (int x = 0; x < 10; ++x)
		pBackBuffer->DrawRectangle(rand() % pBackBuffer->width, rand() % pBackBuffer->height,
			rand() % pBackBuffer->width, rand() % pBackBuffer->height,
			(rand() % 255) << 16 | (rand() % 255) << 8 | rand() % 255);

	EndScence();
}

CMatrix s(false), r(false), t(false), w(false);
bool first4 = true;
void CWindow::TestScence4()
{
	static float theta = 0;
	BeginScence();
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
		pBackBuffer->DrawLine(v[i].x, v[i].y, v[i + 1].x, v[i + 1].y, 0);
	pBackBuffer->DrawLine(v[0].x, v[0].y, v[3].x, v[3].y, 0);

	pBackBuffer->DrawLine(v[0].x, v[0].y, v[2].x, v[2].y, 0);
	pBackBuffer->DrawLine(v[1].x, v[1].y, v[3].x, v[3].y, 0);

	EndScence();
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

	BeginScence();
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
		pBackBuffer->DrawPixel(v.x, v.y - 2, (int(v.x) << 8) | int(v.y));
		pBackBuffer->DrawPixel(v.x, v.y - 1, (int(v.x) << 8) | int(v.y));
		pBackBuffer->DrawPixel(v.x, v.y,     (int(v.x) << 8) | int(v.y));
		pBackBuffer->DrawPixel(v.x, v.y + 1, (int(v.x) << 8) | int(v.y));
		pBackBuffer->DrawPixel(v.x, v.y + 2, (int(v.x) << 8) | int(v.y));
	}



	EndScence();
}


CVertex mesh[8] = 
{
	{ { 1, -1,  1, 1 }, {1,0.2,0.2} }, 
	{ { -1, -1,  1, 1 },{0.2,1.0,0.2},},
	{ { -1,  1,  1, 1 },{0.2,0.2,1.0}},
	{ { 1,  1,  1, 1 }, {1.0,0.2,1.0}},
	{ { 1, -1, -1, 1 }, {1.0,1.0,0.2}},
	{ { -1, -1, -1, 1 },{0.2,1.0,1.0}},
	{ { -1,  1, -1, 1 },{1.0,0.3,0.3}},
	{ { 1,  1, -1, 1 }, {0.2,1.0,0.3}},
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
		transform.getView().set_lookat(CVector(8, 0, 0, 1), CVector(0, 0, 0, 1), CVector(0, 0, 1));
		transform.getProj().set_perspective(3.1415926f*0.5,
			float(windowWidth) / float(windowHeight),
			1.0, 500.0f);
		boxfirst = false;
	}

	BeginScence();
	DrawBackground(DB_MODE_GRADIENT);
	static float theta = 0;
	CMatrix m,t,ww;
	theta += 0.001;
	t.set_rotation(0, -1, 1, 1.5*theta);
	m.set_rotation(0, 0, 1, theta);
	m = m*t;

	// 是否消除背面
	backfaceCull = true;

	int w = windowWidth;
	int h = windowHeight;
	int z, y;
	
	for (int i = 0; i < 3; ++i)
	{
		z = -3 + i * 3;
		for (int j = 0; j < 3; ++j)
		{
			y = -3 + j * 3;
			t.set_translate(0, y, z);
			ww = m*t;
			transform.getWorld() = ww;
			DrawBox(pBackBuffer);
		}
	}
	
	/*
	for (int i = 0; i < 11; ++i)
	{
		z = -15 + i * 3;
		for (int j = 0; j < 11; ++j)
		{
			y = -15 + j * 3;
			t.set_translate(0, y, z);
			ww = m*t;
			transform.getWorld() = ww;
			DrawBox(pBackBuffer);
		}
	}
	*/

	/*
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
	*/

	EndScence();
}