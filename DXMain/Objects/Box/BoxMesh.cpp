#include "stdafx.h"
#include "Objects\Base\Mesh.h"
#include "BoxMesh.h"

CBoxMesh::CBoxMesh(ID3D11Device * pd3dDevice, float fWidth, float fHeight, float fDepth)
	: CMesh()
{
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	m_vVertexPositionColor =
	{
		{XMFLOAT3(-fWidth, +fHeight, -fDepth), XMFLOAT3(0.0f, 1.0f, 0.0f)},
		{XMFLOAT3(+fWidth, +fHeight, -fDepth), XMFLOAT3(1.0f, 1.0f, 0.0f)},
		{XMFLOAT3(+fWidth, +fHeight, +fDepth), XMFLOAT3(1.0f, 1.0f, 1.0f)},
		{XMFLOAT3(-fWidth, +fHeight, +fDepth), XMFLOAT3(0.0f, 1.0f, 1.0f)},
		{XMFLOAT3(-fWidth, -fHeight, -fDepth), XMFLOAT3(0.0f, 0.0f, 0.0f)},
		{XMFLOAT3(+fWidth, -fHeight, -fDepth), XMFLOAT3(1.0f, 0.0f, 0.0f)},
		{XMFLOAT3(+fWidth, -fHeight, +fDepth), XMFLOAT3(1.0f, 0.0f, 1.0f)},
		{XMFLOAT3(-fWidth, -fHeight, +fDepth), XMFLOAT3(0.0f, 0.0f, 1.0f)},
	};

	m_nVertices = static_cast<UINT>(m_vVertexPositionColor.size());
	
	D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
	vertexBufferData.pSysMem = &(m_vVertexPositionColor[0]);
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC vertexBufferDesc(
		  static_cast<UINT>(m_vVertexPositionColor.size() * sizeof(cbuffer))
		, D3D11_BIND_VERTEX_BUFFER);

	pd3dDevice->CreateBuffer(
		  &vertexBufferDesc
		, &vertexBufferData
		, m_pd3dVertexPositionColorBuffer.GetAddressOf()
	);
	AssembleToVertexBuffers(m_pd3dVertexPositionColorBuffer.Get(), sizeof(cbuffer), 0);

	m_vIndies =
	{
		  5
		, 6
		, 4
		, 7
		, 0
		, 3
		, 1
		, 2
		, 2
		, 3
		, 3
		, 7
		, 2
		, 6
		, 1
		, 5
		, 0
		, 4
	};

	// 메시 인덱스를 위한 인덱스 버퍼 생성
	D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
	indexBufferData.pSysMem = &(m_vIndies[0]);
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC indexBufferDesc(
		  static_cast<UINT>(m_vIndies.size() * sizeof(decltype(m_vIndies)::value_type))
		, D3D11_BIND_INDEX_BUFFER);

	pd3dDevice->CreateBuffer(
		  &indexBufferDesc
		, &indexBufferData
		, m_pIndexBuffer.GetAddressOf()
	);

}

int CBoxMesh::CheckRayIntersection(FXMVECTOR xmvRayPosition, FXMVECTOR xmvRayDirection, MESHINTERSECTINFO * pd3dxIntersectInfo)
{
	auto m_nIndices = static_cast<int>(m_vIndies.size());
	int nIntersections = 0;
	BYTE * pbPositions = reinterpret_cast<BYTE *>(&m_vVertexPositionColor[0]) + m_vVertexOffset[0];

	int nOffset = (m_d3dPrimitiveTopology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? 3 : 1;
	int nPrimitives = (m_d3dPrimitiveTopology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? (m_nVertices / 3) : (m_nVertices - 2);
	if (m_vIndies.size() > 0) nPrimitives = (m_d3dPrimitiveTopology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? (m_nIndices / 3) : (m_nIndices - 2);

	XMFLOAT3 v0, v1, v2;
	float fuHitBaryCentric = 0.f;
	float fvHitBaryCentric = 0.f;
	float fHitDistance = 0.f;
	float fNearHitDistance = FLT_MAX;
	
	for (int i = 0; i < nPrimitives; i++)
	{
		v0 = reinterpret_cast<cbuffer*>(pbPositions + ((m_nIndices > 0) ? (m_vIndies[(i * nOffset) + 0]) : ((i * nOffset) + 0)) * m_vVertexStrides[0])->pos;
		v1 = reinterpret_cast<cbuffer*>(pbPositions + ((m_nIndices > 0) ? (m_vIndies[(i * nOffset) + 1]) : ((i * nOffset) + 1)) * m_vVertexStrides[0])->pos;
		v2 = reinterpret_cast<cbuffer*>(pbPositions + ((m_nIndices > 0) ? (m_vIndies[(i * nOffset) + 2]) : ((i * nOffset) + 2)) * m_vVertexStrides[0])->pos;

		XMVECTOR xmv[3] { XMLoadFloat3(&v0), XMLoadFloat3(&v1), XMLoadFloat3(&v2) };
		// 원본 함수
//		if (DirectX::TriangleTests::Intersects(xmvRayPosition, XMVector4Normalize(xmvRayDirection), xmv[0], xmv[1], xmv[2], fHitDistance))
		if (::RayIntersectTriangle(xmvRayPosition, XMVector4Normalize(xmvRayDirection), xmv[0], xmv[1], xmv[2], &fuHitBaryCentric, &fvHitBaryCentric, &fHitDistance))
		{
			if (fHitDistance < fNearHitDistance)
			{
				fNearHitDistance = fHitDistance;
				if (pd3dxIntersectInfo)
				{
					pd3dxIntersectInfo->m_dwFaceIndex = i;
					pd3dxIntersectInfo->m_fU = fuHitBaryCentric;
					pd3dxIntersectInfo->m_fV = fvHitBaryCentric;
					pd3dxIntersectInfo->m_fDistance = fHitDistance;
				}
			}
			nIntersections++;
		}
	}

	return(nIntersections);
}



CTexturedBoxMesh::CTexturedBoxMesh(ID3D11Device * pd3dDevice, float fWidth, float fHeight, float fDepth)
	: CMesh()
{
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_vPositionTexture =
	{
		//	Front 01
		{ XMFLOAT3(-fWidth, +fHeight, -fDepth), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(+fWidth, +fHeight, -fDepth), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(+fWidth, -fHeight, -fDepth), XMFLOAT2(1.0f, 1.0f) },
		//	Front 02
		{ XMFLOAT3(-fWidth, +fHeight, -fDepth), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(+fWidth, -fHeight, -fDepth), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-fWidth, -fHeight, -fDepth), XMFLOAT2(0.0f, 1.0f) },
		//	Up 01
		{ XMFLOAT3(-fWidth, +fHeight, +fDepth), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(+fWidth, +fHeight, +fDepth), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(+fWidth, +fHeight, -fDepth), XMFLOAT2(1.0f, 1.0f) },
		//	Up 02
		{ XMFLOAT3(-fWidth, +fHeight, +fDepth), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(+fWidth, +fHeight, -fDepth), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-fWidth, +fHeight, -fDepth), XMFLOAT2(0.0f, 1.0f) },
		//	Back 01
		{ XMFLOAT3(-fWidth, -fHeight, +fDepth), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(+fWidth, -fHeight, +fDepth), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(+fWidth, +fHeight, +fDepth), XMFLOAT2(1.0f, 0.0f) },
		//	Back 02
		{ XMFLOAT3(-fWidth, -fHeight, +fDepth), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(+fWidth, +fHeight, +fDepth), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-fWidth, +fHeight, +fDepth), XMFLOAT2(0.0f, 0.0f) },
		//	Down 01
		{ XMFLOAT3(-fWidth, -fHeight, -fDepth), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(+fWidth, -fHeight, -fDepth), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(+fWidth, -fHeight, +fDepth), XMFLOAT2(1.0f, 1.0f) },
		//	Down 02
		{ XMFLOAT3(-fWidth, -fHeight, -fDepth), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(+fWidth, -fHeight, +fDepth), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-fWidth, -fHeight, +fDepth), XMFLOAT2(0.0f, 1.0f) },
		//	Left 01
		{ XMFLOAT3(-fWidth, +fHeight, +fDepth), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(-fWidth, +fHeight, -fDepth), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-fWidth, -fHeight, -fDepth), XMFLOAT2(1.0f, 1.0f) },
		//	Left 02
		{ XMFLOAT3(-fWidth, +fHeight, +fDepth), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(-fWidth, -fHeight, -fDepth), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-fWidth, -fHeight, +fDepth), XMFLOAT2(0.0f, 1.0f) },
		//	Right 01
		{ XMFLOAT3(+fWidth, +fHeight, -fDepth), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(+fWidth, +fHeight, +fDepth), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(+fWidth, -fHeight, +fDepth), XMFLOAT2(1.0f, 1.0f) },
		//	Right 02
		{ XMFLOAT3(+fWidth, +fHeight, -fDepth), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(+fWidth, -fHeight, +fDepth), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(+fWidth, -fHeight, -fDepth), XMFLOAT2(0.0f, 1.0f) },

	};

	m_nVertices = static_cast<UINT>(m_vPositionTexture.size());
	
	D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
	vertexBufferData.pSysMem = &(m_vPositionTexture[0]);
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC vertexBufferDesc(
		  static_cast<UINT>(m_vPositionTexture.size() * sizeof(cbuffer))
		, D3D11_BIND_VERTEX_BUFFER);

	pd3dDevice->CreateBuffer(
		  &vertexBufferDesc
		, &vertexBufferData
		, m_pd3dPositionTextureBuffer.GetAddressOf()
	);
	AssembleToVertexBuffers(m_pd3dPositionTextureBuffer.Get(), sizeof(cbuffer), 0);
	
}

int CTexturedBoxMesh::CheckRayIntersection(FXMVECTOR xmvRayPosition, FXMVECTOR xmvRayDirection, MESHINTERSECTINFO * pd3dxIntersectInfo)
{
	auto m_nIndices = static_cast<int>(m_vIndies.size());
	int nIntersections = 0;
	BYTE * pbPositions = reinterpret_cast<BYTE *>(&m_vPositionTexture[0]) + m_vVertexOffset[0];

	int nOffset = (m_d3dPrimitiveTopology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? 3 : 1;
	int nPrimitives = (m_d3dPrimitiveTopology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? (m_nVertices / 3) : (m_nVertices - 2);
	if (m_vIndies.size() > 0) nPrimitives = (m_d3dPrimitiveTopology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? (m_nIndices / 3) : (m_nIndices - 2);

	XMFLOAT3 v0, v1, v2;
	float fuHitBaryCentric = 0.f;
	float fvHitBaryCentric = 0.f;
	float fHitDistance = 0.f;
	float fNearHitDistance = FLT_MAX;
	
	for (int i = 0; i < nPrimitives; i++)
	{
		v0 = reinterpret_cast<cbuffer*>(pbPositions + ((m_nIndices > 0) ? (m_vIndies[(i * nOffset) + 0]) : ((i * nOffset) + 0)) * m_vVertexStrides[0])->pos;
		v1 = reinterpret_cast<cbuffer*>(pbPositions + ((m_nIndices > 0) ? (m_vIndies[(i * nOffset) + 1]) : ((i * nOffset) + 1)) * m_vVertexStrides[0])->pos;
		v2 = reinterpret_cast<cbuffer*>(pbPositions + ((m_nIndices > 0) ? (m_vIndies[(i * nOffset) + 2]) : ((i * nOffset) + 2)) * m_vVertexStrides[0])->pos;

		XMVECTOR xmv[3] { XMLoadFloat3(&v0), XMLoadFloat3(&v1), XMLoadFloat3(&v2) };
		
		if (::RayIntersectTriangle(xmvRayPosition, XMVector4Normalize(xmvRayDirection), xmv[0], xmv[1], xmv[2], &fuHitBaryCentric, &fvHitBaryCentric, &fHitDistance))
		{
			if (fHitDistance < fNearHitDistance)
			{
				fNearHitDistance = fHitDistance;
				if (pd3dxIntersectInfo)
				{
					pd3dxIntersectInfo->m_dwFaceIndex = i;
					pd3dxIntersectInfo->m_fU = fuHitBaryCentric;
					pd3dxIntersectInfo->m_fV = fvHitBaryCentric;
					pd3dxIntersectInfo->m_fDistance = fHitDistance;
				}
			}
			nIntersections++;
		}
	}

	return(nIntersections);
}
