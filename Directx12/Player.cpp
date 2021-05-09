#include "Player.h"

#include "MainApp.h"
#include "Scene.h"
#include "Renderer.h"
#include "Geometry.h"
#include "Camera.h"
#include "Bullet.h"

// Component
#include "Mesh.h"
#include "Transform.h"
#include "Material.h"
#include "Animation.h"
#include "KeyMgr.h"

// FSM
#include "PlayerFSM.h"



Player::Player(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, string _meshName, XMFLOAT3 pos)
	:Object(_meshName)
{
	m_pDevice = device;
	m_pCmdLst = cmdLst;
	m_pRenderer = pRenderer;
	Initialize(pos);


}

Player::~Player()
{
}

void Player::Initialize(XMFLOAT3 pos)
{
	BuildConstantBuffer();

	Component* pComponent = new Transform(XMFLOAT3(0.01f, 0.01f, 0.01f), XMFLOAT3(0.f, 0.f, 0.f), pos);
	//Component* pComponent = new Transform(XMFLOAT3(1.f, 1.f, 1.f), XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(0.f, 0.f, 0.f));
	m_mapComponent["Transform"] = pComponent;
	pComponent = new Mesh(m_pDevice, m_pCmdLst, m_pRenderer->GetHeap(), CHARACTER_WIZARD_01);
	m_mapComponent["Mesh"] = pComponent;
	pComponent = new MaterialCom(CHARACTER_WIZARD_01);
	m_mapComponent["Material"] = pComponent;
	pComponent = new AnimationCom(CHARACTER_WIZARD_01);
	m_mapComponent["Upper_Animation"] = pComponent;
	pComponent = new AnimationCom(CHARACTER_WIZARD_01);
	m_mapComponent["Root_Animation"] = pComponent;

	dynamic_cast<Transform*>(m_mapComponent["Transform"])->SetMeshRotate(XMFLOAT3(-90.f, 0.f, 0.f));
	m_MaterialIndex = dynamic_cast<MaterialCom*>(m_mapComponent["Material"])->GetMaterialIndex();
	
#ifndef NETWORK
	// FSM 만들기
	m_UpperBody = make_unique<PlayerFSM>(this, BoneType::UPPER_BONE); // ( player , BoneType )
	m_RootBody = make_unique<PlayerFSM>(this, BoneType::ROOT_BONE);
#endif // DEBUG


	m_strTextureName = "wizard_01";
}

void Player::Release()
{
}

HRESULT Player::BuildConstantBuffer()
{
	m_SkinnedCB = make_unique<UploadBuffer<SkinnedCB>>(m_pDevice, 1, true);
	
	return S_OK;
}

int Player::Update(const float& fTimeDelta)
{
	Object::Update(fTimeDelta);
	if (m_pCamera->GetMode() == CAMERA_MODE::CAMERA_THIRD)
	{
		XMFLOAT3 xmfRotate = dynamic_cast<Transform*>(m_mapComponent["Transform"])->GetRotate();
		xmfRotate.y = m_pCamera->GetRotY();
		dynamic_cast<Transform*>(m_mapComponent["Transform"])->SetRotate(xmfRotate);

		// 총알 발사
		//if (GetAsyncKeyState(VK_LBUTTON) & 0x0001)
		//{
		//	XMFLOAT3 pos = dynamic_cast<Transform*>(m_mapComponent["Transform"])->GetPosition();
		//	pos.y += 1.f;
		//	MainApp::GetInstance()->GetScene()->PushObject(new Bullet(m_pDevice, m_pCmdLst, m_pRenderer,"FireBall",
		//		pos, dynamic_cast<Transform*>(m_mapComponent["Transform"])->GetRotate(), 30.f), OBJ_TYPE::OBJ_BULLET);
		//}

	}
#ifndef NETWORK
	m_UpperBody->Execute();
	m_RootBody->Execute();
#endif

	return 0;
}

void Player::LateUpdate(const float& fTimeDelta)
{
	Object::LateUpdate(fTimeDelta);

	//UpdateObjectCB();
	UpdateSkinnedCB();

	m_pRenderer->PushObject(RENDER_TYPE::RENDER_DYNAMIC, this);
}

void Player::Render(const float& fTimeDelta, int _instanceCount)
{

	//m_pCmdLst->SetGraphicsRootShaderResourceView(0, InstanceMgr::GetInstnace()->m_InstanceCBs[m_strMeshName]->Resource()->GetGPUVirtualAddress());	// obj
	m_pCmdLst->SetGraphicsRootConstantBufferView(11, m_SkinnedCB->Resource()->GetGPUVirtualAddress());	// skinned
	Object::Render(fTimeDelta, _instanceCount); // 이거 쓰는걸로는 절대로 못함

	
}



void Player::UpdateSkinnedCB()
{
	// SkinnedCB // 이거 애니메이션 붙이기 전까지 붙이면 안뜸 당연하지 쓰레기값가지고 계산되니까
	SkinnedCB SkinnedCB;

	SkinnedCB.BoneTransforms[0] = dynamic_cast<AnimationCom*>(m_mapComponent["Root_Animation"])->GetSkinnedModellnst()->FinalTransforms[0]; // Root 뼈대 == 하체 중심
	SkinnedCB.BoneTransforms[1] = dynamic_cast<AnimationCom*>(m_mapComponent["Root_Animation"])->GetSkinnedModellnst()->FinalTransforms[1]; // Root 뼈대 == 하체 중심
	for (int upper = 2; upper < 27; ++upper) { // 상체
		SkinnedCB.BoneTransforms[upper] = dynamic_cast<AnimationCom*>(m_mapComponent["Upper_Animation"])->GetSkinnedModellnst()->FinalTransforms[upper];
	}
	for (int Root = 27; Root < 33; ++Root) { // 하체
		SkinnedCB.BoneTransforms[Root] = dynamic_cast<AnimationCom*>(m_mapComponent["Root_Animation"])->GetSkinnedModellnst()->FinalTransforms[Root];
	}
	//copy(begin(dynamic_cast<AnimationCom*>(m_mapComponent["Animation"])->GetSkinnedModellnst()->FinalTransforms),
	//	end(dynamic_cast<AnimationCom*>(m_mapComponent["Root_Animation"])->GetSkinnedModellnst()->FinalTransforms),
	//	&SkinnedCB.BoneTransforms[0]);

	m_SkinnedCB->CopyData(0, SkinnedCB);
}

XMFLOAT3 Player::GetPosition()
{
	return dynamic_cast<Transform*>(m_mapComponent["Transform"])->GetPosition();
}

XMFLOAT4X4 Player::GetWorld()
{
	XMFLOAT4X4 world;
	XMStoreFloat4x4(&world, dynamic_cast<Transform*>(m_mapComponent["Transform"])->GetWorldMatrix());
	return world;
}

void Player::SetPosition(XMFLOAT3 xmfPos)
{
	dynamic_cast<Transform*>(m_mapComponent["Transform"])->SetPosition(xmfPos);
}

void Player::SetWorld(XMFLOAT4X4 world)
{
	dynamic_cast<Transform*>(m_mapComponent["Transform"])->SetWorld(world);
}

Component* Player::GetUpperAniController()
{
	auto iter = m_mapComponent.find("Upper_Animation");
	if (iter == m_mapComponent.end())
		return nullptr;

	return (*iter).second;
}

Component* Player::GetRootAniController()
{
	auto iter = m_mapComponent.find("Root_Animation");
	if (iter == m_mapComponent.end())
		return nullptr;

	return (*iter).second;
}

InterfaceFSM* Player::GetRootFSM()
{
	if (m_RootBody == nullptr) // 
		return nullptr;
	return m_RootBody.get();
}


