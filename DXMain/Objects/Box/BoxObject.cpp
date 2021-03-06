#include "stdafx.h"
#include "Objects\Base\Object.h"
#include "BoxObject.h"
#include "Camera\Camera.h"

void CBoxObject::Update(float fTimeElapsed)
{
//	float radiansPerSecond = XMConvertToRadians(45);
//	double totalRotation = fTimeElapsed * radiansPerSecond;
//	float radians = static_cast<float>(fmod(totalRotation, XM_2PI));
//	Rotate(radians);
	
}

void CBoxObject::Render2D(ID2D1RenderTarget * pd2dRenderTarget, IDWriteTextFormat* pdwTextFormat, ID2D1SolidColorBrush * pd2dsbrFont)
{

}

void CBoxObject::Rotate(float radians)
{
	// 업데이트된 모델 매트릭스를 셰이더에 전달하도록 준비합니다.
	m_xmmtxLocal = XMMatrixTranspose(XMMatrixRotationY(radians)) * m_xmmtxLocal;
}

CBoxObjectComponent::~CBoxObjectComponent()
{
}

void CBoxObjectComponent::AddObject(XMVECTOR xmvec, ID3D11Device * pd3dDevice)
{
	unique_ptr<BaseObject> uniqueObj = make_unique<BoxObject>();
	auto pObj = reinterpret_cast<BoxObject*>(uniqueObj.get());

	if (pd3dDevice) pObj->CreateBlendState(pd3dDevice);
	if (pd3dDevice) pObj->CreateRasterizerState(pd3dDevice, D3D11_CULL_NONE);
	
	pObj->SetPosition(xmvec);
	CObjectComponent::AddObject(move(uniqueObj));
}

void CBoxObjectComponent::Update(float fTimeElapsed)
{
	for (auto &p : m_lstObjects)
	{
		p->Update(fTimeElapsed);
	}
}



int Rand3(int id) {
	auto p = ((((id * 7) + 5) % 47) + 3) * 151 - 51;
	if (p % 7 > 3) p = (12125 - p % 12125);
	if (p % 3) p = (25 - ((p * 31 - 5) * 11 % 17)) * 147;
	return p;
}

void CBoxObjectComponent::Render(ID3D11DeviceContext * pd3dDeviceContext, CCamera* pCamera)
{
	// Shader Set
	m_pShader->Render(pd3dDeviceContext);
	for (auto& p : m_lstObjects)
	{
		auto id = Rand3(p->GetID()) % m_ObjectMeterial.size();
		m_ObjectMeterial[id].UpdateTextureShaderVariable(pd3dDeviceContext, p.get());
		// Set States
		p->Render(pd3dDeviceContext);

		// Draw
		m_pMesh->Render(pd3dDeviceContext);

		// Release States
		p->OnFinishRender(pd3dDeviceContext);
	}

	if (m_pSelectedObject)
	{
		m_TextureObjectDrawHP->SetRenderingPosition(m_pSelectedObject->GetLocalMatrix(), XMVectorSet(0, 0.9f, 0, 0), pCamera);
		m_TextureObjectSelectedFace->Render(pd3dDeviceContext, pCamera);
		m_TextureObjectDrawHP->Render(pd3dDeviceContext, pCamera);
	}

//	size_t cnt = 0;
//	for (auto& p : m_lstObjects)
//	{
//		m_vupDummy[cnt]->SetRenderingPosition(p->GetLocalMatrix(), XMVectorSet(0, 0.9f, 0, 0), pCamera);
//		m_vupDummy[cnt]->Render(pd3dDeviceContext, pCamera);
//		cnt++;
//	}

}

void CBoxObjectComponent::BuildComponent(ID3D11Device * pd3dDevice, ID2D1Factory * pd2dFactory, IDWriteFactory * pdwFactory, IWICImagingFactory * pwicFactory)
{
	m_TextureObjectDrawHP = make_unique<CTextureQuadObject>(pd3dDevice, 1.f, 0.5f);
	m_TextureObjectDrawHP->CreateDrawableTexture<CTextureDrawHP>(pwicFactory, pdwFactory, pd3dDevice, pd2dFactory, 200, 100);

	m_TextureObjectSelectedFace = make_unique<CTextureQuadObject>(pd3dDevice, 0.5f, 0.5f);
	m_TextureObjectSelectedFace->CreateTextureFromFile<CTexture>(pd3dDevice, TEXT("Graphics\\sample_picking.png"), 1, 1, 10, 10);

//	for (auto &p : m_lstObjects)
//	{
//		m_vupDummy.emplace_back(make_unique<CTextureQuadObject>(pd3dDevice, 1.f, 0.5f));
//		m_vupDummy.back()->CreateDrawableTexture<CTextureDrawHP>(pwicFactory, pdwFactory, pd3dDevice, pd2dFactory, 200, 100);
//
//		m_vupDummy.back()->Update(0, p.get());
//	}

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

	vector<wstring> graphicList = ReadStringListFromFileW(TEXT("지형텍스처입력.txt"));

	for (int i = 0; i < graphicList.size(); ++i)
	{
		m_ObjectMeterial.emplace_back();
		m_ObjectMeterial[i].m_pTexture = make_unique<CTexture>(1, 1, 10, 10);
		auto pTexture = static_cast<CTexture*>(m_ObjectMeterial[i].m_pTexture.get());
		ComPtr<ID3D11ShaderResourceView> srv;
		CreateWICTextureFromFile(pd3dDevice, graphicList[i].c_str(), nullptr, srv.GetAddressOf());
		pTexture->SetSampler(0, pd3dSamplerState.Get());
		pTexture->SetTexture(0, srv.Get());
	}
	


}
