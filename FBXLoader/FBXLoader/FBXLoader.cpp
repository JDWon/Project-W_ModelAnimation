#include "FBXLoader.h"


float modelScale = 0.005;


CFBXLoader::CFBXLoader(string filename, float modelScale)
{
	m_sFileName = filename;

	m_vMeshInfo.clear();

	CreateFBXScene(filename);
	ImportFBX(filename);

	//DirectX방식의 축으로 변환
	FbxAxisSystem OurAxisSystem = FbxAxisSystem::DirectX;

	FbxAxisSystem SceneAxisSystem = m_pFbxScene->GetGlobalSettings().GetAxisSystem();

	if (SceneAxisSystem != OurAxisSystem)
	{
		FbxAxisSystem::DirectX.ConvertScene(m_pFbxScene);
	}

	FbxSystemUnit SceneSystemUnit = m_pFbxScene->GetGlobalSettings().GetSystemUnit();
	if (SceneSystemUnit.GetScaleFactor() != 1.0)
	{
		FbxSystemUnit::cm.ConvertScene(m_pFbxScene);
	}

	FbxGeometryConverter GeomConverter(m_pFbxManager);
	GeomConverter.Triangulate(m_pFbxScene, true);
}

CFBXLoader::~CFBXLoader()
{
	m_vMeshInfo.clear();

	if (m_pFbxImporter)
	{
		m_pFbxImporter->Destroy();
		m_pFbxImporter = nullptr;
	}

	if (m_pFbxScene)
	{
		m_pFbxScene->Destroy();
		m_pFbxScene = nullptr;
	}

	if (m_pFbxManager)
	{
		m_pFbxManager->Destroy();
		m_pFbxManager = nullptr;
	}
}

void CFBXLoader::CreateFBXScene(string filename)
{
	m_pFbxManager = FbxManager::Create();
	m_pFbxIOSettion = FbxIOSettings::Create(m_pFbxManager, IOSROOT);

	m_pFbxManager->SetIOSettings(m_pFbxIOSettion);

	m_pFbxScene = FbxScene::Create(m_pFbxManager, "tempName");
}

void CFBXLoader::ImportFBX(string path)
{
	m_pFbxImporter = FbxImporter::Create(m_pFbxManager, "");
	if (!m_pFbxImporter->Initialize(path.c_str(), -1, m_pFbxManager->GetIOSettings()))
	{
		std::string ErrorTitle = "FBX import error!";
		GMessageBox(ErrorTitle, path);
	}

	m_pFbxImporter->Import(m_pFbxScene);
	m_pFbxImporter->Destroy();
}

void CFBXLoader::ProcessMeshInfo()
{
	FbxNode* rootNode = m_pFbxScene->GetRootNode();
	if (rootNode) { this->TraversalFBXTree_forMesh(rootNode); }
}

void CFBXLoader::ProcessMeshInfo(FbxScene* scene)
{
	FbxNode* rootNode = scene->GetRootNode();
	if (rootNode) { this->TraversalFBXTree_forMesh(rootNode); }
}

void CFBXLoader::TraversalFBXTree_forMesh(FbxNode* node)
{
	int numChild = node->GetChildCount();

	FbxNode *childNode = nullptr;
	FbxMesh *childMesh = nullptr;

	if (0 != m_vMeshInfo.size())
	{
		m_vMeshInfo.clear();
	}

	for (int i = 0; i < numChild; i++)
	{
		childNode = node->GetChild(i);

		childMesh = childNode->GetMesh();

		if (childMesh != nullptr)
		{

			MeshNodeInfo* tempMeshNodeInfo = new MeshNodeInfo();
			tempMeshNodeInfo->Release();

			tempMeshNodeInfo->nPolygonCount = childMesh->GetPolygonCount();

			//정점
			LoadVertexFromFbx(childMesh, tempMeshNodeInfo);

			//인덱스
			tempMeshNodeInfo->nIndicesCount = childMesh->GetPolygonVertexCount();
			for(int i = 0; i < tempMeshNodeInfo->nIndicesCount; ++i)
				tempMeshNodeInfo->vIndices.push_back(*(childMesh->GetPolygonVertices() + i));

			//노멀값
			LoadNormalFromFbx(childMesh, tempMeshNodeInfo);

			//텍스쳐 uv
			LoadUVFromFbx(childMesh, tempMeshNodeInfo);

			m_vMeshInfo.push_back(*tempMeshNodeInfo);

			delete tempMeshNodeInfo;
		}
	}
}

void CFBXLoader::LoadVertexFromFbx(FbxMesh *pMesh, MeshNodeInfo *pMeshModeInfo)
{
	int numVerts = pMesh->GetControlPointsCount();

	for (int i = 0; i < numVerts; ++i)
	{
		XMFLOAT3 currPosition;

		
		currPosition.x = static_cast<float>(pMesh->GetControlPointAt(i).mData[0] * modelScale);
		currPosition.y = static_cast<float>(pMesh->GetControlPointAt(i).mData[1] * modelScale);
		currPosition.z = static_cast<float>(pMesh->GetControlPointAt(i).mData[2] * modelScale);
		

		if (m_getfirstPos)
		{
			if (m_highestX < currPosition.x) { m_highestX = currPosition.x; }
			if (m_lowestX > currPosition.x) { m_lowestX = currPosition.x; }
			if (m_highestY < currPosition.y) { m_highestY = currPosition.y; }
			if (m_lowestY > currPosition.y) { m_lowestY = currPosition.y; }
			if (m_highestZ < currPosition.z) { m_highestZ = currPosition.z; }
			if (m_lowestZ > currPosition.z) { m_lowestZ = currPosition.z; }
		}
		else 
		{
			m_highestX = currPosition.x;
			m_lowestX = currPosition.x;

			m_highestY = currPosition.y;
			m_lowestY = currPosition.y;

			m_highestZ = currPosition.z;
			m_lowestZ = currPosition.z;
			m_getfirstPos = true;
		}

		pMeshModeInfo->position.push_back(currPosition);
	}
}

void CFBXLoader::LoadNormalFromFbx(FbxMesh *pMesh, MeshNodeInfo *pMeshModeInfo)
{
	FbxGeometryElementNormal* normalElement = pMesh->GetElementNormal();

	if (normalElement)
	{
		if (FbxGeometryElement::eByControlPoint == normalElement->GetMappingMode())
		{
			for (int vertexIndex = 0; vertexIndex < pMesh->GetControlPointsCount(); vertexIndex++)
			{
				int normalIndex = 0;

				//reference mode is direct, the normal index is same as vertex index.
				//get normals by the index of control vertex
				if (normalElement->GetReferenceMode() == FbxGeometryElement::eDirect)
					normalIndex = vertexIndex;

				//reference mode is index-to-direct, get normals by the index-to-direct
				if (normalElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
					normalIndex = normalElement->GetIndexArray().GetAt(vertexIndex);

				//Got normals of each vertex.
				FbxVector4 getNormal = normalElement->GetDirectArray().GetAt(normalIndex);
				pMeshModeInfo->normal.push_back(XMFLOAT3((float)getNormal[0], (float)getNormal[1], (float)getNormal[2]));
			}
		}
		else if (FbxGeometryElement::eByPolygonVertex == normalElement->GetMappingMode())
		{
			int indexByPolygonVertex = 0;

			//Let's get normals of each polygon, since the mapping mode of normal element is by polygon-vertex.
			for (int polygonIndex = 0; polygonIndex < pMeshModeInfo->nPolygonCount; polygonIndex++)
			{
				//get polygon size, you know how many vertices in current polygon.
				int polygonSize = pMesh->GetPolygonSize(polygonIndex);

				//retrieve each vertex of current polygon.
				for (int i = 0; i < polygonSize; i++)
				{
					int lNormalIndex = 0;
					//reference mode is direct, the normal index is same as indexByPolygonVertex.
					if (normalElement->GetReferenceMode() == FbxGeometryElement::eDirect)
						lNormalIndex = indexByPolygonVertex;

					//reference mode is index-to-direct, get normals by the index-to-direct
					if (normalElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
						lNormalIndex = normalElement->GetIndexArray().GetAt(indexByPolygonVertex);

					//Got normals of each polygon-vertex.
					FbxVector4 getNormal = normalElement->GetDirectArray().GetAt(lNormalIndex);
					pMeshModeInfo->normal.push_back(XMFLOAT3((float)getNormal[0], (float)getNormal[1], (float)getNormal[2]));

					indexByPolygonVertex++;
				}
			}
		}
	}
}

void CFBXLoader::LoadUVFromFbx(FbxMesh *pMesh, MeshNodeInfo *pMeshModeInfo)
{
	//UV 이름 리스트를 가져온다. 이름은 UV Channel 1 식으로 나열.
	FbxStringList lUVSetNameList;
	pMesh->GetUVSetNames(lUVSetNameList);

	//iterating over all uv sets
	for (int lUVSetIndex = 0; lUVSetIndex < lUVSetNameList.GetCount(); lUVSetIndex++)
	{
		//get lUVSetIndex-th uv set
		const char* lUVSetName = lUVSetNameList.GetStringAt(lUVSetIndex);
		const FbxGeometryElementUV* lUVElement = pMesh->GetElementUV(lUVSetName);

		if (!lUVElement)
			continue;

		// only support mapping mode eByPolygonVertex and eByControlPoint
		if (lUVElement->GetMappingMode() != FbxGeometryElement::eByPolygonVertex &&
			lUVElement->GetMappingMode() != FbxGeometryElement::eByControlPoint)
			return;

		//index array, where holds the index referenced to the uv data
		const bool lUseIndex = lUVElement->GetReferenceMode() != FbxGeometryElement::eDirect;
		const int lIndexCount = (lUseIndex) ? lUVElement->GetIndexArray().GetCount() : 0;

		//iterating through the data by polygon
		const int lPolyCount = pMesh->GetPolygonCount();

		if (lUVElement->GetMappingMode() == FbxGeometryElement::eByControlPoint)
		{
			for (int lPolyIndex = 0; lPolyIndex < lPolyCount; ++lPolyIndex)
			{
				// build the max index array that we need to pass into MakePoly
				const int lPolySize = pMesh->GetPolygonSize(lPolyIndex);
				for (int lVertIndex = 0; lVertIndex < lPolySize; ++lVertIndex)
				{
					FbxVector2 lUVValue;

					//get the index of the current vertex in control points array
					int lPolyVertIndex = pMesh->GetPolygonVertex(lPolyIndex, lVertIndex);

					//the UV index depends on the reference mode
					int lUVIndex = lUseIndex ? lUVElement->GetIndexArray().GetAt(lPolyVertIndex) : lPolyVertIndex;

					lUVValue = lUVElement->GetDirectArray().GetAt(lUVIndex);

					float dataU = 0, dataV = 0;

					if ((float)lUVValue.mData[0] > 1.0f)
						dataU = (float)lUVValue.mData[0] - (int)lUVValue.mData[0];
					else if ((float)lUVValue.mData[0] < 0.0f)
						dataU = 1.0f - ((float)lUVValue.mData[0] - (int)lUVValue.mData[0]);
					else
						dataU = (float)lUVValue.mData[0];

					if ((float)lUVValue.mData[1] > 1.0f)
						dataV = (float)lUVValue.mData[1] - (int)lUVValue.mData[1];
					else if ((float)lUVValue.mData[1] < 0.0f)
						dataV = 1.0f - ((float)lUVValue.mData[1] - (int)lUVValue.mData[1]);
					else
						dataV = (float)lUVValue.mData[1];

					pMeshModeInfo->uv.push_back(XMFLOAT2(dataU, dataV));
				}
			}
		}
		else if (lUVElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
		{
			int lPolyIndexCounter = 0;
			for (int lPolyIndex = 0; lPolyIndex < lPolyCount; ++lPolyIndex)
			{
				// build the max index array that we need to pass into MakePoly
				const int lPolySize = pMesh->GetPolygonSize(lPolyIndex);
				for (int lVertIndex = 0; lVertIndex < lPolySize; ++lVertIndex)
				{
					if (lPolyIndexCounter < lIndexCount)
					{
						FbxVector2 lUVValue;

						//the UV index depends on the reference mode
						int lUVIndex = lUseIndex ? lUVElement->GetIndexArray().GetAt(lPolyIndexCounter) : lPolyIndexCounter;

						lUVValue = lUVElement->GetDirectArray().GetAt(lUVIndex);

						float dataU = 0, dataV = 0;

						if ((float)lUVValue.mData[0] > 1.0f)
							dataU = (float)lUVValue.mData[0] - (int)lUVValue.mData[0];
						else if ((float)lUVValue.mData[0] < 0.0f)
							dataU = 1.0f + ((float)lUVValue.mData[0] - (int)lUVValue.mData[0]);
						else
							dataU = (float)lUVValue.mData[0];

						if ((float)lUVValue.mData[1] > 1.0f)
							dataV = (float)lUVValue.mData[1] - (int)lUVValue.mData[1];
						else if ((float)lUVValue.mData[1] < 0.0f)
							dataV = 1.0f + ((float)lUVValue.mData[1] - (int)lUVValue.mData[1]);
						else
							dataV = (float)lUVValue.mData[1];

						pMeshModeInfo->uv.push_back(XMFLOAT2((float)dataU, (float)dataV));

						lPolyIndexCounter++;
					}
				}
			}
		}
	}
}


CFBXAnimationLoader::CFBXAnimationLoader(std::string filename, float modelScale, bool isVoxel)
	: CFBXLoader(filename, modelScale)
{
	//애니메이션 스택명 저장
	m_pFbxScene->FillAnimStackNameArray(m_animStackNameArray);

	//AnimationLayerData animdata;

	FbxAnimStack* currAnimStack = m_pFbxScene->GetSrcObject<FbxAnimStack>(0);
	FbxString animStackName = currAnimStack->GetName();
	FbxTakeInfo* takeInfo = m_pFbxScene->GetTakeInfo(animStackName);
	m_animTimeStart = takeInfo->mLocalTimeSpan.GetStart();
	m_animTimeEnd = takeInfo->mLocalTimeSpan.GetStop();
	m_animTimeLength = m_animTimeEnd.GetFrameCount(FPSFORFBX)
		- (m_animTimeStart.GetFrameCount(FPSFORFBX) + 1);

	m_path = filename;
}

CFBXAnimationLoader::~CFBXAnimationLoader()
{
	CFBXLoader::~CFBXLoader();
}

void CFBXAnimationLoader::ProcessAnimationInfo()
{
	// 본(Bone) 계층구조
	ProcessSkeletonHierarchy(m_pFbxScene->GetRootNode());
	FbxNode* rootNode = m_pFbxScene->GetRootNode();
	if (rootNode) {
		// 2016.10.27 error02
		SortBoneName(rootNode);
		this->TraversalFBXTree_forAnimation(rootNode);
	}
}

void CFBXAnimationLoader::SortBoneName(FbxNode* inNode)
{
	int numChild = inNode->GetChildCount();
	FbxNode *childNode = nullptr;

	std::vector<std::pair<int, std::string>> namelist;

	decltype(namelist)::iterator iter_insert = namelist.end();
	for (int i = 0; i < numChild; i++)
	{
		childNode = inNode->GetChild(i);
		FbxMesh *childMesh = childNode->GetMesh();

		if (childMesh != nullptr)
		{
			FbxSkin* skin = (FbxSkin*)childNode->GetMesh()->GetDeformer(0, FbxDeformer::eSkin);
			if (skin != nullptr)
			{
				int boneCount = skin->GetClusterCount();

				for (int boneIndex = 0; boneIndex < boneCount; ++boneIndex)
				{
					FbxCluster* cluster = skin->GetCluster(boneIndex);
					std::string currJointName = cluster->GetLink()->GetName();
					int currJointIndex = FindJointIndexUsingName(currJointName);
					auto itor = find_if(namelist.begin(), namelist.end(), [&](const std::pair<int, std::string>& p) {
						return p.second == currJointName;
					});
					if (itor == namelist.end())
					{
						iter_insert = namelist.insert(iter_insert, make_pair(currJointIndex, currJointName));
						++iter_insert;
					}
					else iter_insert = itor;

//					namelist.push_back( make_pair(currJointIndex, currJointName));
				}
			}

		}
	}

	std::vector<JointInfo> tmpJoints;
	for (auto p : namelist)
	{
		auto fndnode = find_if(m_Skeleton.m_vJoint.begin(), m_Skeleton.m_vJoint.end(), [&](const auto& j) {
			return (j.mName == p.second);
		});
		if (fndnode != m_Skeleton.m_vJoint.end())
			tmpJoints.push_back(move(*fndnode));
	}

	m_Skeleton.m_vJoint.swap(tmpJoints);
}

void CFBXAnimationLoader::ProcessSkeletonHierarchy(FbxNode* inRootNode)
{
	for (int childIndex = 0; childIndex < inRootNode->GetChildCount(); ++childIndex)
	{
		FbxNode* currNode = inRootNode->GetChild(childIndex);

		// 본(Bone)의 최상위의 인덱스는 -1 단계별로 뻗어갈 때보다 +1
		ProcessSkeletonHierarchyRecursively(currNode, 0, 0, -1);
	}
}

void CFBXAnimationLoader::ProcessSkeletonHierarchyRecursively(FbxNode* inNode, int inDepth, int myIndex, int inParentIndex)
{
	if (inNode->GetNodeAttribute()
		&& inNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
		JointInfo currJoint;
		currJoint.mParentIndex = inParentIndex;
		currJoint.mName = inNode->GetName();
		m_Skeleton.m_vJoint.push_back(currJoint);
	}
	for (int i = 0; i < inNode->GetChildCount(); i++)
		ProcessSkeletonHierarchyRecursively(inNode->GetChild(i), inDepth + 1, m_Skeleton.m_vJoint.size(), myIndex);
}

void CFBXAnimationLoader::TraversalFBXTree_forAnimation(FbxNode* node)
{
	int numChild = node->GetChildCount();
	FbxNode *childNode = nullptr;

	for (int i = 0; i < numChild; i++)
	{
		childNode = node->GetChild(i);
		FbxMesh *childMesh = childNode->GetMesh();

		if (childMesh != nullptr)
		{
			if (0 == m_vMeshInfo.size())
			{
				//20160404 애니메이션 정보에서 필요한 정보.
				//메쉬 정보를 얻지 않았을 경우에도 필요하다.
				//메쉬 정보가 없으면 인덱스 값만 추출한다.
				MeshNodeInfo* tempMeshNodeInfo = new MeshNodeInfo();
				tempMeshNodeInfo->nIndicesCount = childMesh->GetPolygonVertexCount();
				tempMeshNodeInfo->pIndices = new int[tempMeshNodeInfo->nIndicesCount];
				tempMeshNodeInfo->pIndices = childMesh->GetPolygonVertices();

				m_vMeshInfo.push_back(*tempMeshNodeInfo);
				delete tempMeshNodeInfo;
			}

			//LoadControlPoints(childNode);
			//LoadSkeletalInfo(childMesh, geometryTransform);
			
			LoadSkeletalInfo(childNode);
	
		}
	}
}

void CFBXAnimationLoader::LoadControlPoints(FbxNode* inNode)
{
	FbxMesh* currMesh = inNode->GetMesh();
	unsigned int ctrlPointCount = currMesh->GetControlPointsCount();
	for (unsigned int i = 0; i < ctrlPointCount; ++i)
	{
		XMFLOAT3 currPosition;
		currPosition.x = static_cast<float>(currMesh->GetControlPointAt(i).mData[0]);
		currPosition.y = static_cast<float>(currMesh->GetControlPointAt(i).mData[1]);
		currPosition.z = static_cast<float>(currMesh->GetControlPointAt(i).mData[2]);

		//m_blendingInfo[i].m_position = currPosition;
	}
}


std::vector<std::pair<unsigned int, std::string>>  CFBXAnimationLoader::LoadSkeletalInfo(FbxNode* inNode)
{
	//int numDeformers = fbxMesh->GetDeformerCount();
	std::vector<std::pair<unsigned int, std::string>> namelist;

	FbxSkin* skin = (FbxSkin*)inNode->GetMesh()->GetDeformer(0, FbxDeformer::eSkin);
	if (skin != nullptr)
	{
		int boneCount = skin->GetClusterCount();


		for (int boneIndex = 0; boneIndex < boneCount; ++boneIndex)
		{
			FbxCluster* cluster = skin->GetCluster(boneIndex);
			std::string currJointName = cluster->GetLink()->GetName();
			unsigned int currJointIndex = FindJointIndexUsingName(currJointName);

			namelist.push_back(make_pair(currJointIndex, currJointName));

			FbxAMatrix transformMatrix;
			FbxAMatrix transformLinkMatrix;			//부모의 월드변환행렬
			FbxAMatrix globalBindposeInverseMatrix;	//본 기준위치에 역행렬

			FbxNode* boneNode = cluster->GetLink();

			//bind pose(본의 기준위치)의 역행렬(globalBindposeInverseMatrix) 얻기
			cluster->GetTransformMatrix(transformMatrix);
			cluster->GetTransformLinkMatrix(transformLinkMatrix);

			//GetGeometryTransformation(inNode) : 노드의 지역행렬.
			globalBindposeInverseMatrix
				= transformLinkMatrix.Inverse() * transformMatrix * GetGeometryTransformation(inNode);

			globalBindposeInverseMatrix.SetT(
				FbxVector4(
					globalBindposeInverseMatrix.GetT().mData[0] * modelScale,
					globalBindposeInverseMatrix.GetT().mData[1] * modelScale,
					globalBindposeInverseMatrix.GetT().mData[2] * modelScale));

			FbxAMatrix a = GetGeometryTransformation(inNode);

			// Update the information in mSkeleton 
			m_Skeleton.m_vJoint[currJointIndex].mGlobalBindposeInverse
				= globalBindposeInverseMatrix;
			m_Skeleton.m_vJoint[currJointIndex].mNode
				= cluster->GetLink();

			//본 정점들의 인덱스 배열
			int *boneVertexIndices = cluster->GetControlPointIndices();
			//본 정점들의 Weight값 배열
			double *boneVertexWeights = cluster->GetControlPointWeights();

			// 본에 영향을 미치는 정점들을 갱신(스키닝)
			int numBoneVertexIndices = cluster->GetControlPointIndicesCount();

			for (int i = 0; i < numBoneVertexIndices; i++)
			{
				//20160317
				m_blendingInfo[boneVertexIndices[i]].mInfo
					.push_back(std::make_pair(currJointIndex, (float)cluster->GetControlPointWeights()[i]));
			}

			//매 프레임 당 애니메이션 변환행렬 추출
			for (FbxLongLong frameCount = m_animTimeStart.GetFrameCount(FPSFORFBX); frameCount <= m_animTimeEnd.GetFrameCount(FPSFORFBX); ++frameCount)
			{
				FbxTime currentTime;
				currentTime.SetFrame(frameCount, FPSFORFBX);

				KeyFrame tempKeyFrame;
				tempKeyFrame.mFrameNum = (int)frameCount;

				FbxAMatrix currnetTransformOffset = inNode->EvaluateGlobalTransform(currentTime) *  GetGeometryTransformation(inNode);
				tempKeyFrame.mGlobalTransform
					= currnetTransformOffset.Inverse() * cluster->GetLink()->EvaluateGlobalTransform(currentTime);

				tempKeyFrame.mGlobalTransform.SetT(
					FbxVector4(tempKeyFrame.mGlobalTransform.GetT().mData[0] * modelScale,
						tempKeyFrame.mGlobalTransform.GetT().mData[1] * modelScale,
						tempKeyFrame.mGlobalTransform.GetT().mData[2] * modelScale));

				m_Skeleton.m_vJoint[boneIndex].mKeyFrames.push_back(tempKeyFrame);
			}
		}

	}

	return namelist;
}


//노드의 지역행렬을 구한다. 
FbxAMatrix CFBXAnimationLoader::GetGeometryTransformation(FbxNode* inNode)
{
	if (!inNode)
	{
		throw std::exception("Null for mesh geometry");
	}

	const FbxVector4 lT = inNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 lR = inNode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 lS = inNode->GetGeometricScaling(FbxNode::eSourcePivot);

	return FbxAMatrix(lT, lR, lS);
}

unsigned int CFBXAnimationLoader::FindJointIndexUsingName(std::string jointName)
{
	for (unsigned int i = 0; i < m_Skeleton.m_vJoint.size(); i++)
	{
		if (m_Skeleton.m_vJoint[i].mName == jointName)
			return i;
	}
	return 0;
}

FbxAMatrix CFBXAnimationLoader::GetSkeletonJointGlobalTransform(unsigned int jointIndex, int keyframeNum)
{
	if (GetSkeletonJoint(jointIndex).mKeyFrames.size() != 0)
		return GetSkeletonJoint(jointIndex).mKeyFrames[keyframeNum].mGlobalTransform;
	else
	{
		FbxAMatrix m;
		m.SetIdentity();
		return m;
	}
}

bool CFBXAnimationLoader::HasAnitamionKeyframe()
{
	if (m_Skeleton.m_vJoint.cbegin()->mKeyFrames.size() != 0)
		return true;
	else
		return false;
}