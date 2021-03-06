#include "stdafx.h"
#include "HLSL\BindingBufferStructs.h"
#include "Shader.h"
#include "Mesh.h"
#include "Object.h"

ID3D11Buffer* CObject::m_pd3dcbWorldMatrix = nullptr;

CObject::CObject()
{
	static UINT uniqueID = 0;
	// 고유 ID 할당
	m_ID = ++uniqueID;
	m_xmmtxLocal = XMMatrixIdentity();

}

CObject::CObject(FXMVECTOR xmvPosition) : CObject()
{
	m_xmmtxLocal.r[3] = xmvPosition;
}

CObject::~CObject()
{
	SafeRelease(m_pd3dBlendState);
	SafeRelease(m_pd3dRasterizerState);
}

// 오로지 Vertex Shader 에 모델의 좌표를 넘겨주는 역할을 수행할 뿐인 버퍼를 생성합니다.
void CObject::CreateModelMatrixBuffer(ID3D11Device * pd3dDevice)
{
	CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);

	pd3dDevice->CreateBuffer(	  &constantBufferDesc
								, nullptr
								, &m_pd3dcbWorldMatrix
	);
}

void CObject::UpdateAnimationTextureShaderVariable(ID3D11DeviceContext * pd3dDeviceContext, FXMMATRIX pd3dxmtxWorld)
{
	ModelConstantBuffer modelBuffer;
	XMStoreFloat4x4A(&modelBuffer.model, pd3dxmtxWorld);
	
	// 그래픽 장치로 보낼 수 있도록 상수 버퍼를 준비합니다.
	pd3dDeviceContext->UpdateSubresource(	  m_pd3dcbWorldMatrix
											, 0
											, NULL
											, &modelBuffer
											, 0
											, 0
	);

	// 셰이더에 연결합니다.
	pd3dDeviceContext->VSSetConstantBuffers(1, 1, &m_pd3dcbWorldMatrix);
}

void CObject::CreateRasterizerState(ID3D11Device * pd3dDevice, D3D11_CULL_MODE cull, D3D11_FILL_MODE fill)
{
	// Create RasterizerState
	D3D11_RASTERIZER_DESC d3dRasterizerDesc;
	ZeroMemory(&d3dRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	d3dRasterizerDesc.CullMode = cull;
	d3dRasterizerDesc.FillMode = fill;

	CreateRasterizerState(pd3dDevice, d3dRasterizerDesc);
}

void CObject::CreateRasterizerState(ID3D11Device * pd3dDevice, D3D11_RASTERIZER_DESC rasterizerDesc)
{
	pd3dDevice->CreateRasterizerState(	  &rasterizerDesc
										, reinterpret_cast<ID3D11RasterizerState**>(&m_pd3dRasterizerState)
	);
}

void CObject::CreateBlendState(ID3D11Device * pd3dDevice, D3D11_BLEND_DESC blendDesc)
{
	pd3dDevice->CreateBlendState(&blendDesc
		, reinterpret_cast<ID3D11BlendState**>(&m_pd3dBlendState)
	);
}

void CObject::CreateBlendState(ID3D11Device * pd3dDevice)
{
	D3D11_BLEND_DESC d3dBlendStateDesc;
	ZeroMemory(&d3dBlendStateDesc, sizeof(D3D11_BLEND_DESC));
	d3dBlendStateDesc.AlphaToCoverageEnable = false;
	d3dBlendStateDesc.IndependentBlendEnable = false;
	d3dBlendStateDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	d3dBlendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	d3dBlendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	d3dBlendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	d3dBlendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	d3dBlendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	d3dBlendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	CreateBlendState(pd3dDevice, d3dBlendStateDesc);
}

void CObject::MoveStrafe(float fDistance)
{
	XMVECTOR xmPosition = GetPosition();
	XMVECTOR xmRight = GetRight();
	xmPosition += fDistance * xmRight;
	SetPosition(xmPosition);
}

void CObject::MoveUp(float fDistance)
{
	XMVECTOR xmPosition = GetPosition();
	XMVECTOR xmUp = GetUp();
	xmPosition += fDistance * xmUp;
	SetPosition(xmPosition);
}

void CObject::MoveForward(float fDistance)
{
	XMVECTOR xmPosition = GetPosition();
	XMVECTOR xmLookAt = GetLook();
	xmPosition += fDistance * xmLookAt;
	SetPosition(xmPosition);
}

int CObject::PickObjectByRayIntersection(shared_ptr<CMesh> pMesh, FXMVECTOR xmvPickPosition, XMMATRIX * pxmmtxView, MESHINTERSECTINFO * pd3dxIntersectInfo)
{
	XMVECTOR d3dxvPickRayPosition, d3dxvPickRayDirection;
	int nIntersected = 0;
	if (pMesh)
	{
		GenerateRayForPicking(xmvPickPosition, &m_xmmtxLocal, pxmmtxView, d3dxvPickRayPosition, d3dxvPickRayDirection);
		nIntersected = pMesh->CheckRayIntersection(d3dxvPickRayPosition, d3dxvPickRayDirection, pd3dxIntersectInfo);
	}
	return(nIntersected);
}

int CObject::PickObjectByRayIntersection(vector<shared_ptr<CMesh>> vMeshes, FXMVECTOR xmvPickPosition, XMMATRIX * pxmmtxView, MESHINTERSECTINFO * pd3dxIntersectInfo)
{
	XMVECTOR d3dxvPickRayPosition, d3dxvPickRayDirection;
	int nIntersected = 0;
	for (auto& p : vMeshes)
	{
		GenerateRayForPicking(xmvPickPosition, &m_xmmtxLocal, pxmmtxView, d3dxvPickRayPosition, d3dxvPickRayDirection);
		nIntersected = p->CheckRayIntersection(d3dxvPickRayPosition, d3dxvPickRayDirection, pd3dxIntersectInfo);
		if (nIntersected > 0) break;
	}
	return(nIntersected);
}

void CObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw((float) XMConvertToDegrees(fRoll), (float) XMConvertToDegrees(fPitch), (float) XMConvertToDegrees(fYaw));
	m_xmmtxLocal = m_xmmtxLocal * mtxRotate;
}

void CObject::Rotate(FXMVECTOR pxmAxis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(pxmAxis, (float) XMConvertToDegrees(fAngle));
	m_xmmtxLocal = m_xmmtxLocal * mtxRotate;
}

void CObject::OnPrepareRender(ID3D11DeviceContext * pd3dDeviceContext)
{
	// Object 지역 좌표를 갱신합니다.
	UpdateAnimationTextureShaderVariable(pd3dDeviceContext, XMMatrixTranspose(m_xmmtxLocal));

	if (m_pd3dRasterizerState) pd3dDeviceContext->RSSetState(m_pd3dRasterizerState);
	if (m_pd3dBlendState) pd3dDeviceContext->OMSetBlendState(m_pd3dBlendState, nullptr, 0xffffffff);
}

void CObject::OnFinishRender(ID3D11DeviceContext * pd3dDeviceContext)
{
	if (m_pd3dRasterizerState) pd3dDeviceContext->RSSetState(nullptr);
	if (m_pd3dBlendState) pd3dDeviceContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);

}

void CObject::Render(ID3D11DeviceContext * pd3dDeviceContext, CCamera* pCamera)
{
	OnPrepareRender(pd3dDeviceContext);
}




CObjectComponent::~CObjectComponent()
{
}

void CObjectComponent::EnquireShaderAndMesh(shared_ptr<BaseShader> pShader, shared_ptr<BaseMesh> pMesh)
{
	m_pShader = pShader;
	m_pMesh = pMesh;
}

void CObjectComponent::AddObject(unique_ptr<BaseObject>&& obj)
{
	m_lstObjects.push_back(move(obj));
}

void CObjectComponent::OnPrepareRender(ID3D11DeviceContext * pd3dDeviceContext)
{
}

void CObjectComponent::Render(ID3D11DeviceContext * pd3dDeviceContext)
{
	// Shader Set
	m_pShader->Render(pd3dDeviceContext);

	for (auto& p : m_lstObjects)
	{
		// Set States
		p->Render(pd3dDeviceContext);

		// Draw
		m_pMesh->Render(pd3dDeviceContext);

		// Release States
		p->OnFinishRender(pd3dDeviceContext);
	}
}

void CObjectComponent::Update(float fTimeElapsed)
{
	for (auto &p : m_lstObjects)
	{
		p->Update(fTimeElapsed);
	}
}
