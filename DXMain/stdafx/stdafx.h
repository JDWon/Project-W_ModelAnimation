// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#pragma warning(disable:4100)
#pragma warning(disable:4324)

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#include <windows.h>
// Windows Resource 파일:
#include "Resource\Resource.h"


// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <assert.h>

// D3D11
#include <d3d11_2.h>
#include <dxgi1_3.h>

// D2D1
#include <d2d1_3.h>
#include <dwrite_3.h>
#include <d2d1_2helper.h>
#include <wincodec.h>

// DirectX Math
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>

// C++11
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <algorithm>
#include <functional>
#include <memory>
#include <vector>
#include <list>
#include <deque>
#include <map>
#include <set>

// U W P
#include <wrl.h>
#include <wrl/client.h>
#include <wrl/internal.h>


// Texture Loader : in Utils
#include "DirectX\TextureLoader\WICTextureLoader.h"
#include "DirectX\TextureLoader\DDSTextureLoader.h"

#include <fbxsdk.h>

using namespace std;
using namespace std::chrono;
using namespace D2D1;
using namespace DirectX;
using namespace DirectX::PackedVector;

using Microsoft::WRL::ComPtr;

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#include "header\D2D_Operator.h"
#include "header\LusWrite.h"
#include "header\MeasureFunctionTimeElapsed.h"
#include "header\scroll.h"
#include "header\IO.h"

// TODO: 프로그램 전반에 필요한 함수는 여기에서 참조합니다.
template<typename Ty, size_t N>
inline constexpr size_t GetArraySize(Ty(&)[N]) noexcept { return N; }

/// <summary>
/// 현재 윈도우 프로시저와 연결된 클래스 인스턴스를 가져옵니다.
/// </summary>
///	<param name = "hWnd"> 윈도우 핸들입니다. 해당 윈도우 핸들과 연결된 클래스 인스턴스를 반환합니다. </param>
/// <returns> 연결된 클래스 인스턴스 포인터를 반환합니다. </returns>
template <typename Frmwk>
inline Frmwk GetUserDataPtr(HWND hWnd) noexcept
{
	return reinterpret_cast<Frmwk>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
}

/// <summary>
/// 안전하게 COM 객체를 1회 해제합니다. 해제한 뒤 객체를 가지고 있던 포인터를 nullptr로 초기화합니다.
/// </summary>
///	<param name = "hWnd"> 윈도우 핸들입니다. 이 핸들 값으로 데이터를 찾을 수 있게 됩니다. </param>
///	<param name = "ptr"> 윈도우 핸들과 연결될 데이터입니다. 대체로 클래스 인스턴스의 포인터를 사용합니다. </param>
/// <returns> LONG_PTR 형식으로 결과값을 반환합니다. </returns>
inline auto SetUserDataPtr(HWND hWnd, LPVOID ptr) noexcept
{
	return ::SetWindowLongPtr(
		hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(ptr));

}

/// <summary>
/// 안전하게 COM 객체를 1회 해제합니다. 해제한 뒤 객체를 가지고 있던 포인터를 nullptr로 초기화합니다.
/// </summary>
///	<param name = "obj"> 해제 후 초기화할 COM 객체입니다. 오로지 COM 객체여야만 합니다. </param>
/// <remarks> obj는 반드지 COM 객체여야만 합니다. </remarks>
/// <returns> 반환값은 존재하지 않습니다. </returns>
template<typename Ty>
inline void SafeRelease(Ty& obj) { if (obj != nullptr) { obj->Release(); obj = nullptr; } }

/// <summary>
/// HRESULT가 S_FALSE 일 경우 입력된 메시지를 입력한 메시지 박스를 출력하며 true를 반환합니다.
/// </summary>
///	<param name = "hr"> 평가할 HRSULT 객체입니다. S_FALSE이면 메시지 박스를 출력합니다. </param>
///	<param name = "msg"> 메시지입니다. 생략 가능합니다. </param>
///	<param name = "captionMsg"> 캡션 문장입니다. 생략 가능합니다. </param>
/// <returns> HRESULT 값이 S_FALSE일 경우 true를 반환합니다. </returns>
inline bool FailureMessage(HRESULT hr, LPCTSTR msg = TEXT(""), LPCTSTR captionMsg = TEXT("Error!"))
{
	if (FAILED(hr))
	{
		MessageBox(NULL, msg, captionMsg, MB_OK);
		return true;
	}
	return false;
}

/// <summary>
/// 클라이언트에서 실제 그려지는 길이를 제외한 시스템 사용 가로 길이입니다.
/// </summary>
int GetMarginWidth();

/// <summary>
/// 클라이언트에서 실제 그려지는 길이를 제외한 시스템 사용 세로 길이입니다.
/// </summary>
int GetMarginHeight();

/// <summary>
/// 클라이언트에서 실제 그려지는 길이를 제외한 시스템 사용 세로 길이입니다.
/// </summary>
HINSTANCE GetHInstance();



struct SFixValueFunc {
	template<typename Ty>
	static wstring _Call(Ty fixnum)
	{
		wstring szFix;
		Ty count = 0;
		Ty nEnd;

		while (fixnum > 0)
		{
			nEnd = fixnum % 10;
			fixnum /= 10;
			szFix += to_wstring(nEnd);
			count++;
			if (count % 4 == 3 && fixnum > 0)
			{
				szFix += ',';
				count++;
			}
		}

		if (count == 0) { szFix = L"0"; count++; }

		reverse(szFix.begin(), szFix.end());

		return szFix;
	}
};

template<typename Ty, bool isInteger = is_integral<Ty>::value>
struct SFixValue;

template<typename Ty>
struct SFixValue<Ty, true> : SFixValueFunc {};

template<typename Ty>
struct SFixValue<Ty, false>
{
	static wstring _Call(Ty fixnum)
	{
		static_assert(false, "FixValue only use inteager type!");
		return wstring();
	}
};

/// <summary>
/// 세자리 단위로 쉼표를 붙인 유니코드 문자열 숫자를 반환합니다.
/// </summary>
///	<param name = "fixnum"> 변형을 가할 정수형 숫자입니다. 정수형 이외의 값은 컴파일 에러 처리됩니다. </param>
/// <returns> wstring 값으로 반환됩니다. </returns>
template<typename Ty>
inline wstring FixValue(Ty fixnum)
{
	return SFixValue<Ty>::_Call(fixnum);
}



// TODO: 프로그램에 필요한 전처리기는 여기에서 참조합니다.

#define CLIENT_WIDTH	800
#define CLIENT_HEIGHT	600

#define CLIENT_MINIMUM_WIDTH	600
#define CLIENT_MINIMUM_HEIGHT	480


#ifdef _DEBUG
#define LOADSCOPATH(path) TEXT(##path)
#else
#define LOADSCOPATH(path) TEXT("..\\x64\\Release\\"##path)
#endif

// 캡션 FPS 출력 여부 -------------------
// 항상 캡션에 FPS를 출력		(0 : 비활성 | 1 : 활성)
#define USE_CAPTIONFPS_ALWAYS	 1
#define USE_TITLE_STRING		 0
#define USE_LIMIT_MAX_FPS		 0
#define USE_DEBUG_WINDOW		 0

#if USE_CAPTIONFPS_ALWAYS
#define SHOW_CAPTIONFPS 
#elif _DEBUG	// Debug에서는 항상 실행
#define SHOW_CAPTIONFPS 
#endif

#if defined(SHOW_CAPTIONFPS)
#define MAX_UPDATE_FPS 1.0 / 3.0
#endif

// 최대 FPS
#if _DEBUG
#define MAX_FPS 0.0
#elif USE_LIMIT_MAX_FPS
#define MAX_FPS 1.0 / 60.0
#else
#define MAX_FPS 0.0
#endif

#define TITLE_MAX_LENGTH 64

#if USE_TITLE_STRING
#define TITLESTRING	TEXT("Warp Framework")
#endif

#define DEBUG_CLIENT_WIDTH	400
#define DEBUG_CLIENT_HEIGHT	600

#if USE_DEBUG_WINDOW

#include "Framework\LogSystem\LogSystem.h"

#endif


