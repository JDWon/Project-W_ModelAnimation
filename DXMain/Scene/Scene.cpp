//-----------------------------------------------------------------------------
// File: Scene.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Framework\Framework.h"
#include "Objects\Base\Mesh.h"
#include "Objects\Base\Object.h"
#include "Scene.h"

CScene::CScene()
{

}

CScene::~CScene()
{
	ReleaseObjects();
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		break;
	case WM_RBUTTONDOWN:
		break;
	case WM_MOUSEMOVE:
		break;
	case WM_LBUTTONUP:
		break;
	case WM_RBUTTONUP:
		break;
	default:
		break;
	}

	return(true);
}

bool CScene::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_SIZE:
		break;
	default:
		return false;
	}

	return true;
}

bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		break;
	default:
		break;
	}
	return(true);
}

void CScene::BuildObjects(wstring Tag, HWND hWnd, CDirectXFramework *pMasterFramework)
{
	m_wsTag = Tag;
	m_hWnd = hWnd;
	m_pMasterFramework = pMasterFramework;
}

void CScene::BuildObjecsFromD3D11Devices(ID3D11Device* pd3dDevice, ID3D11DeviceContext *pd3dDeviceContext)
{
}

void CScene::BuildObjecsFromD2D1Devices(ID2D1Device2 * pd2dDevice, ID2D1DeviceContext2 * pd2dDeviceContext, IDWriteFactory3 * pdwFactory)
{
}

void CScene::ReleaseObjects()
{
}

void CScene::Update2D(float fTimeElapsed)
{
}

void CScene::Update3D(float fTimeElapsed)
{
}

void CScene::AnimateObjects(float fTimeElapsed)
{

}

void CScene::Render2D(ID2D1DeviceContext2 * pd2dDeviceContext)
{
}

void CScene::Render3D(ID3D11DeviceContext *pd3dDeviceContext)
{

}

PickingObject CScene::PickObjectPointedByCursor(int xClient, int yClient, const CObjectComponent& SearchObjectComponent)
{

	XMMATRIX xmmtxView = m_Camera.GetViewMatrix();
	XMMATRIX d3dxmtxProjection = m_Camera.GetProjectionMatrix();
	D3D11_VIEWPORT d3dViewport = m_Camera.GetViewport();

	// 선택한 좌표를 투영 행렬 직전까지 변환
	XMVECTOR d3dxvPickPosition = XMVectorSet(
		  +(((2.0f * xClient) / d3dViewport.Width)  - 1) / XMVectorGetX(d3dxmtxProjection.r[0])
		, -(((2.0f * yClient) / d3dViewport.Height) - 1) / XMVectorGetY(d3dxmtxProjection.r[1])
		, 1.0f	// 찍을 수 있는 가장 멀리까지
		, 0.0f
	);

	int nIntersected = 0;
	float fNearHitDistance = FLT_MAX;

	// 가장 가까운 피킹 객체의 정보입니다.
	MESHINTERSECTINFO d3dxNearestIntersectInfo {};
	MESHINTERSECTINFO d3dxIntersectInfo; 
	CObject *pNearestObject = nullptr;

	auto pMesh = SearchObjectComponent.GetMesh();

	for (auto &p : SearchObjectComponent.GetObjects())
	{
		nIntersected = p->PickObjectByRayIntersection(pMesh, d3dxvPickPosition, &xmmtxView, &d3dxIntersectInfo);
		if ((nIntersected > 0) && (d3dxIntersectInfo.m_fDistance < fNearHitDistance))
		{
			fNearHitDistance = d3dxIntersectInfo.m_fDistance;
			d3dxNearestIntersectInfo = d3dxIntersectInfo;
			pNearestObject = p.get();
		}
	}
	
	return PickingObject { pNearestObject, d3dxNearestIntersectInfo };
}

bool CScene::FindByTag(std::wstring Tag)
{
	return Tag == m_wsTag;
}
