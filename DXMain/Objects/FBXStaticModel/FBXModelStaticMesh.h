#pragma once

#include "Objects\Base\Mesh.h"
#include "HLSL\BindingBufferStructs.h"

class CFBXModelStaticMesh : public CMesh {

public:
	using cbuffer = FBXModelStaticConstantBuffer;

public:

	CFBXModelStaticMesh(ID3D11Device* pd3dDevice, XMFLOAT3 xmfSize);
	virtual int CheckRayIntersection(FXMVECTOR xmvRayPosition, FXMVECTOR xmvRayDirection, MESHINTERSECTINFO * pd3dxIntersectInfo) { return 0; }

protected:

	ComPtr<ID3D11Buffer>			m_pd3dPositionTextureBuffer{ nullptr };
	ComPtr<ID3D11Buffer>			m_pcbBoneTransform{ nullptr };
	vector<cbuffer>					m_vVertexPositionColor;

	ifstream						m_ifStreamForMesh;
};