
#include "D3D11App.h"

namespace
{
	D3D11App* g_D3D11App = 0;
}

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	return g_D3D11App->MsgProc(hWnd, Msg, wParam, lParam);
}

D3D11App::D3D11App(HINSTANCE hInstance): 
	m_hAppInstance(hInstance),
	m_hMainWnd(0),
	m_bAppPaused(false),
	m_bMinimized(false),
	m_bMaximized(false),
	m_bResizing(false),
	m_4xMsaaQuality(0),

	m_D3D11Device(nullptr),
	m_D3D11ImmediateContext(nullptr),
	m_SwapChain(nullptr),
	m_DepthStencilBuffer(nullptr),
	m_RenderTargetView(nullptr),
	m_DepthStencilView(nullptr),

	m_MainWndCaption(L"D3D11 Framework"),
	m_D3D11DriverType(D3D_DRIVER_TYPE_HARDWARE),
	m_ClientWidth(800),
	m_ClientHeight(600),
	m_bEnable4xMsaa(false)
{
	ZeroMemory(&m_ScreenViewport, sizeof(D3D11_VIEWPORT));
	g_D3D11App = this;
}

D3D11App::~D3D11App()
{
	ReleaseCOM(m_RenderTargetView);
	ReleaseCOM(m_DepthStencilView);
	ReleaseCOM(m_SwapChain);
	ReleaseCOM(m_DepthStencilBuffer);

	if (m_D3D11ImmediateContext)
	{
		m_D3D11ImmediateContext->ClearState();
	}

	ReleaseCOM(m_D3D11ImmediateContext);
	CHECK();
	ReleaseCOM(m_D3D11Device);
}

HINSTANCE D3D11App::AppInstance() const
{
	return m_hAppInstance;
}

HWND D3D11App::MainWnd() const
{
	return m_hMainWnd;
}

float D3D11App::AspectRatio() const
{
	return static_cast<float>(m_ClientWidth) / m_ClientHeight;
}

int D3D11App::Run()
{
	MSG Msg = { 0 };

	m_GameTimer.Reset();

	while (Msg.message != WM_QUIT)
	{
		if (PeekMessage(&Msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
		else
		{
			m_GameTimer.Tick();

			if (!m_bAppPaused)
			{
				CalculateFrameStats();
				UpdateScene(m_GameTimer.DeltaTime());
				DrawScene();
			}
			else
			{
				Sleep(100);
			}
		}
	}

	return static_cast<int>(Msg.wParam);
}

bool D3D11App::Init()
{
	if (!InitMainWindow())
	{
		return false;
	}

	if (!InitDirect3D())
	{
		return false;
	}

	return true;
}

void D3D11App::OnResize()
{
	assert(m_D3D11ImmediateContext);
	assert(m_D3D11Device);
	assert(m_SwapChain);

	ReleaseCOM(m_RenderTargetView);
	ReleaseCOM(m_DepthStencilView);
	ReleaseCOM(m_DepthStencilBuffer);

	HR(m_SwapChain->ResizeBuffers(1, m_ClientWidth, m_ClientHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, 0));
	ID3D11Texture2D* BackBuffer;
	HR(m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&BackBuffer)));
	HR(m_D3D11Device->CreateRenderTargetView(BackBuffer, 0, &m_RenderTargetView));
	ReleaseCOM(BackBuffer);

	D3D11_TEXTURE2D_DESC DepthStencilDesc;
	DepthStencilDesc.Width = m_ClientWidth;
	DepthStencilDesc.Height = m_ClientHeight;
	DepthStencilDesc.MipLevels = 1;
	DepthStencilDesc.ArraySize = 1;
	DepthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	if (m_bEnable4xMsaa)
	{
		DepthStencilDesc.SampleDesc.Count = 4;
		DepthStencilDesc.SampleDesc.Quality = m_4xMsaaQuality - 1;
	}
	else
	{
		DepthStencilDesc.SampleDesc.Count = 1;
		DepthStencilDesc.SampleDesc.Quality = 0;
	}
	DepthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	DepthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	DepthStencilDesc.CPUAccessFlags = 0;
	DepthStencilDesc.MiscFlags = 0;

	HR(m_D3D11Device->CreateTexture2D(&DepthStencilDesc, 0, &m_DepthStencilBuffer));
	HR(m_D3D11Device->CreateDepthStencilView(m_DepthStencilBuffer, 0, &m_DepthStencilView));

	m_D3D11ImmediateContext->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView);

	m_ScreenViewport.TopLeftX = 0;
	m_ScreenViewport.TopLeftY = 0;
	m_ScreenViewport.Width = static_cast<float>(m_ClientWidth);
	m_ScreenViewport.Height = static_cast<float>(m_ClientHeight);
	m_ScreenViewport.MinDepth = 0.0f;
	m_ScreenViewport.MaxDepth = 1.0f;

	m_D3D11ImmediateContext->RSSetViewports(1, &m_ScreenViewport);
}

LRESULT D3D11App::MsgProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{

	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			m_bAppPaused = true;
			m_GameTimer.Stop();
		}
		else
		{
			m_bAppPaused = false;
			m_GameTimer.Start();
		}
		return 0;

	case WM_SIZE:
		m_ClientWidth = LOWORD(lParam);
		m_ClientHeight = HIWORD(lParam);
		if (m_D3D11Device)
		{
			if (wParam == SIZE_MINIMIZED)
			{
				m_bAppPaused = true;
				m_bMinimized = true;
				m_bMaximized = false;
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				m_bAppPaused = false;
				m_bMinimized = false;
				m_bMaximized = true;
				OnResize();
			}
			else if (wParam == SIZE_RESTORED)
			{
				if (m_bMinimized)
				{
					m_bAppPaused = false;
					m_bMinimized = false;
					OnResize();
				}
				else if (m_bMaximized)
				{
					m_bAppPaused = false;
					m_bMaximized = false;
					OnResize();
				}
				else if (m_bResizing)
				{

				}
				else 
				{
					OnResize();
				}
			}
		}
		return 0;

	case WM_ENTERSIZEMOVE:
		m_bAppPaused = true;
		m_bResizing = true;
		m_GameTimer.Stop();
		return 0;

	case WM_EXITSIZEMOVE:
		m_bAppPaused = false;
		m_bResizing = false;
		m_GameTimer.Start();
		OnResize();
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_MENUCHAR:
		return MAKELRESULT(0, MNC_CLOSE);

	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return 0;

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;

	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:

		OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;

	case WM_MOUSEMOVE:
		OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;

	}

	return DefWindowProc(hWnd, Msg, wParam, lParam);
}

void D3D11App::OnMouseDown(WPARAM BtnState, int x, int y)
{
}

void D3D11App::OnMouseUp(WPARAM BtnState, int x, int y)
{
}

void D3D11App::OnMouseMove(WPARAM BtnState, int x, int y)
{
}

bool D3D11App::InitMainWindow()
{
	WNDCLASS WndCls;
	WndCls.style = CS_HREDRAW | CS_VREDRAW;
	WndCls.lpfnWndProc = MainWndProc;
	WndCls.cbClsExtra = 0;
	WndCls.cbWndExtra = 0;
	WndCls.hInstance = m_hAppInstance;
	WndCls.hIcon = LoadIcon(0, IDI_APPLICATION);
	WndCls.hCursor = LoadCursor(0, IDC_ARROW);
	WndCls.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	WndCls.lpszMenuName = 0;
	WndCls.lpszClassName = L"D3DWndClassName";

	if (!RegisterClass(&WndCls))
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	RECT R = { 0, 0, m_ClientWidth, m_ClientHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int Width = R.right - R.left;
	int Height = R.bottom - R.top;

	m_hMainWnd = CreateWindow(L"D3DWndClassName", m_MainWndCaption.c_str(),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, Width, Height, 0, 0, m_hAppInstance, 0);
	
	if (!m_hMainWnd)
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	ShowWindow(m_hMainWnd, SW_SHOW);
	UpdateWindow(m_hMainWnd);

	return true;
}

bool D3D11App::InitDirect3D()
{

	UINT CreateDeviceFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)  
	CreateDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL FeatureLevel;
	HRESULT hr = D3D11CreateDevice(
		0,
		m_D3D11DriverType,
		0,
		CreateDeviceFlags,
		0, 0,
		D3D11_SDK_VERSION,
		&m_D3D11Device,
		&FeatureLevel,
		&m_D3D11ImmediateContext);

	if (FAILED(hr))
	{
		MessageBox(0, L"D3D11CreateDevice Failed.", 0, 0);
		return false;
	}

	if (FeatureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		MessageBox(0, L"Direct3D Feature Level 11 unsupported.", 0, 0);
		return false;
	}

	HR(m_D3D11Device->CheckMultisampleQualityLevels(
		DXGI_FORMAT_R16G16B16A16_FLOAT, 4, &m_4xMsaaQuality));
	assert(m_4xMsaaQuality > 0);

	DXGI_SWAP_CHAIN_DESC SwapChainDesc;
	SwapChainDesc.BufferDesc.Width = m_ClientWidth;
	SwapChainDesc.BufferDesc.Height = m_ClientHeight;
	SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	if (m_bEnable4xMsaa)
	{
		SwapChainDesc.SampleDesc.Count = 4;
		SwapChainDesc.SampleDesc.Quality = m_4xMsaaQuality - 1;
	}
	else
	{
		SwapChainDesc.SampleDesc.Count = 1;
		SwapChainDesc.SampleDesc.Quality = 0;
	}
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDesc.BufferCount = 1;
	SwapChainDesc.OutputWindow = m_hMainWnd;
	SwapChainDesc.Windowed = true;
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	SwapChainDesc.Flags = 0;

	IDXGIDevice* DxgiDevice = 0;
	HR(m_D3D11Device->QueryInterface(__uuidof(IDXGIDevice), (void**)&DxgiDevice));

	IDXGIAdapter* DxgiAdapter = 0;
	HR(DxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&DxgiAdapter));

	IDXGIFactory* DxgiFactory = 0;
	HR(DxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&DxgiFactory));

	HR(DxgiFactory->CreateSwapChain(m_D3D11Device, &SwapChainDesc, &m_SwapChain));

	ReleaseCOM(DxgiDevice);
	ReleaseCOM(DxgiAdapter);
	ReleaseCOM(DxgiFactory);

	OnResize();

	return true;
}

void D3D11App::CalculateFrameStats()
{
	static int FrameCount = 0;
	static float TimeElapsed = 0.0f;

	FrameCount++;

	if ((m_GameTimer.TotalTime() - TimeElapsed) >= 1.0f)
	{
		float fps = static_cast<float>(FrameCount);
		float mspf = 1000.0f / fps;

		std::wostringstream outs;
		outs.precision(6);
		outs << m_MainWndCaption << L"    "
			<< L"FPS: " << fps << L"    "
			<< L"Frame Time: " << mspf << L" (ms)";
		SetWindowText(m_hMainWnd, outs.str().c_str());

		FrameCount = 0;
		TimeElapsed += 1.0f;
	}
}
