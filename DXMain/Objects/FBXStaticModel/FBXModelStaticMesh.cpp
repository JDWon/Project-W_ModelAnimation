#include "stdafx.h"
#include "FBXModelStaticMesh.h"

CFBXModelStaticMesh::CFBXModelStaticMesh(ID3D11Device* pd3dDevice, XMFLOAT3 xmfSize) : CMesh()
{
	m_nVertices = 0;

	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// 2016_11_28 Add JD
	std::string tempStr;
	int meshCount;
	int polygonCount;

	XMFLOAT3 vertex = { 0, 0, 0 };
	XMFLOAT3 normal = { 0, 0, 0 };
	XMFLOAT2 UV = { 0, 0 };

	m_ifStreamForMesh.open("Graphics\\FBX\\unit04_attack.fbxModel.txt");
	//m_ifStreamForMesh.open("Graphics\\FBX\\assback3_11_attack.fbxModel.txt");

	m_ifStreamForMesh >> tempStr;

	m_ifStreamForMesh >> meshCount;

	for (int mesh = 0; mesh < meshCount; ++mesh)
	{
		m_ifStreamForMesh >> polygonCount;

		for (int i = 0; i < polygonCount; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				m_ifStreamForMesh >> vertex.x >> vertex.y >> vertex.z;
				vertex.x *= xmfSize.x;
				vertex.y *= xmfSize.y;
				vertex.z *= xmfSize.z;
				//vertex.y = 1.0f - vertex.y;
				m_ifStreamForMesh >> UV.x >> UV.y;
				m_ifStreamForMesh >> normal.x >> normal.y >> normal.z;

				cbuffer tempBuffer = {
					vertex,
					normal,
					UV
				};
				m_vVertexPositionColor.push_back(tempBuffer);
			}
		}
	}

	m_nVertices = m_vVertexPositionColor.size();

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
		, m_pd3dPositionTextureBuffer.GetAddressOf()
	);
	AssembleToVertexBuffers(m_pd3dPositionTextureBuffer.Get(), sizeof(cbuffer), 0);

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.ByteWidth = sizeof(GlobalMatrixBoneTransforms);
	d3dBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, nullptr, m_pcbBoneTransform.GetAddressOf());

}
