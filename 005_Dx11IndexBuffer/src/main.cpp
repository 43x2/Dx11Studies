// Windows
#include <windows.h>
#include <tchar.h>
// DirectX
#include <d3d11.h>
// Others...
#include <dtprintf.h>
// Shaders
#include "constant.vs.h"   // 頂点シェーダ
#include "constant.ps.h"   // ピクセルシェーダ


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
			1280,									// BufferDesc.Width
			720,									// BufferDesc.Height
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
		1280.0f,		// Width
		 720.0f,		// Height
		   0.0f,		// MinDepth
		   1.0f			// MaxDepth
	};
	pDeviceContext->RSSetViewports( 1, &viewport );
	dtprintf( _T( "ID3D11DeviceContext::RSSetViewports: ok\n" ) );


	// 頂点データ
	float vertices[ 5 ][ 7 ] = {
	//    Xaxis  Yaxis  Zaxis  赤     緑     青     Alpha
		{  0.0f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,  1.0f },   // 原点
		{ -0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f },   // 左上
		{  0.5f,  0.5f,  0.5f,  1.0f,  1.0f,  0.0f,  1.0f },   // 右上
		{  0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f },   // 右下
		{ -0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  1.0f,  1.0f }    // 左下
	};
	// 入力エレメント記述子
	D3D11_INPUT_ELEMENT_DESC verticesDesc[] = {
		{ "IN_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,               D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "IN_COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, sizeof(float)*3, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// 頂点バッファを生成
	D3D11_BUFFER_DESC vertexBufferDesc = {
		5 * sizeof( float ) * 7,	// ByteWidth
		D3D11_USAGE_DEFAULT,		// Usage
		D3D11_BIND_VERTEX_BUFFER,	// BindFlags
		0,							// CPUAccessFlags
		0,							// MiscFlags
		0							// StructureByteStride
	};
	D3D11_SUBRESOURCE_DATA vertexResourceData = { vertices };

	ID3D11Buffer * pVertexBuffer = NULL;
	hr = pDevice->CreateBuffer( &vertexBufferDesc, &vertexResourceData, &pVertexBuffer );
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, _T( "失敗: ID3D11Device::CreateBuffer()" ), _T( "エラー" ), MB_OK | MB_ICONERROR );
		return 0;
	}
	dtprintf( _T( "ID3D11Device::CreateBuffer: ok (pVertexBuffer: 0x%p)\n" ), pVertexBuffer );

	// 頂点バッファをバインド
	UINT strides[] = { sizeof( float ) * 7 };
	UINT offsets[] = { 0 };
	pDeviceContext->IASetVertexBuffers( 0, 1, &pVertexBuffer, strides, offsets );
	dtprintf( _T( "ID3D11DeviceContext::IASetVertexBuffers: ok\n" ) );


	// インデックスデータ
	unsigned int indices[] = { 0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 1 };

	// インデックスバッファを生成
	D3D11_BUFFER_DESC indexBufferDesc = {
		sizeof( unsigned int ) * 12,	// ByteWidth
		D3D11_USAGE_DEFAULT,			// Usage
		D3D11_BIND_INDEX_BUFFER,		// BindFlags
		0,								// CPUAccessFlags
		0,								// MiscFlags
		0								// StructureByteStride
	};
	D3D11_SUBRESOURCE_DATA indexResourceData = { indices };

	ID3D11Buffer * pIndexBuffer = NULL;
	hr = pDevice->CreateBuffer( &indexBufferDesc, &indexResourceData, &pIndexBuffer );
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, _T( "失敗: ID3D11Device::CreateBuffer()" ), _T( "エラー" ), MB_OK | MB_ICONERROR );
		return 0;
	}
	dtprintf( _T( "ID3D11Device::CreateBuffer: ok (pIndexBuffer: 0x%p)\n" ), pIndexBuffer );

	// インデックスバッファをバインド
	pDeviceContext->IASetIndexBuffer( pIndexBuffer, DXGI_FORMAT_R32_UINT, 0 );
	dtprintf( _T( "ID3D11DeviceContext::IASetIndexBuffer: ok\n" ) );


	// プリミティブタイプを設定
	pDeviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	dtprintf( _T( "ID3D11DeviceContext::IASetPrimitiveTopology: ok\n" ) );


	// 頂点シェーダを作成
	ID3D11VertexShader * pVertexShader = NULL;
	hr = pDevice->CreateVertexShader( g_vs_constant, sizeof( g_vs_constant ), NULL, &pVertexShader );
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, _T( "失敗: ID3D11Device::CreateVertexShader()" ), _T( "エラー" ), MB_OK | MB_ICONERROR );
		return 0;
	}
	dtprintf( _T( "ID3D11Device::CreateVertexShader: ok (pVertexShader: 0x%p)\n" ), pVertexShader );


	// ピクセルシェーダを作成
	ID3D11PixelShader * pPixelShader = NULL;
	hr = pDevice->CreatePixelShader( g_ps_constant, sizeof( g_ps_constant ), NULL, &pPixelShader );
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, _T( "失敗: ID3D11Device::CreatePixelShader()" ), _T( "エラー" ), MB_OK | MB_ICONERROR );
		return 0;
	}
	dtprintf( _T( "ID3D11Device::CreatePixelShader: ok (pPixelShader: 0x%p)\n" ), pPixelShader );


	// シェーダをバインド
	pDeviceContext->VSSetShader( pVertexShader, NULL, 0 );
	dtprintf( _T( "ID3D11DeviceContext::VSSetShader: ok\n" ) );
	pDeviceContext->PSSetShader( pPixelShader, NULL, 0 );
	dtprintf( _T( "ID3D11DeviceContext::PSSetShader: ok\n" ) );
	pDeviceContext->GSSetShader( NULL, NULL, 0 );
	pDeviceContext->HSSetShader( NULL, NULL, 0 );
	pDeviceContext->DSSetShader( NULL, NULL, 0 );


	// 入力レイアウトを生成
	ID3D11InputLayout * pInputLayout = NULL;
	hr = pDevice->CreateInputLayout( verticesDesc, 2, g_vs_constant, sizeof( g_vs_constant ), &pInputLayout );
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, _T( "失敗: ID3D11Device::CreateInputLayout()" ), _T( "エラー" ), MB_OK | MB_ICONERROR );
		return 0;
	}
	dtprintf( _T( "ID3D11Device::CreateInputLayout: ok (pInputLayout: 0x%p)\n" ), pInputLayout );


	// 入力レイアウトをバインド
	pDeviceContext->IASetInputLayout( pInputLayout );
	dtprintf( _T( "ID3D11DeviceContext::IASetInputLayout: ok\n" ) );



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
			// レンダーターゲットをクリア
			const float clear[ 4 ] = { 0.0f, 0.0f, 1.0f, 1.0f };	// RGBA
			pDeviceContext->ClearRenderTargetView( pRenderTargetView, clear );

			// 描画
			pDeviceContext->DrawIndexed( 12, 0, 0 );

			pSwapChain->Present( 1, 0 );

			// ちょっとだけ待つ
			Sleep( 5 );
		}
	}



	// シェーダをアンバインド
	pDeviceContext->VSSetShader( NULL, NULL, 0 );
	pDeviceContext->PSSetShader( NULL, NULL, 0 );

	// デバイス・リソース解放
	COM_SAFE_RELEASE( pInputLayout );
	COM_SAFE_RELEASE( pPixelShader );
	COM_SAFE_RELEASE( pVertexShader );
	COM_SAFE_RELEASE( pIndexBuffer );
	COM_SAFE_RELEASE( pVertexBuffer );
	COM_SAFE_RELEASE( pRenderTargetView );
	COM_SAFE_RELEASE( pSwapChain );
	COM_SAFE_RELEASE( pDeviceContext );
	COM_SAFE_RELEASE( pDevice );


	return msg.wParam;
}
