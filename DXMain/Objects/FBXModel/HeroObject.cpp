#include "stdafx.h"
#include "FBXModelMesh.h"
#include "HeroObject.h"

void CHeroObject::Update(float fTimeElapsed)
{
	float radiansPerSecond = XMConvertToRadians(45);
	double totalRotation = fTimeElapsed * radiansPerSecond;
	float radians = static_cast<float>(fmod(totalRotation, XM_2PI));
//	Rotate(radians);
}

void CHeroObject::Rotate(float radians)
{
	// 업데이트된 모델 매트릭스를 셰이더에 전달하도록 준비합니다.
	m_xmmtxLocal = m_xmmtxLocal * XMMatrixTranspose(XMMatrixRotationY(radians));
}

void CHeroObjectComponent::AddObject(XMVECTOR xmvec, ID3D11Device * pd3dDevice)
{
	unique_ptr<BaseObject> uniqueObj = make_unique<CHeroObject>();
	auto pObj = reinterpret_cast<HeroObject*>(uniqueObj.get());

	pObj->SetPosition(xmvec);
	if (pd3dDevice) pObj->CreateRasterizerState(pd3dDevice, D3D11_CULL_NONE);
	pObj->Rotate();
	CObjectComponent::AddObject(move(uniqueObj));

}

void CHeroObjectComponent::BuildComponent(ID3D11Device * pd3dDevice, ID2D1Factory * pd2dFactory, IDWriteFactory * pdwFactory, IWICImagingFactory * pwicFactory)
{

	m_ObjectMeterial.m_pTexture = make_unique<CTexture>(1, 1, 10, 10);
	auto pTexture = static_cast<CTexture*>(m_ObjectMeterial.m_pTexture.get());

	ComPtr<ID3D11SamplerState> pd3dSamplerState;
	D3D11_SAMPLER_DESC d3dSamplerDesc;
	ZeroMemory(&d3dSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
	d3dSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	d3dSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	d3dSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	d3dSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	d3dSamplerDesc.MinLOD = 0;
	d3dSamplerDesc.MaxLOD = 0;

	pd3dDevice->CreateSamplerState(&d3dSamplerDesc, pd3dSamplerState.GetAddressOf());
	pTexture->SetSampler(0, pd3dSamplerState.Get());

	ComPtr<ID3D11ShaderResourceView> srv;
	CreateWICTextureFromFile(pd3dDevice, TEXT("Graphics\\FBX\\t-unit04-1.jpg"), nullptr, srv.GetAddressOf());
	
	pTexture->SetTexture(0, srv.Get());
}

void CHeroObjectComponent::Render(ID3D11DeviceContext * pd3dDeviceContext, CCamera * pCamera)
{
	// Shader Set
	m_pShader->Render(pd3dDeviceContext);
	for (auto& p : m_lstObjects)
	{
		m_ObjectMeterial.UpdateTextureShaderVariable(pd3dDeviceContext, p.get());
		// Set States
		p->Render(pd3dDeviceContext);

		// Draw
		m_pMesh->Render(pd3dDeviceContext);

		// Release States
		p->OnFinishRender(pd3dDeviceContext);
	}

}

void CHeroObjectComponent::Update(float fTimeElapsed)
{
	CObjectComponent::Update(fTimeElapsed);
	reinterpret_cast<HeroMesh*>(m_pMesh.get())->Update(fTimeElapsed);
}
