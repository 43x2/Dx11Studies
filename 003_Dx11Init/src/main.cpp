// Windows
#include <windows.h>
#include <tchar.h>
// DirectX
#include <d3d11.h>
// Others...
#include <dtprintf.h>


#define COM_SAFE_RELEASE( p ) { if(p) { (p)->Release(); (p) = NULL; } }


TCHAR *g_className  = _T( "すみすみは俺の嫁" );
TCHAR *g_windowName = g_className;



// ウィンドウプロシージャ
LRESULT CALLBACK WndProc( HWND hWnd, UINT mes, WPARAM wParam, LPARAM lParam )
{
	switch ( mes )
	{
	case WM_DESTROY:
		PostQuitMessage( 0 );
		return 0;
	}
	return DefWindowProc( hWnd, mes, wParam, lParam );
}



// WinMain
int APIENTRY _tWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
{
	HRESULT hr;


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
	hWnd = CreateWindow( g_className,
	                     g_windowName,
	                     WS_OVERLAPPEDWINDOW,
	                     CW_USEDEFAULT,
	                     0,
	                     r.right - r.left,
	                     r.bottom - r.top,
	                     NULL,
	                     NULL,
	                     hInstance,
	                     NULL );
	if ( hWnd == NULL )
	{
		MessageBox( NULL, _T( "失敗: CreateWindow()" ), _T( "エラー" ), MB_OK | MB_ICONERROR );
		return 0;
	}
	dtprintf( _T( "CreateWindow: ok\n" ) );


	// ウィンドウ表示
	ShowWindow(hWnd, nCmdShow);
	dtprintf( _T( "ShowWindow: ok\n" ) );


	// スワップチェイン設定
	DXGI_SWAP_CHAIN_DESC scDesc = {
		{
			640,									// BufferDesc.Width
			360,									// BufferDesc.Height
			{
				60,									// BufferDesc.RefreshRate.Numerator
				1									// BufferDesc.RefreshRate.Denominator
			},
			DXGI_FORMAT_R16G16B16A16_FLOAT,			// BufferDesc.Format
			DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,	// BufferDesc.ScanlineOrdering
			DXGI_MODE_SCALING_CENTERED				// BufferDesc.Scaling
		},
		{
			1,										// SampleDesc.Count
			0										// SampleDesc.Quality
		},
		DXGI_USAGE_RENDER_TARGET_OUTPUT,			// BufferUsage
		1,											// BufferCount
		hWnd,										// OutputWindow
		TRUE,										// Windowed
		DXGI_SWAP_EFFECT_DISCARD,					// SwapEffect
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH		// Flags
	};

	// Direct3D11 デバイス・デバイスコンテキスト・スワップチェーン生成
	ID3D11Device        * pDevice        = NULL;
	ID3D11DeviceContext * pDeviceContext = NULL;
	IDXGISwapChain      * pSwapChain     = NULL;
	D3D_FEATURE_LEVEL     feature;
	hr = D3D11CreateDeviceAndSwapChain( NULL,
	                                    D3D_DRIVER_TYPE_HARDWARE,
	                                    NULL,
	                                    0,
	                                    NULL,
	                                    0,
	                                    D3D11_SDK_VERSION,
	                                    &scDesc,
	                                    &pSwapChain,
	                                    &pDevice,
	                                    &feature,
	                                    &pDeviceContext );
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, _T( "失敗: D3D11CreateDeviceAndSwapChain()" ), _T( "エラー" ), MB_OK | MB_ICONERROR );
		return 0;
	}
	dtprintf( _T( "D3D11CreateDeviceAndSwapChain: ok (pDevice: 0x%p, pDeviceContext: 0x%p, pSwapChain: 0x%p, feature: 0x%4x)\n" ),
	          pDevice,
	          pDeviceContext,
	          pSwapChain,
	          ( int ) feature );


	// バックバッファテクスチャ取得
	ID3D11Texture2D * pBackBuffer = NULL;
	hr = pSwapChain->GetBuffer( 0, __uuidof( pBackBuffer ), reinterpret_cast< void ** >( &pBackBuffer ) );
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, _T( "失敗: IDXGISwapChain::GetBuffer()" ), _T( "エラー" ), MB_OK | MB_ICONERROR );
		return 0;
	}
	dtprintf( _T( "IDXGISwapChain::GetBuffer: ok (pBackBuffer: 0x%p)\n" ), pBackBuffer );


	// レンダーターゲットビュー生成
	ID3D11RenderTargetView * pRenderTargetView = NULL;
	hr = pDevice->CreateRenderTargetView( pBackBuffer, NULL, &pRenderTargetView );
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, _T( "失敗: ID3D11Device::CreateRenderTargetView()" ), _T( "エラー" ), MB_OK | MB_ICONERROR );
		return 0;
	}
	dtprintf( _T( "ID3D11Device::CreateRenderTargetView: ok (pRenderTargetView: 0x%p)\n" ), pRenderTargetView );


	// レンダーターゲットビューをバインド
	pDeviceContext->OMSetRenderTargets( 1, &pRenderTargetView, NULL );
	dtprintf( _T( "ID3D11DeviceContext::OMSetRenderTargets: ok\n" ) );


	// バックバッファはもうここでは使わない
	COM_SAFE_RELEASE( pBackBuffer );


	// ビューポートをバインド
	D3D11_VIEWPORT viewport = {
		  0.0f,		// TopLeftX
		  0.0f,		// TopLeftY
		640.0f,		// Width
		360.0f,		// Height
		  0.0f,		// MinDepth
		  1.0f		// MaxDepth
	};
	pDeviceContext->RSSetViewports( 1, &viewport );
	dtprintf( _T( "ID3D11DeviceContext::RSSetViewports: ok\n" ) );



	MSG msg;

	while ( 1 )
	{
		// メッセージを取得
		if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			if ( msg.message == WM_QUIT )
			{
				dtprintf( _T( "PeekMessage: WM_QUIT\n" ) );
				return msg.wParam;
			}
			// メッセージ処理
			DispatchMessage( &msg );
		}
		else
		{
			// レンダーターゲットをクリア
			const float clear[ 4 ] = { 0.0f, 0.0f, 1.0f, 1.0f };	// RGBA
			pDeviceContext->ClearRenderTargetView( pRenderTargetView, clear );
			pSwapChain->Present( 1, 0 );

			// ちょっとだけ待つ
			Sleep( 5 );
		}
	}



	// デバイス類解放
	COM_SAFE_RELEASE( pRenderTargetView );
	COM_SAFE_RELEASE( pSwapChain );
	COM_SAFE_RELEASE( pDeviceContext );
	COM_SAFE_RELEASE( pDevice );


	return msg.wParam;
}
