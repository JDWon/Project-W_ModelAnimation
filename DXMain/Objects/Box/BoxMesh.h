#pragma once

#include "Objects\Base\Mesh.h"
#include "HLSL\BindingBufferStructs.h"

enum class EBoxFace {
	  Front
	, Up
	, Back
	, Down
	, Left
	, Right
};

class CBoxMesh : public CMesh {

public:
	using cbuffer = VertexPositionColor;

public:

	CBoxMesh(ID3D11Device* pd3dDevice, float fWidth = 0.5f, float fHeight = 0.5f, float fDepth = 0.5f);
	~CBoxMesh() = default;

	virtual int CheckRayIntersection(FXMVECTOR xmvRayPosition, FXMVECTOR xmvRayDirection, MESHINTERSECTINFO * pd3dxIntersectInfo);

protected:

	ComPtr<ID3D11Buffer>			m_pd3dVertexPositionColorBuffer { nullptr }	;
	vector<cbuffer>					m_vVertexPositionColor						;

};

// Front, Up, Back, Down, Left, Right
class CTexturedBoxMesh : public CMesh {

public:
	using cbuffer = PositionTextureConstantBuffer;

public:

	CTexturedBoxMesh(ID3D11Device* pd3dDevice, float fWidth = 0.5f, float fHeight = 0.5f, float fDepth = 0.5f);
	~CTexturedBoxMesh() = default;

	virtual int CheckRayIntersection(FXMVECTOR xmvRayPosition, FXMVECTOR xmvRayDirection, MESHINTERSECTINFO * pd3dxIntersectInfo);

protected:

	ComPtr<ID3D11Buffer>			m_pd3dPositionTextureBuffer { nullptr }	;
	vector<cbuffer>					m_vPositionTexture						;

};