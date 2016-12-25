#include "stdafx.h"
#include "FBXLoader/FBXLoader.h"
#include "Objects/Base/Mesh.h"
#include "FBXModelMesh.h"

CFBXModelMesh::CFBXModelMesh(ID3D11Device* pd3dDevice, XMVECTOR xmfSize)
	: CMesh()
{
	{
		m_fbxAniLoader = new CFBXAnimationLoader("Graphics\\FBX\\unit04_attack.fbx", 1.0f, false);
		m_fbxAniLoader->ProcessAnimationInfo();
	}

	m_fModelScale = xmfSize;

	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// 2016_11_28 Add JD
	std::string tempStr;
	int meshCount;
	int polygonCount;

	XMFLOAT3 vertex = {0, 0, 0};
	XMFLOAT3 normal = { 0, 0, 0 };
	XMFLOAT2 UV = { 0, 0 };

	XMUINT4 boneIndice = { 0, 0, 0, 0 };
	XMFLOAT3 weight = { 0, 0, 0 };

	m_ifStreamForMesh.open("Graphics\\FBX\\unit04_attack.FBXModel.txt");
	m_ifStreamForAni.open("Graphics\\FBX\\unit04_attack.FBXAni.txt");

	//m_ifStreamForMesh.open("Graphics\\FBX\\Player.FBXModel.txt");
	//m_ifStreamForAni.open("Graphics\\FBX\\Player.FBXAni.txt");

	m_ifStreamForMesh >> tempStr;
	m_ifStreamForMesh >> meshCount;

	m_ifStreamForAni >> tempStr;
	m_ifStreamForAni >> meshCount;

	for (int mesh = 0; mesh < meshCount; ++mesh)
	{
		m_ifStreamForMesh >> polygonCount;
		m_ifStreamForAni >> polygonCount;

		for (int i = 0; i < polygonCount; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				m_ifStreamForMesh >> vertex.x >> vertex.y >> vertex.z;
				vertex.x *= m_fModelScale.m128_f32[0];
				vertex.y *= m_fModelScale.m128_f32[1];
				vertex.z *= m_fModelScale.m128_f32[2];

				m_ifStreamForMesh >> UV.x >> UV.y;
				m_ifStreamForMesh >> normal.x >> normal.y >> normal.z;

				m_ifStreamForAni >> boneIndice.x >> boneIndice.y >> boneIndice.z >> boneIndice.w;
				m_ifStreamForAni >> weight.x >> weight.y >> weight.z;

				cbuffer tempBuffer = {
					vertex,
					normal,
					UV,
					boneIndice,
					weight
				};
				m_vVertexPositionColor.push_back(tempBuffer);
			}
		}
	}

	//XMMATRIX mtxGlobalBiped;
	//XMMATRIX mtxGlobalTransform;

	FbxAMatrix mtxGlobalBiped;
	FbxAMatrix mtxGlobalTransform;

	XMVECTOR mtxVector = {0, 0, 0, 0};
	FbxVector4 mtxVectorT;
	FbxVector4 mtxVectorS;
	FbxQuaternion mtxVectorQ;

	JointInfoData tempJointInfo;
	KeyFrameData tempKey;


	int keyFrames = 0;
	char str[30];
	double pValue = 0.0f;
	
	m_ifStreamForAni >> tempStr;
	m_ifStreamForAni >> m_aniStartTime >> m_aniEndTime >> m_jointCount;
	
	for (int jointIndex = 0; jointIndex < m_jointCount; ++jointIndex)
	{
		m_ifStreamForAni.getline(str, 50);
		m_ifStreamForAni.getline(str, 50);
		tempJointInfo.mName = str;
		m_ifStreamForAni >> tempJointInfo.mParentIndex  >> keyFrames;

		m_ifStreamForAni
			>> mtxVectorT.mData[0] >> mtxVectorT.mData[1] >> mtxVectorT.mData[2] >> mtxVectorT.mData[3]
			>> mtxVectorS.mData[0] >> mtxVectorS.mData[1] >> mtxVectorS.mData[2] >> mtxVectorS.mData[3]
			>> mtxVectorQ.mData[0] >> mtxVectorQ.mData[1] >> mtxVectorQ.mData[2] >> mtxVectorQ.mData[3];

		mtxGlobalBiped.SetT(mtxVectorT);
		mtxGlobalBiped.SetS(mtxVectorS);
		mtxGlobalBiped.SetQ(mtxVectorQ);

		//m_ifStreamForAni
		//	>> mtxVectorT.m128_f32[0] >> mtxVectorT.m128_f32[1] >> mtxVectorT.m128_f32[2] >> mtxVectorT.m128_f32[3]
		//	>> mtxVectorS.m128_f32[0] >> mtxVectorS.m128_f32[1] >> mtxVectorS.m128_f32[2] >> mtxVectorS.m128_f32[3]
		//	>> mtxVectorQ.m128_f32[0] >> mtxVectorQ.m128_f32[1] >> mtxVectorQ.m128_f32[2] >> mtxVectorQ.m128_f32[3];
		
		//mtxGlobalBiped = XMMatrixScalingFromVector(mtxVectorS) *
		//	XMMatrixRotationQuaternion(mtxVectorQ) *
		//	XMMatrixTranslationFromVector(mtxVectorT);

		//m_ifStreamForAni
		//	>>mtxGlobalBiped.r[0].m128_f32[0] >> mtxGlobalBiped.r[0].m128_f32[1] >> mtxGlobalBiped.r[0].m128_f32[2] >> mtxGlobalBiped.r[0].m128_f32[3]
		//	>>mtxGlobalBiped.r[1].m128_f32[0] >> mtxGlobalBiped.r[1].m128_f32[1] >> mtxGlobalBiped.r[1].m128_f32[2] >> mtxGlobalBiped.r[1].m128_f32[3]
		//	>>mtxGlobalBiped.r[2].m128_f32[0] >> mtxGlobalBiped.r[2].m128_f32[1] >> mtxGlobalBiped.r[2].m128_f32[2] >> mtxGlobalBiped.r[2].m128_f32[3]
		//	>>mtxGlobalBiped.r[3].m128_f32[0] >> mtxGlobalBiped.r[3].m128_f32[1] >> mtxGlobalBiped.r[3].m128_f32[2] >> mtxGlobalBiped.r[3].m128_f32[3];

		tempJointInfo.mGlobalBindposeInverse = mtxGlobalBiped;
		
		for (int i = 0; i < keyFrames; ++i)
		{
			m_ifStreamForAni >> tempKey.mFrameNum;

			//m_ifStreamForAni
			//	>> mtxGlobalTransform.r[0].m128_f32[0] >> mtxGlobalTransform.r[0].m128_f32[1] >> mtxGlobalTransform.r[0].m128_f32[2] >> mtxGlobalTransform.r[0].m128_f32[3]
			//	>> mtxGlobalTransform.r[1].m128_f32[0] >> mtxGlobalTransform.r[1].m128_f32[1] >> mtxGlobalTransform.r[1].m128_f32[2] >> mtxGlobalTransform.r[1].m128_f32[3]
			//	>> mtxGlobalTransform.r[2].m128_f32[0] >> mtxGlobalTransform.r[2].m128_f32[1] >> mtxGlobalTransform.r[2].m128_f32[2] >> mtxGlobalTransform.r[2].m128_f32[3]
			//	>> mtxGlobalTransform.r[3].m128_f32[0] >> mtxGlobalTransform.r[3].m128_f32[1] >> mtxGlobalTransform.r[3].m128_f32[2] >> mtxGlobalTransform.r[3].m128_f32[3];
			//
			//mtxGlobalTransform = mtxGlobalBiped * mtxGlobalTransform;
			
			m_ifStreamForAni
				>> mtxVectorT.mData[0] >> mtxVectorT.mData[1] >> mtxVectorT.mData[2] >> mtxVectorT.mData[3]
				>> mtxVectorS.mData[0] >> mtxVectorS.mData[1] >> mtxVectorS.mData[2] >> mtxVectorS.mData[3]
				>> mtxVectorQ.mData[0] >> mtxVectorQ.mData[1] >> mtxVectorQ.mData[2] >> mtxVectorQ.mData[3];

			mtxGlobalTransform.SetT(mtxVectorT);
			mtxGlobalTransform.SetS(mtxVectorS);
			mtxGlobalTransform.SetQ(mtxVectorQ);

			//m_ifStreamForAni
			//	>> mtxVectorT.m128_f32[0] >> mtxVectorT.m128_f32[1] >> mtxVectorT.m128_f32[2] >> mtxVectorT.m128_f32[3]
			//	>> mtxVectorS.m128_f32[0] >> mtxVectorS.m128_f32[1] >> mtxVectorS.m128_f32[2] >> mtxVectorS.m128_f32[3]
			//	>> mtxVectorQ.m128_f32[0] >> mtxVectorQ.m128_f32[1] >> mtxVectorQ.m128_f32[2] >> mtxVectorQ.m128_f32[3];
			
			//mtxGlobalTransform = XMMatrixScalingFromVector(mtxVectorS) *
			//	XMMatrixRotationQuaternion(mtxVectorQ) *
			//	XMMatrixTranslationFromVector(mtxVectorT);

			tempKey.mGlobalTransform = mtxGlobalTransform;

			tempJointInfo.mKeyFrames.push_back(tempKey);
		}
		m_skeltonInfo.m_vJoint.push_back(tempJointInfo);
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

void CFBXModelMesh::OnPrepareRender(ID3D11DeviceContext * pd3dDeviceContext)
{
	CMesh::OnPrepareRender(pd3dDeviceContext);

	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pcbBoneTransform.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);

	// 이 구간에서 상수버퍼로 들어갈 본 행렬을 정한다.
	// 애니메이션이 재생 중이라면, 키프레임 마다 저장되어있는 본 변환행렬을 끄집어 와야된다.

	GlobalMatrixBoneTransforms* gmtxbtf = (GlobalMatrixBoneTransforms*) d3dMappedResource.pData;
	string boneName[96];

	FbxAMatrix tempBoneTransforms;
	//XMMATRIX tempBoneTransforms;

	int jointCount = m_skeltonInfo.m_vJoint.size();

	// 2016_11_28 Add JD
	for (int jointIndex = 0; jointIndex < jointCount; ++jointIndex)
	{
		{
			//tempBoneTransforms = XMMatrixIdentity();
			
			tempBoneTransforms.SetIdentity();
			
			//TransformationOfPoseAtTimeT
			tempBoneTransforms
				*= m_fbxAniLoader->GetSkeletonJointGlobalTransform(jointIndex, (int)m_currentKeyFrameNum);
			
			////InverseOfGlobalBindPoseMatrix
			tempBoneTransforms
				*= m_fbxAniLoader->GetSkeletonJoint(jointIndex).mGlobalBindposeInverse;

			XMStoreFloat4x4(&(gmtxbtf->gmtxBoneTransforms[jointIndex]),
				XMMatrixTranspose(XMMATRIX(
				(float)tempBoneTransforms.Get(0, 0), (float)tempBoneTransforms.Get(0, 1), (float)tempBoneTransforms.Get(0, 2), (float)tempBoneTransforms.Get(0, 3),
				(float)tempBoneTransforms.Get(1, 0), (float)tempBoneTransforms.Get(1, 1), (float)tempBoneTransforms.Get(1, 2), (float)tempBoneTransforms.Get(1, 3),
				(float)tempBoneTransforms.Get(2, 0), (float)tempBoneTransforms.Get(2, 1), (float)tempBoneTransforms.Get(2, 2), (float)tempBoneTransforms.Get(2, 3),
				(float)tempBoneTransforms.Get(3, 0), (float)tempBoneTransforms.Get(3, 1), (float)tempBoneTransforms.Get(3, 2), (float)tempBoneTransforms.Get(3, 3))));

			//tempBoneTransforms = XMMatrixMultiply(tempBoneTransforms, m_skeltonInfo.m_vJoint[jointIndex].mKeyFrames[m_currentKeyFrameNum].mGlobalTransform);
			//tempBoneTransforms = XMMatrixMultiply(tempBoneTransforms, m_skeltonInfo.m_vJoint[jointIndex].mGlobalBindposeInverse);
		
			//tempBoneTransforms = m_skeltonInfo.m_vJoint[jointIndex].mKeyFrames[m_currentKeyFrameNum].mGlobalTransform2;
			
			//tempBoneTransforms *= (m_skeltonInfo.m_vJoint[jointIndex].mKeyFrames[m_currentKeyFrameNum].mGlobalTransform);
			//
			//tempBoneTransforms *= (m_skeltonInfo.m_vJoint[jointIndex].mGlobalBindposeInverse);
			
			boneName[jointIndex] = m_skeltonInfo.m_vJoint[jointIndex].mName;

			//tempBoneTransforms.r[0].m128_f32[3] *= 0.015;
			//tempBoneTransforms.r[1].m128_f32[3] *= 0.015;
			//tempBoneTransforms.r[2].m128_f32[3] *= 0.015;

			//XMStoreFloat4x4(&(gmtxbtf->gmtxBoneTransforms[jointIndex]), (tempBoneTransforms));

			//XMStoreFloat4x4(&(XMFLOAT4X4)(gmtxbtf->gmtxBoneTransforms[jointIndex]), XMMatrixIdentity());
		}
	}

	pd3dDeviceContext->Unmap(m_pcbBoneTransform.Get(), 0);
	pd3dDeviceContext->VSSetConstantBuffers(11, 1, m_pcbBoneTransform.GetAddressOf());
}

void CFBXModelMesh::Update(float fTimeElapsed)
{
	if (m_currentKeyFrameNum >= m_aniEndTime - 1)
		m_currentKeyFrameNum = m_aniStartTime;
	else
	{
		m_fTick += (30.0f * fTimeElapsed);
		if (m_fTick > 1.f)
		{
			m_currentKeyFrameNum++;
			m_fTick = 0.0f;
		}
	
		if (m_currentKeyFrameNum >= m_aniEndTime - 1)
			m_currentKeyFrameNum = m_aniEndTime - 1;
	}

	//if (m_currentKeyFrameNum >= m_fbxAniLoader->GetAnimEndTime() - 1)
	//{
	//	m_currentKeyFrameNum = m_fbxAniLoader->GetAnimStartTime();
	//}
	//else
	//{
	//	m_fTick += (30.0f * fTimeElapsed);
	//	//RSH 16'.04.16
	//	/*
	//	키프레임 수정
	//	*/
	//	if(m_fTick > 1.f)
	//	{
	//		m_currentKeyFrameNum++;
	//		m_fTick = 0.f;
	//	}
	//
	//	if (m_currentKeyFrameNum >= m_fbxAniLoader->GetAnimEndTime() - 1)
	//	{
	//		m_currentKeyFrameNum = m_fbxAniLoader->GetAnimEndTime() - 1;
	//	}
	//}

}
