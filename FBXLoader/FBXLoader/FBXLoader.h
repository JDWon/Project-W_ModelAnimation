#pragma once

#pragma warning(disable:4100)
#pragma warning(disable:4324)

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#include <windows.h>
// Windows Resource 파일:


// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <assert.h>

// D3D11
#include <d3d11_2.h>
#include <dxgi1_3.h>

// D2D1
#include <d2d1_3.h>
#include <dwrite_3.h>
#include <d2d1_2helper.h>
#include <wincodec.h>

// DirectX Math
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>

// C++11
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <algorithm>
#include <functional>
#include <memory>
#include <vector>
#include <list>
#include <deque>
#include <map>
#include <set>
#include <unordered_map>

#include "FBXSDK\Include\fbxsdk.h"

#define FPSFORFBX FbxTime::eFrames60

using namespace std;
using namespace std::chrono;

using namespace D2D1;
using namespace DirectX;
using namespace DirectX::PackedVector;

inline int GMessageBox(string _title, string _message)
{
	return MessageBoxA(nullptr, _message.c_str(), _title.c_str(), MB_OK);
}

inline int GMessageBox(TCHAR* title, TCHAR* message)
{
	return MessageBox(nullptr, message, title, MB_OK);
}



struct BlendingIndexWeightPair
{
	//FBX_DATATYPE3 	   m_position;	//위치값

	//BlendingIndexWeightPair
	std::vector<std::pair<unsigned int, float>>			mInfo;

	BlendingIndexWeightPair()
	{
		mInfo.reserve(4);
	}
};

typedef struct MeshNodeInfo
{
	// Node name, Parent node name
	string name;
	string parentName;

	// Mesh position, normal, uv vector
	vector<XMFLOAT3> position;
	vector<XMFLOAT3> normal;
	vector<XMFLOAT2> uv;

	vector<int>	vIndices;
	int *pIndices;
	int nIndicesCount;
	int nPolygonCount;

	void Release()
	{
		position.clear();
		normal.clear();
		uv.clear();
		pIndices = nullptr;
		nIndicesCount = 0;
		nPolygonCount = 0;
	}
}MeshNodeInfo;

typedef struct KeyFrame
{
	FbxLongLong		mFrameNum;
	FbxAMatrix		mGlobalTransform;
}KeyFrame;

typedef struct JointInfo
{
	int mParentIndex;
	std::string mName;

	FbxAMatrix mGlobalBindposeInverse;
	FbxNode* mNode;

	bool	mHasBonedata;

	std::vector<KeyFrame> mKeyFrames;

	JointInfo() : mNode(nullptr)
	{
		mGlobalBindposeInverse.SetIdentity();
		mParentIndex = -1;
		mKeyFrames.clear();
	}

	~JointInfo()
	{
		mKeyFrames.clear();
	}
	
}JointInfo;

typedef struct SkeltonInfo
{
	std::vector<JointInfo> m_vJoint;
}SkeltonInfo;

class CFBXLoader
{
protected :
	FbxManager					*m_pFbxManager;
	FbxScene					*m_pFbxScene;
	FbxIOSettings				*m_pFbxIOSettion;
	FbxImporter					*m_pFbxImporter;

	vector<MeshNodeInfo>		m_vMeshInfo;

	std::string					m_sFileName;

	bool						m_getfirstPos;
	float						m_highestX;
	float						m_lowestX;
	float						m_highestY;
	float						m_lowestY;
	float						m_highestZ;
	float						m_lowestZ;

public :
	CFBXLoader(string filename, float modelScale);
	~CFBXLoader();

	void CreateFBXScene(string filename);
	void ImportFBX(string path);

	virtual void ProcessMeshInfo();
	virtual void ProcessMeshInfo(FbxScene*);
	virtual void TraversalFBXTree_forMesh(FbxNode* node);
	
	void LoadVertexFromFbx(FbxMesh *pMesh, MeshNodeInfo *pMeshModeInfo);
	void LoadNormalFromFbx(FbxMesh *pMesh, MeshNodeInfo *pMeshModeInfo);
	void LoadUVFromFbx(FbxMesh *pMesh, MeshNodeInfo *pMeshModeInfo);

	vector<MeshNodeInfo>& getVMeshInfo() { return m_vMeshInfo; }

	XMFLOAT3 GetVertices(int meshIndex, int index) { return m_vMeshInfo[meshIndex].position[index]; }
	XMFLOAT3 GetNormals(int meshIndex, int index) { return m_vMeshInfo[meshIndex].normal[index]; }
	XMFLOAT2 GetTextureCoords(int meshIndex, int index) { return m_vMeshInfo[meshIndex].uv[index]; }
	vector<int>& GetVectorIndices(int meshIndex = 0) { return m_vMeshInfo[meshIndex].vIndices; }

	int* GetIndices(int meshIndex = 0) { return m_vMeshInfo[meshIndex].pIndices; }
	int GetIndiecesCount(int meshIndex = 0) { return m_vMeshInfo[meshIndex].nIndicesCount; }
	int GetPolygonCount(int meshIndex = 0) { return m_vMeshInfo[meshIndex].nPolygonCount; }
	int GetMeshCount() { return m_vMeshInfo.size(); };

	float GetHighestX() { return m_highestX; };
	float GetLowestX() { return m_lowestX; };
	float GetHighestY() { return m_highestY; };
	float GetLowestY() { return m_lowestY; };
	float GetHighestZ() { return m_highestZ; };
	float GetLowestZ() { return m_lowestZ; };
};

class CFBXAnimationLoader : public CFBXLoader
{
public:
	CFBXAnimationLoader(std::string filename, float modelScale = 1.0f, bool isVoxel = false);
	~CFBXAnimationLoader();

	virtual void ProcessAnimationInfo();
	virtual void TraversalFBXTree_forAnimation(FbxNode* node);

	void ProcessSkeletonHierarchy(FbxNode* inRootNode);
	void ProcessSkeletonHierarchyRecursively(FbxNode* inNode, int inDepth, int myIndex, int inParentIndex);

	void LoadControlPoints(FbxNode* inNode);
	//void LoadSkeletalInfo(FbxMesh*, FbxAMatrix geometryTransform);
	std::vector<std::pair<unsigned int, std::string>>  LoadSkeletalInfo(FbxNode* inNode);

	int GetAnimStartTime(int keyframeLayer = 0) { return (int)m_animTimeStart.GetFrameCount(FPSFORFBX); }
	int GetAnimEndTime(int keyframeLayer = 0) { return (int)m_animTimeEnd.GetFrameCount(FPSFORFBX); }
	FbxTime GetAnimTimeLength(int keyframeLayer = 0) { return m_animTimeLength; }

	FbxAMatrix GetGeometryTransformation(FbxNode* inNode);
	unsigned int FindJointIndexUsingName(std::string jointName);

	std::unordered_map<unsigned int, BlendingIndexWeightPair> GetBlendingInfo() { return m_blendingInfo; }
	std::vector<std::pair<unsigned int, float>> GetBlendingIndexWeightPair(int index) { return m_blendingInfo[index].mInfo; }
	SkeltonInfo GetSkeleton() { return m_Skeleton; }
	JointInfo GetSkeletonJoint(unsigned int boneindex) { return m_Skeleton.m_vJoint[boneindex]; }
	size_t GetSkeletonJointCount() { return m_Skeleton.m_vJoint.size(); }
	FbxAMatrix GetSkeletonJointGlobalTransform(unsigned int jointIndex, int keyframeNum);

	bool HasAnitamionKeyframe();

	// 2016.10.27 error3
	void SortBoneName(FbxNode* inNode);

private:

	std::string			m_path;
	mutable FbxTime		m_animTimeLength;
	mutable FbxTime		m_animTimeStart;
	mutable FbxTime		m_animTimeEnd;

	FbxNode												*m_processedNode;
	//FbxAnimLayer										*m_currentAnimLayer;
	FbxArray<FbxString*>								m_animStackNameArray;

	SkeltonInfo										m_Skeleton;


	std::unordered_map<unsigned int, BlendingIndexWeightPair> m_blendingInfo;
};