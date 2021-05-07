#include "framework.h"
#include "Object.h"
#include "PhysXMgr.h"
#include "MeshMgr.h"

CPhysXMgr* CPhysXMgr::m_pInstance = NULL;

CPhysXMgr::CPhysXMgr()
{
}


CPhysXMgr::~CPhysXMgr()
{
	VecPGO.clear();
	PDynamiclist.clear();
	PStaticlist.clear();
	if (mControllerManager != nullptr)
	{
		mControllerManager->purgeControllers();
		mControllerManager->release();
	}

	gScene->release();
	gDispatcher->release();

	gCooking->release(); //추가

	gPhysics->release();
	PxPvdTransport* transport = gPvd->getTransport();
	gPvd->release();
	transport->release();

	gFoundation->release();
}




//void CPhysXMgr::CheckCollsionPhysX(void(*pPred)(PxRigidDynamic* dy), PxRigidDynamic* pDynamic)
//{
//	const PxU32 sActorBufferSize = 100;
//	PxActor* actors[sActorBufferSize];
//	PxU32 number = CPhysXMgr::GetInstance()->gScene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC, actors, sActorBufferSize);
//
//	if (number > 0)
//	{
//		for (PxU32 i = 0; i < number; ++i)
//		{
//			PxRigidDynamic* RD = nullptr;
//			RD = actors[i]->is<PxRigidDynamic>();
//
//			if (RD != nullptr)
//			{
//				PxVec3 pos = RD->getGlobalPose().p;
//
//				if (RD == pDynamic)
//					continue;
//
//
//				if (CPhysXMgr::GetInstance()->SweepBetweenTwoObject(RD, pDynamic))
//				{
//					pPred(RD);
//				}
//
//			}
//		}
//	}
//}




bool CPhysXMgr::Initialize()
{
	const char* strTransport = "127.0.0.1";

	gFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gAllocator, gErrorCallback);

	gPvd = PxCreatePvd(*gFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(strTransport, 5425, 10);
	gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	PxTolerancesScale scale;

	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);

	PxCookingParams cookingParams(gPhysics->getTolerancesScale());
	gCooking = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, cookingParams);

	
	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -40.f, 0.0f); // 중력
	gDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	gScene = gPhysics->createScene(sceneDesc);
	
	
	PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	//0.6
	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.1f); // // 정적마찰계수, 동적마찰계수 ,  반발계수(튕기는힘),
	mControllerManager = PxCreateControllerManager(*gScene);
	PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, PxPlane(0, 1, 0, 0), *gMaterial);
	gScene->addActor(*groundPlane);


	return false;
}

//PxRigidDynamic* CPhysXMgr::CreateSphere(CPhysXObject* pObj, _vec3 Pos, float Radius, const PxVec3& velocity, PxMaterial* Material_)
//{	
//	const PxTransform& t = PxTransform(PxVec3(Pos.x, Pos.y, Pos.z));
//
//	if (Material_ == nullptr)
//		Material_ = gMaterial;
//
//	PxRigidDynamic* body = PxCreateDynamic(*gPhysics, t, PxSphereGeometry(Radius), *Material_, 5.0f);
//	body->setAngularDamping(0.5f);
//	body->setLinearDamping(0.5f);
//	body->setLinearVelocity(velocity); //객체의 선형 속도
//	//dynamic->setMass(1.f);
//	gScene->addActor(*body);
//	
//	MyPhysXGameObject str;
//	str.pObject = pObj;
//	str.pRigidDynamic = body;
//	VecPGO.push_back(str);
//	
//	PDynamiclist.push_back(body);
//
//	body->setName("");
//
//	return body;
//}
//
//
//
//PxRigidDynamic* CPhysXMgr::CreateBox(CPhysXObject* pObj, _vec3 Pos, PxReal x, PxReal y, PxReal z, PxMaterial* Material_) // 가로길이가 1.f일경우 x는 0.5f다
//{
//
//	const PxTransform& t = PxTransform(PxVec3(Pos.x, Pos.y, Pos.z));
//			
//	if (Material_ == nullptr)
//		Material_ = gMaterial;
//	
//
//	PxShape* shape = gPhysics->createShape(PxBoxGeometry(x, y, z), *Material_);
//	
//	PxTransform localTm(PxVec3(0, 0, 0) * x);
//	PxRigidDynamic* body = gPhysics->createRigidDynamic(t.transform(localTm));
//	body->attachShape(*shape);
//	PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
//	gScene->addActor(*body);
//	//body->setMass(1.f);
//	
//	shape->release();
//
//	MyPhysXGameObject str;
//	str.pObject = pObj;
//	str.pRigidDynamic = body;
//	VecPGO.push_back(str);
//
//	PDynamiclist.push_back(body);
//
//	body->setName("");
//
//	return body;
//}
//
//PxRigidStatic* CPhysXMgr::CreateStaticBox(CPhysXObject* pObj, _vec3 Pos, PxReal x, PxReal y, PxReal z)
//{
//	const PxTransform& t = PxTransform(PxVec3(Pos.x, Pos.y, Pos.z));
//
//
//	PxTransform localTm(PxVec3(0, 0, 0) * x);
//	PxRigidStatic* body = gPhysics->createRigidStatic(t.transform(localTm));
//	PxShape* shape = gPhysics->createShape(PxBoxGeometry(x, y, z), *gMaterial);
//
//	body->attachShape(*shape);	
//	gScene->addActor(*body);
//	shape->release();
//
//	MyPhysXGameObject str;
//	str.pObject = pObj;
//	str.pRigidStatic = body;
//	VecPGO.push_back(str);
//
//	PStaticlist.push_back(body);
//
//	body->setName("");
//
//	return body;
//
//}
//
////현재안쓰는중
//PxRigidStatic* CPhysXMgr::CreateHexahedron(CPhysXObject* pObj, Engine::CCubeColor* pBuffer, Engine::VTXCOLORCUBE* pVertex, Engine::INDEX16* pIndex, _vec3 ObjectPos)
//{
//	PxRigidStatic* iglooActor = gPhysics->createRigidStatic(PxTransform(PxVec3(0, 0, 0)));
//	
//	PxVec3 VectexPos[8];
//
//	for (_ulong i = 0; i < pBuffer->Get_VtxCnt(); ++i)
//	{
//		VectexPos[i].x = pVertex[i].vPos.x + ObjectPos.x;
//		VectexPos[i].y = pVertex[i].vPos.y + ObjectPos.y;
//		VectexPos[i].z = pVertex[i].vPos.z + ObjectPos.z;
//	}
//
//	Engine::Triangle indices[12];
//
//	
//
//	for (_ulong i = 0; i < pBuffer->Get_TriCnt(); ++i)
//	{
//		indices[i].ind0 = pIndex[i]._0;
//		indices[i].ind1 = pIndex[i]._1;
//		indices[i].ind2 = pIndex[i]._2;
//	}
//
//	PxTriangleMeshDesc meshDesc;
//	meshDesc.points.count = pBuffer->Get_VtxCnt();
//	meshDesc.points.stride = sizeof(PxVec3);
//	meshDesc.points.data = VectexPos;
//
//	meshDesc.triangles.count = pBuffer->Get_TriCnt();
//	meshDesc.triangles.stride = sizeof(Engine::Triangle);
//	meshDesc.triangles.data = indices;
//
//
//
//	PxDefaultMemoryOutputStream writeBuffer;
//	PxTriangleMeshCookingResult::Enum result;
//	bool status = gCooking->cookTriangleMesh(meshDesc, writeBuffer, &result);
//	
//	if (!status)
//		return nullptr;
//
//	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
//
//	PxTriangleMesh* triMesh = gPhysics->createTriangleMesh(readBuffer);
//
//
//
//	if (triMesh == NULL)
//		return nullptr;
//
//
//	PxTriangleMeshGeometry geom(triMesh);
//	PxShape* iglooShape = PxRigidActorExt::createExclusiveShape(*iglooActor, geom, *gMaterial);
//
//
//	gScene->addActor(*iglooActor);
//
//	MyPhysXGameObject str;
//	str.pObject = pObj;
//	str.pRigidStatic = iglooActor;
//	VecPGO.push_back(str);
//
//	iglooActor->setName("");
//
//	return iglooActor;
//	
//}
//
//
//
//
PxRigidStatic * CPhysXMgr::CreateTriangleStaticMesh(Object* pObj, string meshname, XMMATRIX matWorld)
{
	MeshMgr* mgr = MeshMgr::GetInstnace();
	int cnt = mgr->GetMeshVtxInfo(meshname).ulVertexCnt;
	vector<XMFLOAT3> pos = mgr->GetMeshVtxInfo(meshname).vecVtxPos;
	vector<Indices> idx = mgr->GetMeshVtxInfo(meshname).vecIdx;
	if(cnt == 0)
		return nullptr;

	PxRigidStatic* iglooActor = gPhysics->createRigidStatic(PxTransform(PxVec3(0, 0, 0)));

	pxMeshVtxInfo pPxVtx;
	pPxVtx.ulVertexCnt = cnt;
	pPxVtx.pxVecVertexPos = new PxVec3[cnt];

	XMFLOAT3 vecPos;
	
	for (unsigned long i = 0; i < mgr->GetMeshVtxInfo(meshname).ulVertexCnt; ++i)
	{
		XMStoreFloat3(&vecPos, XMVector3TransformCoord(XMLoadFloat3(&pos[i]), matWorld));
		pPxVtx.pxVecVertexPos[i].x = vecPos.x;
		pPxVtx.pxVecVertexPos[i].y = vecPos.y;
		pPxVtx.pxVecVertexPos[i].z = vecPos.z;
	}

	Indices* indic = new Indices[idx.size()];
	for (size_t i = 0; i < idx.size(); ++i)
	{
		indic[i] = idx[i];
	}
	/*Engine::Triangle* indices = nullptr;

	indices = new Engine::Triangle[pMesh->NumFace];



	size_t size = pMesh->vecTIndex.size();
	for (_ulong i = 0; i < pMesh->NumFace; ++i)
	{
	indices[i] = pMesh->vecTIndex[i];
	}*/




	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = cnt;
	meshDesc.points.stride = sizeof(PxVec3);
	meshDesc.points.data = pPxVtx.pxVecVertexPos;

	meshDesc.triangles.count = idx.size(); // 없어야 된다고??
	meshDesc.triangles.stride = 3 * sizeof(unsigned int);

	meshDesc.triangles.data = indic; // 이게없어야되네...?




	assert(meshDesc.isValid());


	PxDefaultMemoryOutputStream writeBuffer;
	PxTriangleMeshCookingResult::Enum result;
	bool status = gCooking->cookTriangleMesh(meshDesc, writeBuffer, &result);

	 
	if (!status)
	{
		delete[] pPxVtx.pxVecVertexPos;

		return nullptr;
	}
	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());

	PxTriangleMesh* triMesh = gPhysics->createTriangleMesh(readBuffer);


	//Engine::Safe_Delete_Array(indices);

	if (triMesh == NULL)
	{
		delete[] pPxVtx.pxVecVertexPos;
		return nullptr;
	}


	PxTriangleMeshGeometry geom(triMesh);
	PxShape* iglooShape = PxRigidActorExt::createExclusiveShape(*iglooActor, geom, *gMaterial);


	gScene->addActor(*iglooActor);

	MyPhysXGameObject str;
	str.pObject = pObj;
	str.pRigidStatic = iglooActor;
	VecPGO.push_back(str);

	PStaticlist.push_back(iglooActor);

	iglooActor->setName("");

	return iglooActor;
}

//PxRigidStatic* CPhysXMgr::CreateTerrain(Engine::CTerrainTex* pBuffer, Engine::VTXTEX* pVertex, Engine::INDEX32* pIndex)
//{
//	m_StaticOldTerrain = m_StaticTerrain;
//	PxRigidStatic* iglooActor = gPhysics->createRigidStatic(PxTransform(PxVec3(0, 0, 0)));
//
//
//	PxVec3* VectexPos = new PxVec3[pBuffer->Get_VtxCnt()]; 
//
//	PxTriangle* indices = new PxTriangle[pBuffer->Get_TriCnt()];
//
//
//
//	for (_ulong i = 0; i < pBuffer->Get_VtxCnt(); ++i)
//	{
//		VectexPos[i].x = pVertex[i].vPos.x;
//		VectexPos[i].y = pVertex[i].vPos.y;
//		VectexPos[i].z = pVertex[i].vPos.z;
//	}
//	
//	for (_ulong i = 0; i < pBuffer->Get_TriCnt(); ++i)
//	{
//		indices[i].ind0 = pIndex[i]._0;
//		indices[i].ind1 = pIndex[i]._1;
//		indices[i].ind2 = pIndex[i]._2;
//	}
//
//	PxTriangleMeshDesc meshDesc;
//	meshDesc.points.count = pBuffer->Get_VtxCnt();
//	meshDesc.points.stride = sizeof(PxVec3);
//	meshDesc.points.data = VectexPos;
//
//	meshDesc.triangles.count = pBuffer->Get_TriCnt();
//	meshDesc.triangles.stride = sizeof(PxTriangle);
//	meshDesc.triangles.data = indices;
//
//
//	PxDefaultMemoryOutputStream writeBuffer;
//	PxTriangleMeshCookingResult::Enum result;
//	bool status = gCooking->cookTriangleMesh(meshDesc, writeBuffer, &result);
//	
//	if (!status)
//		return nullptr;
//
//	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
//
//	PxTriangleMesh* triMesh = gPhysics->createTriangleMesh(readBuffer);
//
//	Safe_Delete_Array(VectexPos);
//	Safe_Delete_Array(indices);
//
//	if (triMesh == NULL)
//		return nullptr;
//
//
//	PxTriangleMeshGeometry geom(triMesh);
//	PxShape* iglooShape = PxRigidActorExt::createExclusiveShape(*iglooActor, geom, *gMaterial);
//
//
//	gScene->addActor(*iglooActor);
//
//	
//
//	m_StaticTerrain = iglooActor;
//	m_StaticTerrain->setName("터레인");
//
//	
//	MyPhysXGameObject str;
//	str.pRigidStatic = iglooActor;
//	VecPGO.push_back(str);
//	PStaticlist.push_back(iglooActor);
//
//	return iglooActor;
//}
//
////현재안쓰는중
//PxBoxController* CPhysXMgr::CreateBoxController(CPhysXObject* pObj, _vec3 Pos_, _vec3 Scale_)
//{	
//	PxBoxControllerDesc cDesc;
//
//	cDesc.halfSideExtent	 = Scale_.x; // 가로
//	cDesc.halfHeight = Scale_.y; // 높이
//	cDesc.halfForwardExtent  = Scale_.z; // 세로
//
//
//	cDesc.material = gMaterial;
//	
//	cDesc.position.x = Pos_.x;
//	cDesc.position.y = Pos_.y;
//	cDesc.position.z = Pos_.z;
//
//	cDesc.slopeLimit = 0.f;
//	cDesc.contactOffset = 0.000001f;
//	cDesc.stepOffset = 0.2f;
//	cDesc.invisibleWallHeight = 0.f;
//
//	
//
//	cDesc.maxJumpHeight = 0.f;
//	
//	
//	//mControllerInitialPosition = cDesc.position;
//	PxBoxController* ctrl = static_cast<PxBoxController*>(mControllerManager->createController(cDesc));
//	
//	// remove controller shape from scene query for standup overlap test
//	PxRigidDynamic* actor = ctrl->getActor();
//	PxShape* ctrlShape = nullptr;
//
//	if (actor)
//	{
//		if (actor->getNbShapes())
//		{
//			actor->getShapes(&ctrlShape, 1);
//			ctrlShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, false);
//			//ctrlShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
//			
//		}
//	}
//	
//	MyPhysXGameObject str;
//	str.pObject = pObj;
//	str.pRigidDynamic = actor;
//	VecPGO.push_back(str);
//	
//	actor->setName("");
//
//	return ctrl;
//}
//
PxCapsuleController * CPhysXMgr::CreateCapsuleController(Object* pObj, XMFLOAT3 Pos_, float Radius, float Height, bool Player)
{
	PxCapsuleControllerDesc cDesc;

	cDesc.height = Height;
	cDesc.radius = Radius;



	cDesc.material = gMaterial;

	cDesc.position.x = Pos_.x;
	cDesc.position.y = Pos_.y;
	cDesc.position.z = Pos_.z;

	cDesc.slopeLimit = 0.f;
	//cDesc.contactOffset = 0.0000009f;
	cDesc.contactOffset = 0.1f; // 보이지않는 추가적인충돌범위추가




	cDesc.stepOffset = 0.2f;

	cDesc.invisibleWallHeight = 0.f;
	cDesc.maxJumpHeight = 10.f;
	cDesc.density = 10.f;
	

	PxCapsuleController* ctrl = static_cast<PxCapsuleController*>(mControllerManager->createController(cDesc));

	PxRigidDynamic* actor = ctrl->getActor();
	PxShape* ctrlShape = nullptr;

	if (actor)
	{
		if (actor->getNbShapes())
		{
			actor->getShapes(&ctrlShape, 1);
			if(Player)
				ctrlShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, false);
			else
				ctrlShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);


		/*	ctrlShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
			ctrlShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);*/
		}
	}

	MyPhysXGameObject str;
	str.pObject = pObj;
	str.pRigidDynamic = actor;
	VecPGO.push_back(str);


	PDynamiclist.push_back(ctrl->getActor());

	actor->setName("");

	return ctrl;

}

//bool CPhysXMgr::CollisionDetectionBetweenDynamicAndStaticMesh(PxRigidDynamic* pDynamic, PxRigidStatic* pStatic)
//{
//	if (pDynamic == nullptr || pStatic == nullptr)
//		return false;
//
//	PxShape* ThisShape;
//	pDynamic->getShapes(&ThisShape, 1);
//	PxGeometry* pGeometry = nullptr;
//
//
//	if (ThisShape->getGeometryType() == PxGeometryType::eBOX)
//	{
//		PxBoxGeometry boxgeom;
//		ThisShape->getBoxGeometry(boxgeom);
//		pGeometry = &boxgeom;
//
//	}
//
//	if (ThisShape->getGeometryType() == PxGeometryType::eSPHERE)
//	{
//		PxSphereGeometry ballgeom;
//		ThisShape->getSphereGeometry(ballgeom);
//		pGeometry = &ballgeom;
//
//	}
//
//	if (ThisShape->getGeometryType() == PxGeometryType::eCAPSULE)
//	{
//		PxCapsuleGeometry Capsulegeom;
//		ThisShape->getCapsuleGeometry(Capsulegeom);
//
//		if (!strcmp(pDynamic->getName(), "Player"))
//		{
//			float hh = Capsulegeom.halfHeight;
//			Capsulegeom.halfHeight = 2.f;
//		}
//		pGeometry = &Capsulegeom;
//
//	}
//		
//	PxShape* terraShape;
//	pStatic->getShapes(&terraShape, 1);
//
//	if (terraShape->getGeometryType() != PxGeometryType::eTRIANGLEMESH)
//	{
//		return false;
//	}
//
//	PxTriangleMeshGeometry terrametry;
//	terraShape->getTriangleMeshGeometry(terrametry);
//	//PxTransform meshPose = pTerrain->m_Static->getGlobalPose();
//
//	PxTransform abc = pStatic->getGlobalPose();
//
//	PxMeshOverlapUtil overlapUtil;
//	const PxU32 nbTouchedTris = overlapUtil.findOverlap(*pGeometry, pDynamic->getGlobalPose(), terrametry, pStatic->getGlobalPose());
//	//const PxU32* PX_RESTRICT indices = overlapUtil.getResults();
//	
//	if (nbTouchedTris != 0)
//	{
//		return true;
//	}
//
//	return false;
//}
//
//
//
//bool CPhysXMgr::OverlapBetweenTwoObject(PxRigidActor * pBody0, PxRigidActor * pBody1) // 겹침체크
//{
//	PxShape* Shape[2];
//	pBody0->getShapes(&Shape[0], 1);
//	pBody1->getShapes(&Shape[1], 1);
//	PxGeometry* Geometry[2];
//
//	if (Shape[0]->getGeometryType() == PxGeometryType::eBOX)
//	{
//		PxBoxGeometry boxgeom;
//		Shape[0]->getBoxGeometry(boxgeom);
//		Geometry[0] = &boxgeom;
//	}
//	else
//		return false;
//	
//
//	if (Shape[1]->getGeometryType() == PxGeometryType::eBOX)
//	{
//		PxBoxGeometry boxgeom;
//		Shape[1]->getBoxGeometry(boxgeom);
//		Geometry[1] = &boxgeom;
//	}
//	else
//		return false;
//
//	if (PxGeometryQuery::overlap(*Geometry[0], pBody0->getGlobalPose(), *Geometry[1], pBody1->getGlobalPose()))
//	{
//		return true;
//	}
//	else
//		return false;
//}

//bool CPhysXMgr::SweepBetweenTwoObject(PxRigidDynamic * pBody0, PxRigidDynamic * pBody1){
//	PxVec3 Pos[2];
//	Pos[0] = pBody0->getGlobalPose().p;
//	Pos[1] = pBody1->getGlobalPose().p;
//	PxVec3 Dir = Pos[0] - Pos[1];
//	Dir.normalize();
//
//
//
//	PxShape* Shape[2];
//	pBody0->getShapes(&Shape[0], 1);
//	pBody1->getShapes(&Shape[1], 1);
//
//	PxGeometry* Geometry[2];
//
//	/*auto box = PxGeometryType::eBOX;
//	auto sphere = PxGeometryType::eSPHERE;
//	auto capsule = PxGeometryType::eCAPSULE;
//
//	auto geomet0 = Shape[0]->getGeometryType();
//	auto geomet1 = Shape[1]->getGeometryType();
//
//	
//
//	if (geomet0 == box)
//	{
//		PxBoxGeometry boxgeom;
//		Shape[0]->getBoxGeometry(boxgeom);
//		Geometry[0] = &boxgeom;
//
//	}
//
//	if (geomet0 == sphere)
//	{
//		PxSphereGeometry ballgeom;
//		Shape[0]->getSphereGeometry(ballgeom);
//		Geometry[0] = &ballgeom;
//	}
//
//	if (geomet0 == capsule)
//	{
//
//		PxCapsuleGeometry Capsulegeom;
//		Shape[0]->getCapsuleGeometry(Capsulegeom);
//
//		if (!strcmp(pBody0->getName(), "Player"))
//		{
//			Capsulegeom.radius = 0.6f;
//
//			Capsulegeom.halfHeight = 2.f;
//		}
//
//
//		Geometry[0] = &Capsulegeom;
//	}
//
//
//
//	if (geomet1 == box)
//	{
//		PxBoxGeometry boxgeom2;
//		Shape[1]->getBoxGeometry(boxgeom2);
//		Geometry[1] = &boxgeom2;
//	}
//
//	if (geomet1 == sphere)
//	{
//		PxSphereGeometry ballgeom2;
//		Shape[1]->getSphereGeometry(ballgeom2);
//		Geometry[1] = &ballgeom2;
//	}
//
//	if (geomet1 == capsule)
//	{
//
//		PxCapsuleGeometry Capsulegeom2;
//		Shape[1]->getCapsuleGeometry(Capsulegeom2);
//		if (!strcmp(pBody1->getName(), "Player"))
//		{
//			Capsulegeom2.radius = 0.6f;
//
//			Capsulegeom2.halfHeight = 2.f;
//		}
//		Geometry[1] = &Capsulegeom2;
//	}*/
//
//	/*if (Shape[0]->getGeometryType() == PxGeometryType::eTRIANGLEMESH ||
//		Shape[1]->getGeometryType() == PxGeometryType::eTRIANGLEMESH
//		)
//	{
//		return false;
//	}*/
//
//	/*if (Shape[0]->getGeometryType() == PxGeometryType::eBOX)
//	{
//		PxBoxGeometry boxgeom;
//		Shape[0]->getBoxGeometry(boxgeom);
//		Geometry[0] = &boxgeom;
//	
//	}
//
//	if (Shape[0]->getGeometryType() == PxGeometryType::eSPHERE)
//	{
//		PxSphereGeometry ballgeom;
//		Shape[0]->getSphereGeometry(ballgeom);
//		Geometry[0] = &ballgeom;
//	}
//
//	if (Shape[0]->getGeometryType() == PxGeometryType::eCAPSULE)
//	{
//
//		PxCapsuleGeometry Capsulegeom;
//		Shape[0]->getCapsuleGeometry(Capsulegeom);
//
//		if (!strcmp(pBody0->getName(), "Player"))
//		{
//			Capsulegeom.radius = 0.6f;
//
//			Capsulegeom.halfHeight = 2.f;
//		}
//
//
//		Geometry[0] = &Capsulegeom;
//	}*/
//
//	PxCapsuleGeometry Capsulegeom;
//	Shape[0]->getCapsuleGeometry(Capsulegeom);
//
//	if (!strcmp(pBody0->getName(), "Player"))
//	{
//		Capsulegeom.radius = 0.6f;
//
//		Capsulegeom.halfHeight = 2.f;
//	}
//	
//	Geometry[0] = &Capsulegeom;
//
//
//	if (Shape[1]->getGeometryType() == 3) // PxGeometryType::eBOX
//	{
//		PxBoxGeometry boxgeom2;
//		Shape[1]->getBoxGeometry(boxgeom2);
//		Geometry[1] = &boxgeom2;
//	}
//
//	if (Shape[1]->getGeometryType() == 0) // PxGeometryType::eSPHERE
//	{
//		PxSphereGeometry ballgeom2;
//		Shape[1]->getSphereGeometry(ballgeom2);
//		Geometry[1] = &ballgeom2;
//	}
//
//	
//
//	
//
//	//PxHitCallback
//
//	PxSweepHit SweepHit;
//	if (PxGeometryQuery::sweep(Dir, 100.f, *Geometry[0], pBody0->getGlobalPose(), *Geometry[1], pBody1->getGlobalPose(), SweepHit, PxHitFlag::eDEFAULT))
//	{
//		//충돌했다?
//		return true;
//	}
//	return false;
//}

bool CPhysXMgr::SweepBoxSphere(PxRigidDynamic * pBody0, PxRigidDynamic * pBody1)
{
	PxVec3 Pos[2];
	Pos[0] = pBody0->getGlobalPose().p;
	Pos[1] = pBody1->getGlobalPose().p;
	PxVec3 Dir = Pos[0] - Pos[1];
	Dir.normalize();

	PxShape* Shape[2];
	pBody0->getShapes(&Shape[0], 1);
	pBody1->getShapes(&Shape[1], 1);

	PxGeometry* Geometry[2];

	if (Shape[0]->getGeometryType() != PxGeometryType::eBOX)
		return false;

	PxBoxGeometry boxgeom;
	Shape[0]->getBoxGeometry(boxgeom);
	Geometry[0] = &boxgeom;

	if (Shape[1]->getGeometryType() != PxGeometryType::eSPHERE)
		return false;

	PxSphereGeometry ballgeom;
	Shape[1]->getSphereGeometry(ballgeom);
	Geometry[1] = &ballgeom;

	PxSweepHit SweepHit;
	if (PxGeometryQuery::sweep(Dir, 10.f, *Geometry[0], pBody0->getGlobalPose(), *Geometry[1], pBody1->getGlobalPose(), SweepHit, PxHitFlag::eDEFAULT))
	{
		//충돌했다?
		return true;
	}
	return false;

}

bool CPhysXMgr::SweepBoxBox(PxRigidDynamic * pBody0, PxRigidDynamic * pBody1)
{
	PxVec3 Pos[2];
	Pos[0] = pBody0->getGlobalPose().p;
	Pos[1] = pBody1->getGlobalPose().p;
	PxVec3 Dir = Pos[0] - Pos[1];
	Dir.normalize();

	PxShape* Shape[2];
	pBody0->getShapes(&Shape[0], 1);
	pBody1->getShapes(&Shape[1], 1);

	PxGeometry* Geometry[2];

	if (Shape[0]->getGeometryType() != PxGeometryType::eBOX)
		return false;

	PxBoxGeometry boxgeom;
	Shape[0]->getBoxGeometry(boxgeom);
	Geometry[0] = &boxgeom;

	if (Shape[1]->getGeometryType() != PxGeometryType::eBOX)
		return false;

	PxBoxGeometry boxgeom2;
	Shape[1]->getBoxGeometry(boxgeom2);
	Geometry[1] = &boxgeom2;

	PxSweepHit SweepHit;
	if (PxGeometryQuery::sweep(Dir, 10.f, *Geometry[0], pBody0->getGlobalPose(), *Geometry[1], pBody1->getGlobalPose(), SweepHit, PxHitFlag::eDEFAULT))
	{
		//충돌했다?
		return true;
	}
	return false;
}

bool CPhysXMgr::SweepSphereSphere(PxRigidDynamic * pBody0, PxRigidDynamic * pBody1)
{
	PxVec3 Pos[2];
	Pos[0] = pBody0->getGlobalPose().p;
	Pos[1] = pBody1->getGlobalPose().p;
	PxVec3 Dir = Pos[0] - Pos[1];
	Dir.normalize();

	PxShape* Shape[2];
	pBody0->getShapes(&Shape[0], 1);
	pBody1->getShapes(&Shape[1], 1);

	PxGeometry* Geometry[2];

	if (Shape[0]->getGeometryType() != PxGeometryType::eSPHERE)
		return false;

	PxSphereGeometry ballgeom;
	Shape[0]->getSphereGeometry(ballgeom);
	Geometry[0] = &ballgeom;

	if (Shape[1]->getGeometryType() != PxGeometryType::eSPHERE)
		return false;

	PxSphereGeometry ballgeom2;
	Shape[1]->getSphereGeometry(ballgeom2);
	Geometry[1] = &ballgeom2;

	PxSweepHit SweepHit;
	if (PxGeometryQuery::sweep(Dir, 10.f, *Geometry[0], pBody0->getGlobalPose(), *Geometry[1], pBody1->getGlobalPose(), SweepHit, PxHitFlag::eDEFAULT))
	{
		//충돌했다?
		return true;
	}
	return false;
}

bool CPhysXMgr::SweepCapsuleBox(PxRigidDynamic * pBody0, PxRigidDynamic * pBody1)
{
	PxVec3 Pos[2];
	Pos[0] = pBody0->getGlobalPose().p;
	Pos[1] = pBody1->getGlobalPose().p;
	PxVec3 Dir = Pos[0] - Pos[1];
	Dir.normalize();

	PxShape* Shape[2];
	pBody0->getShapes(&Shape[0], 1);
	pBody1->getShapes(&Shape[1], 1);

	PxGeometry* Geometry[2];

	if (Shape[0]->getGeometryType() != PxGeometryType::eCAPSULE)
		return false;

	PxCapsuleGeometry Capsulegeom;
	Shape[0]->getCapsuleGeometry(Capsulegeom);
	if (!strcmp(pBody0->getName(), "Player"))
	{
		Capsulegeom.radius = 0.6f;

		Capsulegeom.halfHeight = 2.f;
	}
	Geometry[0] = &Capsulegeom;

	if (Shape[1]->getGeometryType() != PxGeometryType::eBOX)
		return false;

	PxBoxGeometry boxgeom;
	Shape[1]->getBoxGeometry(boxgeom);
	Geometry[1] = &boxgeom;

	PxSweepHit SweepHit;
	if (PxGeometryQuery::sweep(Dir, 10.f, *Geometry[0], pBody0->getGlobalPose(), *Geometry[1], pBody1->getGlobalPose(), SweepHit, PxHitFlag::eDEFAULT))
	{
		//충돌했다?
		return true;
	}
	return false;
}

bool CPhysXMgr::SweepCapsuleSphere(PxRigidDynamic * pBody0, PxRigidDynamic * pBody1)
{
	PxVec3 Pos[2];
	Pos[0] = pBody0->getGlobalPose().p;
	Pos[1] = pBody1->getGlobalPose().p;
	PxVec3 Dir = Pos[0] - Pos[1];
	Dir.normalize();

	PxShape* Shape[2];
	pBody0->getShapes(&Shape[0], 1);
	pBody1->getShapes(&Shape[1], 1);

	PxGeometry* Geometry[2];

	if (Shape[0]->getGeometryType() != PxGeometryType::eCAPSULE)
		return false;

	PxCapsuleGeometry Capsulegeom;
	Shape[0]->getCapsuleGeometry(Capsulegeom);

	if (!strcmp(pBody0->getName(), "Player"))
	{
		Capsulegeom.radius = 0.6f;

		Capsulegeom.halfHeight = 2.f;
	}
	Geometry[0] = &Capsulegeom;

	if (Shape[1]->getGeometryType() != PxGeometryType::eSPHERE)
		return false;

	PxSphereGeometry ballgeom;
	Shape[1]->getSphereGeometry(ballgeom);
	Geometry[1] = &ballgeom;

	PxSweepHit SweepHit;
	if (PxGeometryQuery::sweep(Dir, 10.f, *Geometry[0], pBody0->getGlobalPose(), *Geometry[1], pBody1->getGlobalPose(), SweepHit, PxHitFlag::eDEFAULT))
	{
		//충돌했다?
		return true;
	}
	return false;
}



bool CPhysXMgr::SweepBetweenPlayerAndStatic(PxRigidActor * pBody0, PxRigidActor * pBody1)
{
	PxVec3 Pos[2];
	Pos[0] = pBody0->getGlobalPose().p;
	Pos[1] = pBody1->getGlobalPose().p;
	PxVec3 Dir = Pos[0] - Pos[1];
	Dir.normalize();


	PxShape* Shape[2];
	pBody0->getShapes(&Shape[0], 1);
	pBody1->getShapes(&Shape[1], 1);

	PxGeometry* Geometry[2];

	if (Shape[0]->getGeometryType() == PxGeometryType::eBOX)
	{
		PxBoxGeometry boxgeom;
		Shape[0]->getBoxGeometry(boxgeom);
		Geometry[0] = &boxgeom;
	}

	if (Shape[0]->getGeometryType() == PxGeometryType::eSPHERE)
	{
		PxSphereGeometry ballgeom;
		Shape[0]->getSphereGeometry(ballgeom);
		Geometry[0] = &ballgeom;
	}

	if (Shape[0]->getGeometryType() == PxGeometryType::eCAPSULE)
	{

		PxCapsuleGeometry Capsulegeom;
		Shape[0]->getCapsuleGeometry(Capsulegeom);

		if (!strcmp(pBody0->getName(), "Player"))		
		{
			Capsulegeom.radius = 0.6f;

			Capsulegeom.halfHeight = 2.f;
		}
		Geometry[0] = &Capsulegeom;
	}

	if (Shape[0]->getGeometryType() == PxGeometryType::eTRIANGLEMESH)
	{
		PxTriangleMeshGeometry TriangleMeshgeom;
		Shape[0]->getTriangleMeshGeometry(TriangleMeshgeom);
		Geometry[0] = &TriangleMeshgeom;
	}

	if (Shape[1]->getGeometryType() == PxGeometryType::eBOX)
	{
		PxBoxGeometry boxgeom;
		Shape[1]->getBoxGeometry(boxgeom);
		Geometry[1] = &boxgeom;
	}

	if (Shape[1]->getGeometryType() == PxGeometryType::eSPHERE)
	{
		PxSphereGeometry ballgeom;
		Shape[1]->getSphereGeometry(ballgeom);
		Geometry[1] = &ballgeom;
	}

	if (Shape[1]->getGeometryType() == PxGeometryType::eCAPSULE)
	{
		PxCapsuleGeometry Capsulegeom;
		Shape[1]->getCapsuleGeometry(Capsulegeom);

		if (!strcmp(pBody1->getName(), "Player"))
		{
			Capsulegeom.radius = 0.6f;

			Capsulegeom.halfHeight = 2.f;
		}
		Geometry[1] = &Capsulegeom;
	}

	if (Shape[1]->getGeometryType() == PxGeometryType::eTRIANGLEMESH)
	{
		PxTriangleMeshGeometry TriangleMeshgeom;
		Shape[1]->getTriangleMeshGeometry(TriangleMeshgeom);
		Geometry[1] = &TriangleMeshgeom;
	}

	PxVec3 gp = pBody0->getGlobalPose().p;

	PxSweepHit SweepHit;
	if (PxGeometryQuery::sweep(Dir, 100.f, *Geometry[0], pBody0->getGlobalPose(), *Geometry[1], pBody1->getGlobalPose(), SweepHit, PxHitFlag::ePRECISE_SWEEP))
	{
		PxRigidStatic* Static = pBody1->is<PxRigidStatic>();
		if (Static != NULL)
		{
			return true;
		}
	}
	return false;
}

//PxRigidDynamic* CPhysXMgr::ShotRay(_vec3 RayPos, _vec3 RayDir, float MaxDistance)
//{
//	PxVec3 origin = ToPxVec3(RayPos);
//		
//	D3DXVec3Normalize(&RayDir, &RayDir);
//
//	PxVec3 unitDir = ToPxVec3(RayDir);    // [in] Normalized ray direction
//	// [in] MaxDistance -> Raycast max distance
//
//	PxRaycastBuffer buf_;
//	
//	if (gScene->raycast(origin, unitDir, MaxDistance, buf_, PxHitFlags(PxHitFlag::eMESH_ANY)))
//	{
//		//PxHitFlags(PxHitFlag::eMESH_ANY) 레이에 첫번째로 적중한 객체를 반환한다.
//		const char* name = buf_.block.actor->getName();
//		PxRigidStatic* Static = buf_.block.actor->is<PxRigidStatic>();
//		if (Static != NULL)
//		{
//			int a = 0;
//		}
//			PxRigidDynamic* Dynamic = buf_.block.actor->is<PxRigidDynamic>();
//			if (Dynamic != NULL)
//			{
//				return Dynamic;
//			}
//		
//	}
//
//	return nullptr;
//}
//
//PxRigidStatic* CPhysXMgr::ShotRayStatic(_vec3 RayPos, _vec3 RayDir, float MaxDistance, _float& fDist)
//{
//	PxVec3 origin = ToPxVec3(RayPos);
//
//	D3DXVec3Normalize(&RayDir, &RayDir);
//
//	PxVec3 unitDir = ToPxVec3(RayDir);    // [in] Normalized ray direction
//										  // [in] MaxDistance -> Raycast max distance
//
//	PxRaycastBuffer buf_;
//
//	if (gScene->raycast(origin, unitDir, MaxDistance, buf_, PxHitFlags(PxHitFlag::eMESH_ANY)))
//	{
//		//PxHitFlags(PxHitFlag::eMESH_ANY) 레이에 첫번째로 적중한 객체를 반환한다.
//		fDist = buf_.block.distance;
//
//		PxRigidStatic* Static = buf_.block.actor->is<PxRigidStatic>();
//		//if (Static->getName() == "터레인")
//
//
//		if (Static != NULL)
//		{
//			return Static;
//		}
//		/*PxRigidDynamic* Dynamic = buf_.block.actor->is<PxRigidDynamic>();
//		if (Dynamic != NULL)
//		{
//		return Dynamic;
//		}*/
//
//	}
//
//	return nullptr;
//}
//
//CPhysXObject * CPhysXMgr::SearchSameObject(PxRigidActor * pActor)
//{
//	if(pActor == nullptr)
//		return nullptr;
//
//	for (auto iter : VecPGO)
//	{
//		if (pActor == iter.pRigidDynamic)
//		{
//			return iter.pObject;
//		}
//	}
//
//	return nullptr;
//}
//
//
//
//
//_matrix CPhysXMgr::ApplyPhysXWorldMatrix(PxRigidActor* pRigid, _vec3 _Scale, _vec3 _Pivot)
//{
//	_matrix matScale, matTrans, qutmat, WorldMatrix, pivotchange;
//
//	D3DXQUATERNION matr;
//
//	_vec3 vecs, vect;
//
//	PxTransform gp = pRigid->getGlobalPose();
//	PxMat44  m = PxMat44(gp);//피직스 객체에서 월드행렬 가져옴
//	
//	WorldMatrix = CPhysXMgr::GetInstance()->ToMatrix(m); // 써먹기위해 자료형 변환
//
//	D3DXMatrixDecompose(&vecs, &matr, &vect, &WorldMatrix); // 쿼터니온을 뽑아내서 각도를 알아냄
//	D3DXMatrixRotationQuaternion(&qutmat, &matr); // 쿼터니언 -> 행렬로 변환
//
//	D3DXMatrixScaling(&matScale, _Scale.x, _Scale.y, _Scale.z);
//	D3DXMatrixTranslation(&matTrans, gp.p.x, gp.p.y, gp.p.z);
//	
//	D3DXMatrixTranslation(&pivotchange, _Pivot.x, _Pivot.y, _Pivot.z);
//	
//	WorldMatrix = matScale * pivotchange * qutmat * matTrans;
//	
//	return WorldMatrix;
//}
//
//
//void CPhysXMgr::ApplyPhysXAngle(PxRigidDynamic* pDynamic, _vec3 _Angle)
//{
//	if (pDynamic == nullptr)
//		return;
//
//	PxQuat QuatTest;
//	D3DXQUATERNION Quatx, Quaty, Quatz, QuatLast;
//	_matrix matrotx, matroty, matrotz;
//	_matrix qutmat;
//	_vec3 vecs, vect;
//
//	D3DXMatrixRotationX(&matrotx, _Angle.x);
//	D3DXMatrixRotationY(&matroty, _Angle.y);
//	D3DXMatrixRotationZ(&matrotz, _Angle.z);
//
//	D3DXMatrixDecompose(&vecs, &Quatx, &vect, &matrotx);
//	D3DXMatrixDecompose(&vecs, &Quaty, &vect, &matroty);
//	D3DXMatrixDecompose(&vecs, &Quatz, &vect, &matrotz);
//
//	QuatLast = Quatx * Quaty * Quatz;
//	QuatTest.x = QuatLast.x;
//	QuatTest.y = QuatLast.y;
//	QuatTest.z = QuatLast.z;
//	QuatTest.w = QuatLast.w;
//
//	PxTransform imsi = pDynamic->getGlobalPose();
//	imsi.q = QuatTest;
//	pDynamic->setGlobalPose(imsi);
//}
//
//
//
//
//bool CPhysXMgr::CheckCircleCollision(PxRigidBody* pDynamic1, PxRigidBody* pDynamic2)
//{
//
//	_vec3 Pos1 = ToVec3(pDynamic1->getGlobalPose().p);
//	_vec3 Pos2 = ToVec3(pDynamic2->getGlobalPose().p);
//
//	PxSphereGeometry ballgeom[2];
//	PxShape* Shape[2];
//	float Radius[2];
//
//	pDynamic1->getShapes(&Shape[0], 1);
//	pDynamic2->getShapes(&Shape[1], 1);
//	Shape[0]->getSphereGeometry(ballgeom[0]);
//	Shape[0]->getSphereGeometry(ballgeom[1]);
//	Radius[0] = ballgeom[0].radius;
//	Radius[1] = ballgeom[1].radius;
//	
//
//
//	if (구충돌체크(Pos1, Radius[0], Pos2, Radius[1]))
//	{
//		return true;
//	}
//	
//		return false;
//}
//
//
//bool CPhysXMgr::AABBCheck(PxRigidDynamic* pDynamic1, PxRigidDynamic* pDynamic2)
//{
//	PxShape* Shape[2];
//	PxBoxGeometry ThisboxGeom[2];
//	float width[2], length[2], height[2];
//	_vec3 가세높[2];
//	pDynamic1->getShapes(&Shape[0], 1);
//	Shape[0]->getBoxGeometry(ThisboxGeom[0]);
//	pDynamic2->getShapes(&Shape[1], 1);
//	Shape[1]->getBoxGeometry(ThisboxGeom[1]);
//
//
//	width[0] = ThisboxGeom[0].halfExtents.x;
//	length[0] = ThisboxGeom[0].halfExtents.y;
//	height[0] = ThisboxGeom[0].halfExtents.z;
//
//	width [1] = ThisboxGeom[1].halfExtents.x;
//	length[1] = ThisboxGeom[1].halfExtents.y;
//	height[1] = ThisboxGeom[1].halfExtents.z;
//
//	가세높[0] = { width[0] ,length[0],height[0] };
//	가세높[1] = { width[1] ,length[1],height[1] };
//
//	_matrix pDestWorld, pSourWorld;
//
//	pDestWorld = CPhysXMgr::GetInstance()->ToMatrix(PxMat44(pDynamic1->getGlobalPose()));
//	pSourWorld = CPhysXMgr::GetInstance()->ToMatrix(PxMat44(pDynamic2->getGlobalPose()));
//
//	if (AABBCollisionCheck(가세높[0], &pDestWorld, 가세높[1], &pSourWorld))
//	{
//		return true;
//	}
//	
//	return false;		
//}
//
//
//
//bool CPhysXMgr::AABBCollisionCheck
//(
//	const _vec3 가세높1,
//	_matrix* pDestWorld,
//	const _vec3 가세높2,
//	_matrix* pSourWorld)
//{
//	_vec3	vDestMin, vDestMax, vSourMin, vSourMax,
//		pDestMin , pDestMax, pSourMin, pSourMax;
//
//	pDestMin.x = -가세높1.x;
//	pDestMin.y = -가세높1.y;
//	pDestMin.z = -가세높1.z;
//
//	pDestMax.x = 가세높1.x;
//	pDestMax.y = 가세높1.y;
//	pDestMax.z = 가세높1.z;
//
//	pSourMin.x = -가세높2.x;
//	pSourMin.y = -가세높2.y;
//	pSourMin.z = -가세높2.z;
//
//	pSourMax.x = 가세높2.x;
//	pSourMax.y = 가세높2.y;
//	pSourMax.z = 가세높2.z;
//
//
//	_float	fMin, fMax;
//
//	D3DXVec3TransformCoord(&vDestMin, &pDestMin, pDestWorld);
//	D3DXVec3TransformCoord(&vDestMax, &pDestMax, pDestWorld);
//
//	D3DXVec3TransformCoord(&vSourMin, &pSourMin, pSourWorld);
//	D3DXVec3TransformCoord(&vSourMax, &pSourMax, pSourWorld);
//
//	// x축에서 바라봤을 때
//
//	fMin = max(vDestMin.x, vSourMin.x);
//	fMax = min(vDestMax.x, vSourMax.x);
//
//	if (fMin > fMax)
//		return false;
//
//	// y축에서 바라봤을 때
//
//	fMin = max(vDestMin.y, vSourMin.y);
//	fMax = min(vDestMax.y, vSourMax.y);
//
//	if (fMin > fMax)
//		return false;
//
//	// z축에서 바라봤을 때
//
//	fMin = max(vDestMin.z, vSourMin.z);
//	fMax = min(vDestMax.z, vSourMax.z);
//
//	if (fMin > fMax)
//		return false;
//
//	return true;
//}
//
//bool CPhysXMgr::BoxSphereCollisionCheck(PxRigidBody* pDynamic1, PxRigidBody* pDynamic2) // Box, Sphere
//{
//	PxShape* Shape[2];
//	PxBoxGeometry boxgeom;
//	PxSphereGeometry ballgeom;
//	_vec3 Pos[2];
//
//	float width, length, height;
//	pDynamic1->getShapes(&Shape[0], 1);
//	Shape[0]->getBoxGeometry(boxgeom);
//
//	pDynamic2->getShapes(&Shape[1], 1);
//	Shape[1]->getSphereGeometry(ballgeom);
//	
//	 width = boxgeom.halfExtents.x;
//	length = boxgeom.halfExtents.y;
//	height = boxgeom.halfExtents.z;
//
//	float radius = ballgeom.radius;
//
//	_matrix pDestWorld, pSourWorld;
//
//	pDestWorld = CPhysXMgr::GetInstance()->ToMatrix(PxMat44(pDynamic1->getGlobalPose()));
//	pSourWorld = CPhysXMgr::GetInstance()->ToMatrix(PxMat44(pDynamic2->getGlobalPose()));
//
//	_vec3	vDestMin, vDestMax, vSourMin, vSourMax,
//		pDestMin, pDestMax, pSourMin, pSourMax;
//
//	pDestMin.x = -width;
//	pDestMin.y = -length;
//	pDestMin.z = -height;
//
//	pDestMax.x = width;
//	pDestMax.y = length;
//	pDestMax.z = height;
//
//	pSourMin.x = -radius;
//	pSourMin.y = -radius;
//	pSourMin.z = -radius;
//
//	pSourMax.x = radius;
//	pSourMax.y = radius;
//	pSourMax.z = radius;
//
//
//	Pos[0] = ToVec3(pDynamic1->getGlobalPose().p);
//	Pos[1] = ToVec3(pDynamic2->getGlobalPose().p);
//
//
//	
//
//
//	_float	fMin, fMax;
//
//	D3DXVec3TransformCoord(&vDestMin, &pDestMin, &pDestWorld);
//	D3DXVec3TransformCoord(&vDestMax, &pDestMax, &pDestWorld);
//
//	D3DXVec3TransformCoord(&vSourMin, &pSourMin, &pSourWorld);
//	D3DXVec3TransformCoord(&vSourMax, &pSourMax, &pSourWorld);
//
//	// x축에서 바라봤을 때
//
//	fMin = max(vDestMin.x, vSourMin.x);
//	fMax = min(vDestMax.x, vSourMax.x);
//
//	if (fMin > fMax)
//		return false;
//
//	// y축에서 바라봤을 때
//
//	fMin = max(vDestMin.y, vSourMin.y);
//	fMax = min(vDestMax.y, vSourMax.y);
//
//	if (fMin > fMax)
//		return false;
//
//	// z축에서 바라봤을 때
//
//	fMin = max(vDestMin.z, vSourMin.z);
//	fMax = min(vDestMax.z, vSourMax.z);
//
//	if (fMin > fMax)
//		return false;
//
//	return true;
//
//
//}
//
//
//bool CPhysXMgr::구충돌체크(_vec3 Pos1, float Radius1, _vec3 Pos2, float Radius2)
//{
//	float fDistance = sqrtf(pow((Pos1.x - Pos2.x), 2)
//		+ pow((Pos1.y - Pos2.y), 2)
//		+ pow((Pos1.z - Pos2.z), 2));
//
//	float dis = D3DXVec3Length(&(Pos1 - Pos2));
//
//	float fRadiusSum = float((Radius1) + (Radius2));
//
//
//	if (fRadiusSum > fDistance)
//	{
//		return true;
//	}
//	
//	return false;
//}
//
//
//
//PxMat44 CPhysXMgr::ToPxMat(_matrix _matworld)
//{
//	PxMat44  PhysXMatrix; 
//	PhysXMatrix.column0.x = _matworld._11;
//	PhysXMatrix.column0.y = _matworld._12;
//	PhysXMatrix.column0.z = _matworld._13;
//	PhysXMatrix.column0.w = _matworld._14;
//
//	PhysXMatrix.column1.x = _matworld._21;
//	PhysXMatrix.column1.y = _matworld._22;
//	PhysXMatrix.column1.z = _matworld._23;
//	PhysXMatrix.column1.w = _matworld._24;
//
//	PhysXMatrix.column2.x = _matworld._31;
//	PhysXMatrix.column2.y = _matworld._32;
//	PhysXMatrix.column2.z = _matworld._33;
//	PhysXMatrix.column2.w = _matworld._34;
//
//	PhysXMatrix.column3.x = _matworld._41;
//	PhysXMatrix.column3.y = _matworld._42;
//	PhysXMatrix.column3.z = _matworld._43;
//	PhysXMatrix.column3.w = _matworld._44;
//
//	return PhysXMatrix;
//}
//
//_matrix CPhysXMgr::ToMatrix(PxMat44 _mat44)
//{
//	_matrix  matworld;
//	matworld._11 = _mat44.column0.x;
//	matworld._12 = _mat44.column0.y;
//	matworld._13 = _mat44.column0.z;
//	matworld._14 = _mat44.column0.w;
//	matworld._21 = _mat44.column1.x;
//	matworld._22 = _mat44.column1.y;
//	matworld._23 = _mat44.column1.z;
//	matworld._24 = _mat44.column1.w;
//	matworld._31 = _mat44.column2.x;
//	matworld._32 = _mat44.column2.y;
//	matworld._33 = _mat44.column2.z;
//	matworld._34 = _mat44.column2.w;
//	matworld._41 = _mat44.column3.x;
//	matworld._42 = _mat44.column3.y;
//	matworld._43 = _mat44.column3.z;
//	matworld._44 = _mat44.column3.w;
//
//	return matworld;
//}
//
//_vec3 CPhysXMgr::ToVec3(PxVec3 _PxVector3)
//{
//	_vec3 _vector3;
//	_vector3.x = _PxVector3.x;
//	_vector3.y = _PxVector3.y;
//	_vector3.z = _PxVector3.z;
//	return _vector3;
//}
//
//PxVec3 CPhysXMgr::ToPxVec3(_vec3 _vector3)
//{
//	PxVec3 Pxvector3_;
//	Pxvector3_.x = _vector3.x;
//	Pxvector3_.y = _vector3.y;
//	Pxvector3_.z = _vector3.z;
//	return Pxvector3_;
//}

void CPhysXMgr::Clear_OldTerrain(void)
{
	if (m_StaticOldTerrain != nullptr)
	{

		for (auto iter = CPhysXMgr::GetInstance()->PStaticlist.begin(); CPhysXMgr::GetInstance()->PStaticlist.begin() != CPhysXMgr::GetInstance()->PStaticlist.end();)
		{
			if ((*iter) == m_StaticOldTerrain)
			{
				iter = CPhysXMgr::GetInstance()->PStaticlist.erase(iter);
				break;
			}
			else
				++iter;
		}

		for (auto iter = CPhysXMgr::GetInstance()->VecPGO.begin(); CPhysXMgr::GetInstance()->VecPGO.begin() != CPhysXMgr::GetInstance()->VecPGO.end();)
		{
			if ((*iter).pRigidStatic == m_StaticOldTerrain)
			{
				iter = CPhysXMgr::GetInstance()->VecPGO.erase(iter);
				break;
			}
			else
				++iter;
		}

		m_StaticOldTerrain->release();
		m_StaticOldTerrain = nullptr;
	}

	
}

void CPhysXMgr::Clear_Terrain(void)
{
	if (m_StaticTerrain != nullptr)
	{
		
		for (auto iter = CPhysXMgr::GetInstance()->PStaticlist.begin();
			CPhysXMgr::GetInstance()->PStaticlist.begin() != CPhysXMgr::GetInstance()->PStaticlist.end();)
		{
			if ((*iter) == m_StaticTerrain)
			{
				iter = CPhysXMgr::GetInstance()->PStaticlist.erase(iter);
				break;
			}
			else
				++iter;
		}

		for (auto iter = CPhysXMgr::GetInstance()->VecPGO.begin();
			CPhysXMgr::GetInstance()->VecPGO.begin() != CPhysXMgr::GetInstance()->VecPGO.end();)
		{
			if ((*iter).pRigidStatic == m_StaticTerrain)
			{
				iter = CPhysXMgr::GetInstance()->VecPGO.erase(iter);
				break;
			}
			else
				++iter;
		}

		m_StaticTerrain->release();
		m_StaticTerrain = nullptr;
	}
}



