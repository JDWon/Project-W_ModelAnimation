#pragma once

#include "FBXLoader\FBXLoader.h"
#include "Objects\Base\Mesh.h"
#include "HLSL\BindingBufferStructs.h"

struct KeyFrameData
{
	int		mFrameNum;
	FbxAMatrix	mGlobalTransform;
	//XMMATRIX	mGlobalTransform;
};

struct JointInfoData
{
	int mParentIndex;
	std::string mName;

	FbxAMatrix mGlobalBindposeInverse;
	//XMMATRIX mGlobalBindposeInverse;
	//FbxAMatrix mGlobalBindposeInverse;
	bool	mHasBonedata;

	std::vector<KeyFrameData> mKeyFrames;
	//std::vector<KeyFrame> mKeyFrames;

	JointInfoData()
	{
		// mGlobalBindposeInverse.SetIdentity();
		mParentIndex = -1;
		mKeyFrames.clear();
	}

	~JointInfoData()
	{
		mKeyFrames.clear();
	}

};

struct SkeltonInfoData
{
	std::vector<JointInfoData> m_vJoint;
};


class CFBXModelMesh : public CMesh {

public:
	using cbuffer = FBXModelConstantBuffer ;
	CFBXAnimationLoader			*m_fbxAniLoader;

public:

	CFBXModelMesh(ID3D11Device* pd3dDevice, XMVECTOR xmfSize);
	virtual int CheckRayIntersection(FXMVECTOR xmvRayPosition, FXMVECTOR xmvRayDirection, MESHINTERSECTINFO * pd3dxIntersectInfo) { return 0; }
	virtual void OnPrepareRender(ID3D11DeviceContext *pd3dDeviceContext);

	void Update(float fTimeElapsed);

protected:

	ComPtr<ID3D11Buffer>			m_pd3dPositionTextureBuffer		{ nullptr }	;
	ComPtr<ID3D11Buffer>			m_pcbBoneTransform				{ nullptr }	;
	vector<cbuffer>					m_vVertexPositionColor						;
	SkeltonInfoData					m_skeltonInfo								;

	ifstream						m_ifStreamForMesh							;
	ifstream						m_ifStreamForAni							;

	int								m_currentKeyFrameNum			{ 0 }		;
	int								m_aniStartTime					{ 0 }		;
	int								m_aniEndTime					{ 0 }		;
	int								m_jointCount					{ 0 }		;
	float							m_fTick							{ 0 }		;
	XMVECTOR						m_fModelScale					{ 0, 0, 0, 0 } ;

};