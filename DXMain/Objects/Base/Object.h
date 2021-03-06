#pragma once

class CShaderBase;
class CMesh;
class CCamera;


class CObject {

public:

	CObject();
	
	CObject(FXMVECTOR xmvPosition);

	virtual ~CObject();

	UINT GetID() const { return m_ID; }

	static void CreateModelMatrixBuffer(ID3D11Device *pd3dDevice);
	static void UpdateAnimationTextureShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, FXMMATRIX pd3dxmtxWorld);

	void CreateRasterizerState(ID3D11Device* pd3dDevice, D3D11_CULL_MODE cull = D3D11_CULL_BACK, D3D11_FILL_MODE fill = D3D11_FILL_SOLID);

	void CreateRasterizerState(ID3D11Device* pd3dDevice, D3D11_RASTERIZER_DESC rasterizerDesc);

	void CreateBlendState(ID3D11Device* pd3dDevice, D3D11_BLEND_DESC blendDesc);

	void CreateBlendState(ID3D11Device* pd3dDevice);

	void SetPosition(float x, float y, float z) { m_xmmtxLocal.r[3] = XMVectorSet(x, y, z, 1); }

	void SetPosition(FXMVECTOR xmPosition) { m_xmmtxLocal.r[3] = xmPosition; }

	virtual void MoveStrafe(float fDistance);

	virtual void MoveUp(float fDistance);

	virtual void MoveForward(float fDistance);



	/// <summary>
	/// PickRay 위치와 PickRay 방향을 생성하는 함수입니다.
	/// </summary>
	///	<param name = "xmvPickPosition"> 선택한 위치입니다. </param>
	///	<param name = "pxmmtxModel"> 객체의 모델 행렬입니다. </param>
	///	<param name = "xmmtxView"> 현재 카메라의 뷰 행렬입니다. </param>
	///	<param name = "xmvPickRayPosition"> 반환값입니다. 이 값은 위치를 반환합니다. </param>
	///	<param name = "xmvPickRayDirection"> 반환값입니다. 이 값은 방향을 반혼합니다. </param>
	/// <remarks> </remarks>
	/// <returns> 이 함수는 생성된 xmvPickRayPosition 과 xmvPickRayDirection 을 반환합니다. </returns>
	void GenerateRayForPicking(	  FXMVECTOR xmvPickPosition
								, XMMATRIX *pxmmtxModel
								, XMMATRIX *xmmtxView
								, XMVECTOR& xmvPickRayPosition
								, XMVECTOR& xmvPickRayDirection) const
	{
		XMMATRIX xmmtxModelView = *xmmtxView;
		if (pxmmtxModel) xmmtxModelView = XMMatrixMultiply(*pxmmtxModel, *xmmtxView);
		XMMATRIX xmmtxInverse = XMMatrixInverse(nullptr, xmmtxModelView);

		XMVECTOR xmvCameraOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		xmvPickRayPosition		= XMVector3TransformCoord(xmvCameraOrigin, xmmtxInverse);
		xmvPickRayDirection		= XMVector3TransformCoord(xmvPickPosition, xmmtxInverse);
		xmvPickRayDirection -= xmvPickRayPosition;
	}

	int PickObjectByRayIntersection(shared_ptr<CMesh> pMesh, FXMVECTOR xmvPickPosition, XMMATRIX *pxmmtxView, struct MESHINTERSECTINFO *pd3dxIntersectInfo);

	int PickObjectByRayIntersection(vector<shared_ptr<CMesh>> vMeshes, FXMVECTOR xmvPickPosition, XMMATRIX *pxmmtxView, struct MESHINTERSECTINFO *pd3dxIntersectInfo);

	virtual void Rotate(float fPitch, float fYaw, float fRoll);

	virtual void Rotate(FXMVECTOR pxmAxis, float fAngle);



	FXMVECTOR GetPosition() const { return m_xmmtxLocal.r[3]; }

	FXMVECTOR GetRight() const { return XMVector3Normalize(m_xmmtxLocal.r[0]); }

	FXMVECTOR GetUp() const { return XMVector3Normalize(m_xmmtxLocal.r[1]); }

	FXMVECTOR GetLook() const { return XMVector3Normalize(m_xmmtxLocal.r[2]); }

	FXMMATRIX GetLocalMatrix() const { return m_xmmtxLocal; }

	virtual void OnPrepareRender(ID3D11DeviceContext* pd3dDeviceContext);

	virtual void OnFinishRender(ID3D11DeviceContext* pd3dDeviceContext);

	virtual void Render(ID3D11DeviceContext* pd3dDeviceContext, CCamera* pCamera = nullptr);
	
	virtual void Update(float fTimeElapsed) {}

protected:

	UINT						m_ID					{ 0 }		;
	XMMATRIX					m_xmmtxLocal			{}			;

	static ID3D11Buffer		*	m_pd3dcbWorldMatrix					;

	ID3D11RasterizerState1	*	m_pd3dRasterizerState	{ nullptr }	;
	ID3D11BlendState		*	m_pd3dBlendState		{ nullptr }	;
};



class CObjectComponent {

protected:
	
	using BaseShader = CShaderBase;
	using BaseMesh = CMesh;

	using BaseObject = CObject;

public:

	CObjectComponent() = default;
	virtual ~CObjectComponent();

	void EnquireShaderAndMesh(shared_ptr<BaseShader> pShader, shared_ptr<BaseMesh> pMesh);

	virtual void AddObject(unique_ptr<BaseObject>&& obj);

	template<typename Fn>
	void RemoveObject(Fn&& func) { m_lstObjects.remove_if(func); }

	virtual void OnPrepareRender(ID3D11DeviceContext* pd3dDeviceContext);
	virtual void Render(ID3D11DeviceContext* pd3dDeviceContext);

	virtual void Update(float fTimeElapsed);

	auto& GetObjects() const { return m_lstObjects; }

	template<typename Fn>
	auto find(Fn&& func) { return std::find_if(m_lstObjects.begin(), m_lstObjects.end(), func)->get(); }

	auto& GetMesh() const { return m_pMesh; }

	virtual void BuildComponent(	  ID3D11Device * pd3dDevice
									, ID2D1Factory * pd2dFactory = nullptr
									, IDWriteFactory * pdwFactory = nullptr
									, IWICImagingFactory * pwicFactory = nullptr)
	{

	}

protected:

	shared_ptr<CShaderBase>			m_pShader		{ nullptr }	;
	shared_ptr<CMesh>				m_pMesh			{ nullptr }	;

	list<unique_ptr<CObject>>		m_lstObjects	{}			;

};