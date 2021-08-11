#pragma once
//#include <ctype.h>
#include "PxPhysicsAPI.h"

using namespace physx;
class Object;

struct Indices
{
	unsigned int i1, i2, i3;
};
struct MeshVtxInfo
{
	unsigned long		ulVertexCnt;
	vector<XMFLOAT3>	vecVtxPos;
	vector<Indices>		vecIdx;
};

struct pxMeshVtxInfo
{
	unsigned long	ulVertexCnt;
	PxVec3*			pxVecVertexPos;
	Indices*		pIndices;
};

struct MyPhysXGameObject
{
	Object* pObject = nullptr;
	//CPhysXObject* pObject = nullptr;
	PxRigidStatic* pRigidStatic = nullptr;
	PxRigidDynamic* pRigidDynamic = nullptr;
};



class CPhysXMgr //: public PxSimulationEventCallback 
{
private:
	CPhysXMgr();
	CPhysXMgr(const CPhysXMgr& rhs) = delete;
	~CPhysXMgr();
public:
	static CPhysXMgr* GetInstance()
	{
		if (!m_pInstance)
			m_pInstance = new CPhysXMgr();
		return m_pInstance;
	}
	static void DestroyInstance()
	{
		if (m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}
private:
	static CPhysXMgr* m_pInstance;
	//void CheckCollsionPhysX(void(*pPred)(PxRigidDynamic* dy), PxRigidDynamic * pDynamic);


private:
	struct PxTriangle
	{
		PxU32 ind0, ind1, ind2;
	};
public:

	PxDefaultAllocator		gAllocator;
	PxDefaultErrorCallback	gErrorCallback;

	PxFoundation*			gFoundation = NULL;
	PxPhysics*				gPhysics = NULL;
	PxCooking*				gCooking = NULL; // 추가

	PxDefaultCpuDispatcher*	gDispatcher = NULL;
	PxScene*				gScene = NULL;
	PxSceneDesc*			gScenedesc = NULL;

	PxMaterial*				gMaterial = NULL;

	PxPvd*                  gPvd = NULL;

	PxReal stackZ = 10.0f;

	
	PxCapsuleController* m_PlayerController = nullptr;
	
public:
	bool Initialize();
	
	PxRigidDynamic * CreateSphere(Object* pObj, XMFLOAT3 Pos, float Radius, const PxVec3 & velocity = { 0,0,0 }, PxMaterial*	Material_ = nullptr);
	//
	PxRigidDynamic * CreateBox(XMFLOAT3 Pos, PxReal x, PxReal y, PxReal z, PxMaterial* Material_ = nullptr);
	//
	PxCapsuleController * CreateCapsuleController(Object* pObj, XMFLOAT3 Pos, float Radius, float Height, bool Player = false);
	//
	//bool CollisionDetectionBetweenDynamicAndStaticMesh(PxRigidDynamic * pDynamic, PxRigidStatic * pStatic);
	//
	//PxRigidStatic * CreateStaticBox(CPhysXObject* pObj, _vec3 Pos, PxReal x, PxReal y, PxReal z);
	//
	PxRigidStatic * CreateTriangleStaticMesh(Object* pObj, string meshname, XMMATRIX matWorld);
	//
	//PxRigidStatic * CreateTerrain(Engine::CTerrainTex * pBuffer, Engine::VTXTEX * pVertex, Engine::INDEX32 * pIndex);
	//
	//
	//
	////현재안씀
	//PxRigidStatic* CreateHexahedron(CPhysXObject* pObj, Engine::CCubeColor* pBuffer, Engine::VTXCOLORCUBE* pVertex, Engine::INDEX16* pIndex, _vec3 ObjectPos);
	//
	////현재안씀
	//PxBoxController * CreateBoxController(CPhysXObject* pObj, _vec3 Pos_, _vec3 Scale_);

	


	bool OverlapBetweenTwoObject(PxRigidActor * pBody0, PxRigidActor * pBody1); // 겹쳐있는지 검사

	//bool SweepBetweenTwoObject(PxRigidDynamic * pBody0, PxRigidDynamic * pBody1);

	bool SweepBoxSphere(PxRigidDynamic * pBody0, PxRigidDynamic * pBody1);

	bool SweepBoxBox(PxRigidDynamic * pBody0, PxRigidDynamic * pBody1);

	



	bool SweepSphereSphere(PxRigidDynamic * pBody0, PxRigidDynamic * pBody1);

	bool SweepCapsuleBox(PxRigidDynamic * pBody0, PxRigidDynamic * pBody1);

	bool SweepCapsuleSphere(PxRigidDynamic * pBody0, PxRigidDynamic * pBody1);

	bool SweepBetweenPlayerAndStatic(PxRigidActor * pBody0, PxRigidActor * pBody1);

	bool ShotRay(XMVECTOR RayPos, XMVECTOR RayDir, float MaxDistance, bool& dynamicObj, bool& staticObj);
	//
	PxRigidStatic * ShotRayStatic(XMVECTOR RayPos, XMVECTOR RayDir, float MaxDistance, float& fDist);
	//
	//CPhysXObject* SearchSameObject(PxRigidActor* pActor);
	
	vector<PxVec3>				mVerts; // 정점의 위치
	vector<PxU32>				mTris; // 인덱스 순서



	PxControllerManager*			mControllerManager = nullptr;
	

	vector<PxVec3> gContactPositions;
	vector<PxVec3> gContactImpulses;



public:
	//_matrix ApplyPhysXWorldMatrix(PxRigidActor * pRigid, _vec3 _Scale, _vec3 _Pivot = { 0.f, 0.f, 0.f });
	//void ApplyPhysXAngle(PxRigidDynamic * pDynamic, _vec3 _Angle);
	//PxMat44 ToPxMat(_matrix _matworld);
	XMMATRIX ToMatrix(PxMat44 _mat44);
	//_vec3   ToVec3(PxVec3 _PxVector3); // 피직스벡터 -> 벡터
	PxVec3 ToPxVec3(XMFLOAT3 _vector3); // 벡터 -> 피직스벡터
	PxVec3 ToPxVec3(XMVECTOR _vector3); // 벡터 -> 피직스벡터
	//void Set_Layer(Engine::CLayer* pLayer) { m_StaticLayer = pLayer; }

	void	Clear_OldTerrain(void);
	void	Clear_Terrain(void);


	bool CheckCircleCollision(PxRigidBody* dynamic1, PxRigidBody* dynamic2);

	bool AABBCheck(PxRigidDynamic * pDynamic1, PxRigidDynamic * pDynamic2);
	
	//bool AABBCollisionCheck(const _vec3 가세높1, _matrix * pDestWorld, const _vec3 가세높2, _matrix * pSourWorld);

	bool BoxSphereCollisionCheck(PxRigidBody * pDynamic1, PxRigidBody * pDynamic2);

	//bool 구충돌체크(_vec3 Pos1, float Radius1, _vec3 Pos2, float Radius2);
	void ModifyPhysXPos(const float& fTimeDelta, PxRigidDynamic* pDynamic, XMFLOAT3 scale, XMFLOAT4X4 world, XMFLOAT4X4* outWorld, XMFLOAT3* outPos);
	PxTransform MakePxTransform(XMFLOAT4X4 world);
public:
	bool CollisionForStaticObjects(PxRigidActor* pBody);
private:
	//LPDIRECT3DDEVICE9 m_pGraphicDev = nullptr;
	//Engine::CLayer* m_StaticLayer = nullptr;

public:
	PxRigidStatic* m_StaticTerrain = nullptr;
	PxRigidStatic* m_StaticOldTerrain = nullptr;
	vector<MyPhysXGameObject> VecPGO;

	list<PxRigidStatic*> PStaticlist;
	list<PxRigidDynamic*> PDynamiclist;
	PxRigidDynamic* m_pxDynamicBox[5][10];
	list<PxRigidStatic*> m_lstStaticOoc[5][10];

public:
	bool GameStart = false;
	
	float PhysXStartTime = 0.f;
};

