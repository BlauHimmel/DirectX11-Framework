#pragma once

#ifndef __D3D11_UTILS_H__
#define __D3D11_UTILS_H__

#if defined(DEBUG) || defined(_DEBUG)
	#define _CRTDBG_MAP_ALLOC
	#include <crtdbg.h>
#endif

#include <Windows.h>
#include <xnamath.h>
#include <dxerr.h>
#include <cassert>
#include <fstream>
#include <sstream>

#pragma comment(lib, "legacy_stdio_definitions.lib")

#if defined(DEBUG) | defined(_DEBUG)
	#ifndef HR
		#define HR(x)\
		{\
			HRESULT hr = (x);\
			if(FAILED(hr))\
			{\
				DXTrace(__FILE__, (DWORD)__LINE__, hr, L#x, true); \
			}\
		}
	#endif
	#else
	#ifndef HR
		#define HR(x) (x)
	#endif
#endif 

#if defined(DEBUG) || defined(_DEBUG)  
	#ifndef CHECK
		#define CHECK()\
		ID3D11Debug* D3DDebug;\
		HRESULT hr = m_D3D11Device->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&D3DDebug));\
		if (SUCCEEDED(hr))\
		{\
			hr = D3DDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);\
		}\
		if (D3DDebug != nullptr)\
		{\
			D3DDebug->Release();\
		}
	#endif
	#else
	#ifndef CHECK
		#define CHECK()
	#endif
#endif 

#define ReleaseCOM(x) { if(x){ (x)->Release(); (x) = nullptr; } }

#define SafeDelete(x) { if (x) { delete (x); (x) = nullptr; } }

#define SafeDeleteArray(x) { if (x) { delete[] (x); (x) = nullptr; } }

namespace Colors
{
	XMGLOBALCONST XMVECTORF32 White				= { 1.0f,	1.0f,	1.0f,	1.0f };
	XMGLOBALCONST XMVECTORF32 Black				= { 0.0f,	0.0f,	0.0f,	1.0f };
	XMGLOBALCONST XMVECTORF32 Red				= { 1.0f,	0.0f,	0.0f,	1.0f };
	XMGLOBALCONST XMVECTORF32 Green				= { 0.0f,	1.0f,	0.0f,	1.0f };
	XMGLOBALCONST XMVECTORF32 Blue				= { 0.0f,	0.0f,	1.0f,	1.0f };
	XMGLOBALCONST XMVECTORF32 Yellow			= { 1.0f,	1.0f,	0.0f,	1.0f };
	XMGLOBALCONST XMVECTORF32 Cyan				= { 0.0f,	1.0f,	1.0f,	1.0f };
	XMGLOBALCONST XMVECTORF32 Magenta			= { 1.0f,	0.0f,	1.0f,	1.0f };
	XMGLOBALCONST XMVECTORF32 Silver			= { 0.75f,	0.75f,	0.75f,	1.0f };
	XMGLOBALCONST XMVECTORF32 LightSteelBlue    = { 0.69f,	0.77f,	0.87f,	1.0f };
}

#endif