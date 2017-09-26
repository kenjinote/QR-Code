#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <windows.h>
#include "QR_Encode.h"

VOID DrawQR(LPCSTR lpszSourceText, HBITMAP *phBitmap, int *pnSymbleSize)
{
	CQR_Encode*	pQR_Encode = new CQR_Encode;
	if (pQR_Encode)
	{
		if (pQR_Encode->EncodeData(1, 0, TRUE, -1, lpszSourceText))
		{
			HDC hMemDC = CreateCompatibleDC(NULL);
			if (hMemDC)
			{
				if (*phBitmap == NULL || DeleteObject(*phBitmap))
				{
					*phBitmap = CreateBitmap(pQR_Encode->m_nSymbleSize, pQR_Encode->m_nSymbleSize, 1, 1, NULL);
					if (*phBitmap)
					{
						HBITMAP	hOldBitmap = (HBITMAP)SelectObject(hMemDC, *phBitmap);
						*pnSymbleSize = pQR_Encode->m_nSymbleSize;
						PatBlt(hMemDC, 0, 0, *pnSymbleSize, *pnSymbleSize, WHITENESS);
						for (int i = 0; i < *pnSymbleSize; ++i)
						{
							for (int j = 0; j < *pnSymbleSize; ++j)
							{
								if (pQR_Encode->m_byModuleData[i][j])
								{
									SetPixel(hMemDC, i + QR_MARGIN, j + QR_MARGIN, RGB(0, 0, 0));
								}
							}
						}
						SelectObject(hMemDC, hOldBitmap);
					}
				}
				DeleteDC(hMemDC);
			}
		}
		delete pQR_Encode;
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static BOOL bIsEmpty = TRUE;
	static HFONT hFont;
	static HWND hEdit;
	static HBITMAP hBitmap;
	static int nSymbleSize;
	switch (msg)
	{
	case WM_CREATE:
		hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), 0, WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL, 0, 0, 0, 0, hWnd, NULL, ((LPCREATESTRUCT)(lParam))->hInstance, NULL);
		SendMessage(hEdit, EM_LIMITTEXT, 0, 0);
		break;
	case WM_COMMAND:
		if (HIWORD(wParam) == EN_CHANGE)
		{
			const int nTextLength = GetWindowTextLengthA(hEdit);
			if (nTextLength == 0)
			{
				bIsEmpty = TRUE;
			}
			else
			{
				bIsEmpty = FALSE;
				LPSTR lpszText = (LPSTR)GlobalAlloc(GMEM_FIXED, nTextLength + 1);
				GetWindowTextA(hEdit, lpszText, nTextLength + 1);
				DrawQR(lpszText, &hBitmap, &nSymbleSize);
				GlobalFree(lpszText);
			}
			InvalidateRect(hWnd, NULL, bIsEmpty);
		}
		break;
	case WM_SETFOCUS:
		SetFocus(hEdit);
		SendMessage(hEdit, EM_SETSEL, 0, -1);
		break;
	case WM_SIZE:
		MoveWindow(hEdit, 5, 5, LOWORD(lParam) - HIWORD(lParam) - 5, HIWORD(lParam) - 10, TRUE);
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			const HDC hdc = BeginPaint(hWnd, &ps);
			const HDC hMemDC = CreateCompatibleDC(hdc);
			const HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
			RECT rect;
			GetClientRect(hWnd, &rect);
			if (!bIsEmpty)
				StretchBlt(hdc, rect.right - rect.bottom + 5, 5, rect.bottom - 10, rect.bottom - 10, hMemDC, 0, 0, nSymbleSize, nSymbleSize, SRCCOPY);
			SelectObject(hMemDC, hOldBitmap);
			DeleteDC(hMemDC);
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_DESTROY:
		DeleteObject(hBitmap);
		DeleteObject(hFont);
		PostQuitMessage(0);
		break;
	default:
		return(DefWindowProc(hWnd, msg, wParam, lParam));
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hinst, HINSTANCE hPreInst, LPSTR pCmdLine, int nCmdShow)
{
	TCHAR szClassName[] = TEXT("QRCODE");
	WNDCLASS wndclass = { 0 };
	if (!hPreInst)
	{
		wndclass.style = CS_HREDRAW | CS_VREDRAW;
		wndclass.lpfnWndProc = WndProc;
		wndclass.hInstance = hinst;
		wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
		wndclass.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);
		wndclass.lpszClassName = szClassName;
		if (!RegisterClass(&wndclass))
		{
			return 0;
		}
	}
	const DWORD dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN;
	RECT rect;
	SetRect(&rect, 0, 0, 800, 400);
	AdjustWindowRect(&rect, dwStyle, 0);
	HWND hWnd = CreateWindow(szClassName,
		TEXT("QR CODE"),
		dwStyle,
		CW_USEDEFAULT,
		0,
		rect.right - rect.left,
		rect.bottom - rect.top,
		0,
		0,
		hinst,
		0);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}
