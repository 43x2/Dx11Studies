#include <windows.h>
#include <tchar.h>

#include <dtprintf.h>


TCHAR *g_className  = _T( "すみすみは俺の嫁" );
TCHAR *g_windowName = g_className;


LRESULT CALLBACK WndProc( HWND hWnd, UINT mes, WPARAM wParam, LPARAM lParam )
{
	if ( mes == WM_DESTROY )
	{
		PostQuitMessage( 0 );
		return 0;
	}

	return DefWindowProc( hWnd, mes, wParam, lParam );
}



int APIENTRY _tWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
{

	// ウィンドウクラスを登録
	WNDCLASSEX wcex = {
		sizeof( WNDCLASSEX ),			// cbSize
		CS_HREDRAW | CS_VREDRAW,		// style
		WndProc,						// lpfnWndProc
		0,								// cbClsExtra
		0,								// cbWndExtra
		hInstance,						// hInstance
		NULL,							// hIcon
		NULL,							// hCursor
		( HBRUSH )( COLOR_WINDOW + 1 ),	// hbrBackGround
		NULL,							// lpszMenuName
		g_className,					// lpszClassName
		NULL							// hIconSm
	};
	if ( ! RegisterClassEx( &wcex ) )
	{
		MessageBox( NULL, _T( "失敗: RegisterClassEx()" ), _T( "エラー" ), MB_OK | MB_ICONERROR );
		return 0;
	}
	dtprintf( _T( "RegisterClassEx: ok\n" ) );


	// ウィンドウサイズを計算
	RECT r = { 0, 0, 800, 450 };   // 800x450 (16:9)
	if ( ! AdjustWindowRect( &r, WS_OVERLAPPEDWINDOW, FALSE ) )
	{
		MessageBox( NULL, _T( "失敗: AdjustWindowRect()" ), _T( "エラー" ),  MB_OK | MB_ICONERROR );
		return 0;
	}
	dtprintf( _T( "AdjustWindowRect: ok (%d, %d)－(%d, %d)\n" ), r.left, r.top, r.right, r.bottom );


	// ウィンドウ生成
	HWND hWnd;
	if ( ( hWnd = CreateWindow( g_className,
	                            g_windowName,
	                            WS_OVERLAPPEDWINDOW,
	                            CW_USEDEFAULT,
	                            0,
	                            r.right - r.left,
	                            r.bottom - r.top,
	                            NULL,
	                            NULL,
	                            hInstance,
	                            NULL ) ) == NULL )
	{
		MessageBox( NULL, _T( "失敗: CreateWindow()" ), _T( "エラー" ), MB_OK | MB_ICONERROR );
		return 0;
	}
	dtprintf( _T( "CreateWindow: ok\n" ) );


	// ウィンドウ表示
	ShowWindow(hWnd, nCmdShow);
	dtprintf( _T( "ShowWindow: ok\n" ) );


	MSG msg;
	while ( 1 )
	{
		// メッセージを取得
		if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			if ( msg.message == WM_QUIT )
			{
				dtprintf( _T( "PeekMessage: WM_QUIT\n" ) );
				break;
			}
			// メッセージ処理
			DispatchMessage( &msg );
		}
		else
		{
			Sleep( 5 );
		}
	}
	return msg.wParam;
}
