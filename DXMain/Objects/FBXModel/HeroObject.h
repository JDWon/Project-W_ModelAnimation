#pragma once

#include "FBXModelMesh.h"
#include "Scene\Scene.h"
#include "Objects\Base\Object.h"
#include "Objects\Base\Shader.h"
#include "Objects\Base\Meterial.h"
#include "Objects\TextureQuad\TextureQuadObject.h"

class CHeroObject : public CObject {

public:

	CHeroObject() : CObject() {}
	virtual ~CHeroObject() = default;

	virtual void Update(float fTimeElapsed);
	void Rotate(float radians);
	void Rotate()
	{
		m_xmmtxLocal = XMMatrixTranspose(XMMatrixRotationX(XMConvertToRadians(90))) * XMMatrixTranspose(XMMatrixRotationY(XMConvertToRadians(180))) * m_xmmtxLocal;

	}

protected:

};



class CHeroObjectComponent : public CObjectComponent {

public:

	using HeroShader = CShader<FBXModelConstantBuffer>;
	using HeroMesh = CFBXModelMesh;
	using HeroObject = CHeroObject;

public:

	CHeroObjectComponent() = default;
	virtual ~CHeroObjectComponent() = default;

	void AddObject(XMVECTOR xmvec, ID3D11Device * pd3dDevice = nullptr);
	
	virtual void BuildComponent(	  ID3D11Device			* pd3dDevice
									, ID2D1Factory			* pd2dFactory	= nullptr
									, IDWriteFactory		* pdwFactory	= nullptr
									, IWICImagingFactory	* pwicFactory	= nullptr);
	
	virtual void Render(ID3D11DeviceContext * pd3dDeviceContext, CCamera* pCamera);
	virtual void Update(float fTimeElapsed);


protected:

	CMaterial							m_ObjectMeterial					;

};
