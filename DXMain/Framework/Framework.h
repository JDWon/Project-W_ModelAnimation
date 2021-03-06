#pragma once

class CScene;


///	<remarks> 
///	Create 함수는 객체의 할당 여부를 검사하지 않습니다. 할당될 객체들은 모두 해제되는 등 할당되지 않은 상태여야 합니다.
/// 모든 할당되지 않은 객체는 nullptr를 유지해야 합니다.
///	</remarks>
class CDirectXFramework {

private:

	ComPtr<ID3D11Device2>			m_pd3dDevice				{ nullptr }	;
	ComPtr<IDXGISwapChain2>			m_pdxgiSwapChain			{ nullptr }	;

	ComPtr<ID3D11RenderTargetView>	m_pd3dRenderTargetView		{ nullptr }	;
	ComPtr<ID3D11DeviceContext2>	m_pd3dDeviceContext			{ nullptr }	;

	HINSTANCE						m_hInstance					{ NULL }	;
	HWND							m_hWnd						{ NULL }	;
	RECT							m_rcClient								;

	// for fullscreen(window mode)
	RECT							m_rcClientRememberLast					;
	POINT							m_ptClientPosRememberLast				;
	bool							m_bClientSetFullScreenMode	{  false  }	;
	DWORD							m_dwWindowStyleRememberLast				;
	DWORD							m_dwWindowStyleExRememberLast			;


	// Rendering 추가 부분 : Depth-Stencil 을 하지 않으면 정상적인 큐브가 출력되지 않는다.
	ComPtr<ID3D11Texture2D>			m_pd3dDepthStencilBuffer	{ nullptr }	;
	ComPtr<ID3D11DepthStencilView>	m_pd3dDepthStencilView		{ nullptr }	;

	DXGI_FORMAT						m_dxgiFormatCurrentSelect { DXGI_FORMAT_R16G16B16A16_FLOAT }	;

public:

	CDirectXFramework();
	
	~CDirectXFramework();

	bool Initialize(HINSTANCE hInstance, HWND hWnd);

	void ReleaseObjects();

	bool CreateD3D11Deivce();

	bool CreateRenderTargetDepthStencilView();

	void ReleaseD3D11Devices();

	void SetFullscreen_WindowMode();



	void Render();

	void Update(float fTimeElapsed);

	auto GetD3D11Device() const { return m_pd3dDevice.Get(); }

	// 2D code
private:

	ComPtr<ID2D1Device2>				m_pd2dDevice		{ nullptr }	;
	ComPtr<ID2D1Factory3>				m_pd2dFactory		{ nullptr }	;
	ComPtr<ID2D1DeviceContext2>			m_pd2dDeviceContext	{ nullptr }	;
	ComPtr<IDWriteFactory3>				m_pdwFactory		{ nullptr }	;
	ComPtr<IWICImagingFactory2>			m_wicFactory		{ nullptr }	;
	
	// 3D SwapChain에서 RenderTarget을 얻기 위한 BackBuffer 입니다.
	ComPtr<ID2D1Bitmap1>				m_pd2dBmpBackBuffer { nullptr }	;
	
	// 이전까지의 Drawing 상태를 저장합니다.
	ComPtr<ID2D1DrawingStateBlock1>		m_pd2dStateBlock	{ nullptr }	;


	void ReleaseD2DResources();

public:

	bool CreateD2D1Device(IDXGIDevice3* pdxgiDevice);

	bool CreateD2DBackBuffer();

	void Render2D();



// 게임 전체의 로직을 담당합니다.
private:

	std::chrono::system_clock::time_point m_current_time;
	std::chrono::duration<double> m_timeElapsed; // 시간이 얼마나 지났나

	double		m_fps = 0.0;

	TCHAR		m_CaptionTitle[TITLE_MAX_LENGTH];
	int			m_TitleLength;

	double		m_dCumulativefps = 0.0;
	int			m_nCumulativefpsCount = 0;

	std::chrono::system_clock::time_point m_LastUpdate_time;
	std::chrono::duration<double> m_UpdateElapsed; // 시간이 얼마나 지났나

	using FLOAT4 = float[4];

	std::list<std::unique_ptr<CScene>>		m_lstScenes;
	CScene								*	m_pCurrentScene	{ nullptr }	;
public:

	// 후 처리가 없을 경우 true 반환
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	// 후 처리가 없을 경우 true 반환
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	// 후 처리가 없을 경우 true 반환
	virtual LRESULT OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void BuildScene();
	void FrameAdvance();

	// Framework 에서 호출하는 윈도우 프로시저입니다.
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
};
