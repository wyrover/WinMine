/*-------------------------------------------------------
		扫雷程序


                            _ooOoo_
                           o8888888o
                           88" . "88
                           (| -_- |)
                            O\ = /O
                        ____/`---'\____
                      .   ' \\| |// `.
                       / \\||| : |||// \
                     / _||||| -:- |||||- \
                       | | \\\ - /// | |
                     | \_| ''\---/'' | |
                      \ .-\__ `-` ___/-. /
                   ___`. .' /--.--\ `. . __
                ."" '< `.___\_<|>_/___.' >'"".
               | | : `- \`.;`\ _ /`;.`/ - ` : | |
                 \ \ `-. \_ __\ /__ _/ .-` / /
         ======`-.____`-.___\_____/___.-`____.-'======
                            `=---='

         .............................................
                  佛祖保佑             永无BUG

-----------------------------------------------------------------*/

#include <windows.h>
#include "resource.h"
#include "winmine.h"
#include "functions.cpp"




LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT("Winmine");
	HWND         hwnd;
	MSG          msg;
	WNDCLASS     wndclass;

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;

	if (!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("This program requires Windows NT!"),
			szAppName, MB_ICONERROR);
		return 0;
	}

	hwnd = CreateWindow(szAppName, TEXT("扫雷"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL);

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HBITMAP hBitmap;
	static int mineRowNum, mineColNum,mineNum;
	static int     cxClient, cyClient, cxSource, cySource;
	static int mi,mj;
	bool layMineFlag = false;
	BITMAP         bitmap;
	HDC            hdc, hdcMem;
	HINSTANCE      hInstance;
	int            x = 0, y = 0;
	PAINTSTRUCT    ps;

	switch (message)
	{
	case WM_CREATE:
		hInstance = ((LPCREATESTRUCT)lParam)->hInstance;
		hBitmap = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP1));
		GetObject(hBitmap, sizeof (BITMAP), &bitmap);
		cxSource = bitmap.bmWidth;
		cySource = bitmap.bmHeight;
		
		return 0;

	case WM_SIZE:
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		mineRowNum = DEFAULT_ROW_NUM;
		mineColNum = DEFAULT_COL_NUM;
		mineNum = DEFAULT_MINE_NUM;
		//找到第一个点使雷区在窗口中央
		x = cxClient/2  - (cxSource*mineRowNum) / 2;
		y = cyClient/2  - (cxSource*mineColNum) / 2;
		hdcMem = CreateCompatibleDC(hdc); //内存区域的句柄
		SelectObject(hdcMem, hBitmap);
		//贴砖
		for (int xi = x; xi < (cxClient + cxSource*mineRowNum) / 2; xi += cxSource)
		{
			for (int yi = y; yi <(cyClient + cxSource*mineColNum) / 2; yi += cxSource)
			{
				BitBlt(hdc, xi, yi, cxSource, cySource / 16, hdcMem, 0, 0, SRCCOPY);
			}
		}

		//释放内存
		DeleteDC(hdcMem);
		EndPaint(hwnd, &ps);
		layMine(mineRowNum, mineColNum, mineNum);
		countAroundMine(mineRowNum, mineColNum);
		return 0;

	case WM_LBUTTONDOWN:
		
		hdc = GetDC(hwnd);
		hdcMem = CreateCompatibleDC (hdc) ; //内存区域的句柄
		SelectObject (hdcMem, hBitmap) ;
		// x,y为贴图所在的坐标，cx,xy为图片的宽和高， 后面坐标为图片的坐标，
		x = LOWORD(lParam);
		y = HIWORD(lParam);
		//鼠标按下是贴图
		mi = ((x - (cxClient / 2 - (cxSource*mineRowNum) / 2)) / cxSource) ;
		mj = ((y - (cyClient / 2 - (cxSource*mineColNum) / 2)) / cxSource) ;
			if (map[mi][mj] == BOMB)
			{
				
				/*mx = (x - (cxClient / 2 - (cxSource*mineRowNum) / 2)) + cxSource*(i + 1);
				my = (y - (cyClient / 2 - (cxSource*mineColNum) / 2)) + cxSource*(j + 1);*/
				if (x >= (cxClient / 2 - (cxSource*mineRowNum) / 2))
					x = ((x - (cxClient / 2 - (cxSource*mineRowNum) / 2)) / cxSource)*cxSource + (cxClient / 2 - (cxSource*mineRowNum) / 2);
				if (y >= (cyClient / 2 - (cxSource*mineColNum) / 2))
					y = ((y - (cyClient / 2 - (cxSource*mineColNum) / 2)) / cxSource)*cxSource + (cyClient / 2 - (cxSource*mineColNum) / 2);
				if (x >= (cxClient / 2 - (cxSource*mineRowNum) / 2) && x < (cxClient + cxSource*mineColNum) / 2 &&
					y >= (cyClient / 2 - (cxSource*mineColNum) / 2) && y < (cyClient + cxSource*mineColNum) / 2)
				{
					BitBlt(hdc, x, y, cxSource, cySource / 16, hdcMem, 0, cySource * 3 / 16, SRCCOPY);
				}
			}
			else
			{
				if (x >= (cxClient / 2 - (cxSource*mineRowNum) / 2))
					x = ((x - (cxClient / 2 - (cxSource*mineRowNum) / 2)) / cxSource)*cxSource + (cxClient / 2 - (cxSource*mineRowNum) / 2);
				if (y >= (cyClient / 2 - (cxSource*mineColNum) / 2))
					y = ((y - (cyClient / 2 - (cxSource*mineColNum) / 2)) / cxSource)*cxSource + (cyClient / 2 - (cxSource*mineColNum) / 2);
				if (x >= (cxClient / 2 - (cxSource*mineRowNum) / 2) && x < (cxClient + cxSource*mineColNum) / 2 &&
					y >= (cyClient / 2 - (cxSource*mineColNum) / 2) && y < (cyClient + cxSource*mineColNum) / 2)
				{
					if (map[mi][mj]!=BOMB)
					BitBlt(hdc, x, y, cxSource, cySource / 16, hdcMem, 0, cySource * (15-counter[mi][mj]) / 16, SRCCOPY);
					else
					BitBlt(hdc, x, y, cxSource, cySource / 16, hdcMem, 0, cySource * 15 / 16, SRCCOPY);
				}
			}
		//释放内存
		DeleteDC (hdcMem) ;
		EndPaint (hwnd, &ps) ;
		return 0;


	case WM_LBUTTONUP:
		//	PlaySound (TEXT ("HelloWin.wav"), NULL, SND_FILENAME | SND_SYNC) ;
		//	InvalidateRect(hwnd, NULL, FALSE);
		//	SendMessage(hwnd, WM_MYMSG, wParam, lParam);
		return 0;

	case WM_DESTROY:
		DeleteObject(hBitmap);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}
