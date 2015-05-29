#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include "QR_Encode.h"
#include "resource.h"

TCHAR szClassName[] = TEXT("window");

HBITMAP hBmp=NULL;
int nSymbleSize=0;

HWND hEdit;

void DrawQR(LPCSTR lpsSource)
{
	CQR_Encode*	pQR_Encode=NULL;
	pQR_Encode=new CQR_Encode;
	if(pQR_Encode)
	{
		if(pQR_Encode->EncodeData(1,0,TRUE,-1,lpsSource))
		{
			HDC hMemWork = CreateCompatibleDC( NULL );
			if( hMemWork )
			{
				if(hBmp==NULL || DeleteObject(hBmp))
				{
					hBmp=CreateBitmap( pQR_Encode->m_nSymbleSize, pQR_Encode->m_nSymbleSize, 1, 1, NULL );
					if( hBmp )
					{
						HBITMAP	hOldBitmap = (HBITMAP)SelectObject( hMemWork, hBmp );
						nSymbleSize = pQR_Encode->m_nSymbleSize;
						PatBlt( hMemWork, 0, 0, nSymbleSize, nSymbleSize, WHITENESS );
						int	i;
						for(i=0;i<nSymbleSize;++i)
						{
							int j;
							for(j=0;j<nSymbleSize;++j)
							{
								if( pQR_Encode->m_byModuleData[i][j] )
								{
									SetPixel( hMemWork, i + QR_MARGIN, j + QR_MARGIN, RGB(0, 0, 0) );
								}
							}
						}
						SelectObject( hMemWork, hOldBitmap );
						DeleteObject( hOldBitmap );
						DeleteDC( hMemWork );
					}
				}
			}
		}
	}
	if(pQR_Encode)
	{
		delete pQR_Encode;
	}
}

LRESULT CALLBACK WndProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rect;
	static BOOL bIsEmpty = TRUE;
	static HFONT hFont;
	switch (msg)
	{
		case WM_COMMAND:
			if(HIWORD( wParam ) == EN_CHANGE)
			{
				CHAR szText[1024];
				if(GetWindowTextLength(hEdit)==0)
				{
					bIsEmpty=TRUE;
				}
				else
				{
					bIsEmpty=FALSE;
					GetWindowTextA(hEdit,szText,1024);
					DrawQR(szText);
				}
				InvalidateRect(hWnd,NULL,bIsEmpty);
			}
			break;
	case WM_SETFOCUS:
		SetFocus(hEdit);
		SendMessage(hEdit,EM_SETSEL,0,-1);
		break;
	case WM_SIZE:
		MoveWindow(hEdit,5,5,LOWORD(lParam)-HIWORD(lParam)-5,HIWORD(lParam)-10,TRUE);
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			const HDC hdc = BeginPaint(hWnd, &ps);
			const HDC hdc_mem = CreateCompatibleDC(hdc);
			SelectObject(hdc_mem, hBmp);
			GetClientRect(hWnd, &rect);
			if (!bIsEmpty)StretchBlt(hdc, rect.right - rect.bottom + 5, 5, rect.bottom - 10, rect.bottom - 10, hdc_mem, 0, 0, nSymbleSize, nSymbleSize, SRCCOPY);
			DeleteDC(hdc_mem);
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_CREATE:
		hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), 0, WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL, 0, 0, 0, 0, hWnd, NULL, ((LPCREATESTRUCT)(lParam))->hInstance, NULL);
		break;
	case WM_DESTROY:
		DeleteObject(hBmp);
		DeleteObject(hFont);
		PostQuitMessage(0);
		break;
	default:
		return(DefWindowProc(hWnd,msg,wParam,lParam));
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hinst,HINSTANCE hPreInst,
				   LPSTR pCmdLine,int nCmdShow)
{
	HWND hWnd;
	MSG msg;
	WNDCLASS wndclass;
	if(!hPreInst)
	{
		wndclass.style=CS_HREDRAW|CS_VREDRAW;
		wndclass.lpfnWndProc=WndProc;
		wndclass.cbClsExtra=0;
		wndclass.cbWndExtra=0;
		wndclass.hInstance =hinst;
		wndclass.hIcon=NULL;
		wndclass.hCursor=LoadCursor(NULL,IDC_ARROW);
		wndclass.hbrBackground=(HBRUSH)(COLOR_3DFACE+1);
		wndclass.lpszMenuName=NULL;
		wndclass.lpszClassName=szClassName;
		if(!RegisterClass(&wndclass))
			return FALSE;
	}
	hWnd=CreateWindow(szClassName,
		TEXT("QR CODE"),
		WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		hinst,
		NULL);
	ShowWindow(hWnd,nCmdShow);
	UpdateWindow(hWnd);
    while (GetMessage(&msg,NULL,0,0))
	{
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}
