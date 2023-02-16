#include <windows.h>
#include <chrono>	// 获取时间
#include <thread>
#include <fstream>
#include "resource.h"
#define IDR_QUIT 1
#define BMPWIDTH 130
#define BMPHEIGHT 384

int width = 0;
int height = 0;
int start_x = 0;
int start_y = 0;

HBITMAP pics[11];	// 11张辉光管图

// 更新画图位置
void getSize(HWND hWnd) {
	RECT size;
	GetWindowRect(hWnd, &size);
	height = (size.bottom - size.top) / 3;
	width = height * BMPWIDTH / BMPHEIGHT;
	start_y = height;
	start_x = (size.right - size.left - 8 * width) / 2;
}

// 绘制窗口
void paint_clock(HDC hdc){
	// 获取时间
	time_t t = time(0);
	tm T;
	localtime_s(&T, &t);
	// 得到并分解时间
	char time[8] = { T.tm_hour / 10, T.tm_hour % 10, 10, T.tm_min / 10, T.tm_min % 10, 10, T.tm_sec / 10, T.tm_sec % 10 };
	// 双缓冲画图
	// 1.创建内存dc
	HDC hMemdc = CreateCompatibleDC(hdc);
	HBITMAP canvas = CreateCompatibleBitmap(hdc, 8 * BMPWIDTH, BMPHEIGHT);
	HGDIOBJ Oldbmp = SelectObject(hMemdc, canvas);
	// 2.位图一个个画入hMemdc
	HDC temp = CreateCompatibleDC(hdc);
	for (int i = 0; i < 8; i++) {
		// 向temp中填入图片
		HGDIOBJ oldbmp = SelectObject(temp, pics[(int)time[i]]);
		// 将temp粘贴至hMemdc
		BitBlt(hMemdc, i * BMPWIDTH, 0, BMPWIDTH, BMPHEIGHT, temp, 0, 0, SRCCOPY);
		// 把空图片填回去
		SelectObject(temp, oldbmp);
	}
	// 3.把内存dc变换成合适大小放入原图
	StretchBlt(hdc, start_x, start_y, 8*width, height, hMemdc, 0, 0, 8*BMPWIDTH, BMPHEIGHT, SRCCOPY);
	// 4.收尾
	SelectObject(hMemdc, Oldbmp);
	DeleteObject(canvas);
	DeleteDC(temp);
	DeleteDC(hMemdc);
}

// 窗口名字
LPCTSTR szWindowClass = TEXT("Steins_WallPaper");
LPCTSTR szTitle = TEXT("Steins_WallPaper");
HMENU hmenu;//菜单句柄
NOTIFYICONDATA nid;

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	switch(message){
	case WM_TIMER: {	// 当计时器到点时强制重新绘制窗口 直接发送WM_PAINT没用
		// 这个矩形是将要被刷新的区域，矩形外的像素不会被刷新
		RECT rc = { start_x, start_y, start_x + 8 * width, start_y + height };
		// 强制让系统刷新这个区域，第三个参数决定了是否擦除背景
		InvalidateRect(hwnd, &rc, FALSE);
		break;
	}
	case WM_PAINT: {
		PAINTSTRUCT ps = { 0 };
		HDC hdc = BeginPaint(hwnd, &ps);
		paint_clock(hdc);
		EndPaint(hwnd, &ps);
		break;
	}
	case WM_SIZE:		// 计算新的位置
		getSize(hwnd);
		break;
	case WM_CREATE: {	//窗口创建时做托盘
			nid.cbSize = sizeof(nid);
			nid.hWnd = hwnd;
			nid.uID = 0;
			nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
			nid.uCallbackMessage = WM_USER;
			nid.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
        lstrcpy(nid.szTip, szWindowClass);
        hmenu = CreatePopupMenu();//生成菜单
			AppendMenu(hmenu, MF_STRING, IDR_QUIT, TEXT("EXIT"));
		Shell_NotifyIcon(NIM_ADD, &nid);
        break;
	}
	case WM_USER: 
        if (lParam == WM_RBUTTONDOWN) {
			POINT pt;
            GetCursorPos(&pt);//取鼠标坐标
            SetForegroundWindow(hwnd);//解决在菜单外单击左键菜单不消失的问题
			switch(TrackPopupMenu(hmenu, TPM_RETURNCMD, pt.x, pt.y, NULL, hwnd, NULL)){
				case IDR_QUIT:
					Shell_NotifyIcon(NIM_DELETE, &nid);
					KillTimer(hwnd, 0);
					PostQuitMessage(0);
			}
        }
        break;
	case WM_DESTROY:	// 窗口销毁时候的消息
		Shell_NotifyIcon(NIM_DELETE, &nid);
		KillTimer(hwnd, 0);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
		break;
	}
	return 0;
}

// 隐藏上面的窗口
BOOL CALLBACK EnumWindowsProc(_In_ HWND hwnd, _In_ LPARAM Lparam){
	HWND hDefView = FindWindowEx(hwnd, 0, L"SHELLDLL_DefView", 0);
	if (hDefView != 0) {
		HWND hWorkerw = FindWindowEx(0, hwnd, L"WorkerW", 0);
		ShowWindow(hWorkerw, SW_HIDE);
		return FALSE;
	}
	return TRUE;
}

// 加入桌面特殊层
void setWallPaper(HWND wallpaper) {
	HWND hProgman = FindWindow(L"Progman", 0);
	SendMessageTimeout(hProgman, 0x52C, 0, 0, 0, 100, 0);
	SetParent(wallpaper, hProgman);
	EnumWindows(EnumWindowsProc, 0);
}

int WINAPI WinMain(		// 不写_In_会警告
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPreInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow
){
	// 初始化位图数据
	for(int i=0;i<11;i++)
		pics[i] = LoadBitmap(hInstance, MAKEINTRESOURCE(101 + i));
	// 调整dpi 如果不调整，在缩放非100%的情况下会出现窗口不铺满的情况
	if (!SetProcessDPIAware()) {	// 以编程方式设置默认感知 必须在hwnd创建之前设置
		MessageBox(NULL,
			TEXT("Call to SetProcessDPIAware failed!"),
			TEXT("beshar project"),
			NULL); return 1;
	}
	// 创建窗口类
	WNDCLASSEX wcex;
		wcex.cbSize = sizeof(wcex);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.lpfnWndProc = WndProc;
		wcex.hInstance = hInstance;
		wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
		wcex.hIconSm = LoadIconW(hInstance, IDI_APPLICATION);
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground = CreateSolidBrush(BLACK_PEN);
		wcex.lpszClassName = szWindowClass;
		wcex.lpszMenuName = NULL;
	// 注册窗口类
	if (!RegisterClassEx(&wcex)) {
		MessageBox(NULL,
			TEXT("Call to registerclass filed!"),
			TEXT("beshar project"),
			NULL
		); return 1;
	}
	// 创建窗口
	HWND hwnd = CreateWindowEx(
		NULL,
		szWindowClass,
		szTitle,
		WS_POPUP | WS_MAXIMIZE,	// POPUP弹出窗口没有边框
		0,
		0,
		GetSystemMetrics(SM_CXSCREEN),	// 获取屏幕大小
		GetSystemMetrics(SM_CYSCREEN),
		NULL,
		NULL,
		hInstance,
		NULL
	);
	if (!hwnd) {
		MessageBox(NULL,
			TEXT("Call to createwindowex failed!"),
			TEXT("beshar project"),
			NULL); return 1;
	}
	// 计时器
	SetTimer(hwnd, 0, 1000, NULL);
	// 显示窗口
	setWallPaper(hwnd);
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	// 消息循环
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}

// 参考资料：https://www.bilibili.com/video/BV1HZ4y1978a