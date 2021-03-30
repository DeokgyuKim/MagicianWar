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




Player::Player(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer)
{
	m_pDevice = device;
	m_pCmdLst = cmdLst;
	m_pRenderer = pRenderer;
	Initialize();

	
}

Player::~Player()
{
}

void Player::Initialize()
{
	BuildConstantBuffer();

	Component* pComponent = new Transform(XMFLOAT3(0.01f, 0.01f, 0.01f), XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(10.f, 0.f, 10.f));
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

	// FSM 만들기
	m_UpperBody = make_unique<PlayerFSM>(this);
	m_RootBody = make_unique<PlayerFSM>(this);

	dynamic_cast<AnimationCom*>(m_mapComponent["Upper_Animation"])->ChangeAnimation(ANIMATION_TYPE::ATTACK);

	dynamic_cast<AnimationCom*>(m_mapComponent["Root_Animation"])->ChangeAnimation(ANIMATION_TYPE::WALK_FOWARD);
	
	m_strTextureName = "wizard_01";
}

void Player::Release()
{
}

HRESULT Player::BuildConstantBuffer()
{
	m_ObjectCB = make_unique<UploadBuffer<ObjectCB>>(m_pDevice, 1, true);
	m_SkinnedCB = make_unique<UploadBuffer<SkinnedCB>>(m_pDevice, 1, true);
	m_MaterialCB = make_unique<UploadBuffer<MaterialCB>>(m_pDevice, 1, true);

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

		KeyInput();
		if (GetAsyncKeyState(VK_LBUTTON) & 0x0001)
		{
			XMFLOAT3 pos = dynamic_cast<Transform*>(m_mapComponent["Transform"])->GetPosition();
			pos.y += 1.f;
			MainApp::GetInstance()->GetScene()->PushObject(new Bullet(m_pDevice, m_pCmdLst, m_pRenderer,
				pos, dynamic_cast<Transform*>(m_mapComponent["Transform"])->GetRotate(), 30.f), OBJ_TYPE::OBJ_BULLET);
		}
		
	}
	//dynamic_cast<AnimationCom*>(m_mapComponent["Upper_Animation"])->ChangeAnimation(ANIMATION_TYPE::ATTACK);
	//dynamic_cast<AnimationCom*>(m_mapComponent["Root_Animation"])->Update(fTimeDelta);

	return 0;
}

void Player::LateUpdate(const float& fTimeDelta)
{
	Object::LateUpdate(fTimeDelta);

	//m_UpperBody->Execute();		// 상체 갱신
	//m_RootBody->Execute();		// 하체 갱신
	
	// objCB Update
	ObjectCB	ObjCB;
	XMStoreFloat4x4(&ObjCB.World, XMMatrixTranspose(dynamic_cast<Transform*>(m_mapComponent["Transform"])->GetWorldMatrix()));
	m_ObjectCB->CopyData(0, ObjCB);

	// SkinnedCB // 이거 애니메이션 붙이기 전까지 붙이면 안뜸 당연하지 쓰레기값가지고 계산되니까
	SkinnedCB SkinnedCB;
	
	for (int upper = 0; upper < 27; ++upper) { // 상체 갱신
		SkinnedCB.BoneTransforms[upper] = dynamic_cast<AnimationCom*>(m_mapComponent["Upper_Animation"])->GetSkinnedModellnst()->FinalTransforms[upper];
	}
	for (int Root = 27; Root < 33; ++Root) { // 하체 갱신
		SkinnedCB.BoneTransforms[Root] = dynamic_cast<AnimationCom*>(m_mapComponent["Root_Animation"])->GetSkinnedModellnst()->FinalTransforms[Root];
	}
	//copy(begin(dynamic_cast<AnimationCom*>(m_mapComponent["Animation"])->GetSkinnedModellnst()->FinalTransforms),
	//	end(dynamic_cast<AnimationCom*>(m_mapComponent["Root_Animation"])->GetSkinnedModellnst()->FinalTransforms),
	//	&SkinnedCB.BoneTransforms[0]);

	m_SkinnedCB->CopyData(0, SkinnedCB);

	// MaterialCB
	
	MaterialCB matCB;
	XMStoreFloat4(&matCB.Diffuse, dynamic_cast<MaterialCom*>(m_mapComponent["Material"])->GetDiffuse());
	XMStoreFloat4(&matCB.Ambient, dynamic_cast<MaterialCom*>(m_mapComponent["Material"])->GetAmbient());
	XMStoreFloat4(&matCB.Specular, dynamic_cast<MaterialCom*>(m_mapComponent["Material"])->GetSpecular());
	m_MaterialCB->CopyData(0, matCB);

	m_pRenderer->PushObject(RENDER_TYPE::RENDER_DYNAMIC, this);

}

void Player::Render(const float& fTimeDelta)
{

	m_pCmdLst->SetGraphicsRootConstantBufferView(0, m_ObjectCB->Resource()->GetGPUVirtualAddress());	// obj
	m_pCmdLst->SetGraphicsRootConstantBufferView(2, m_MaterialCB->Resource()->GetGPUVirtualAddress());	// material
	m_pCmdLst->SetGraphicsRootConstantBufferView(11, m_SkinnedCB->Resource()->GetGPUVirtualAddress());	// skinned
	Object::Render(fTimeDelta);

	//m_pBuffer->Render(fTimeDelta);
}

XMFLOAT3 Player::GetPosition()
{
	return dynamic_cast<Transform*>(m_mapComponent["Transform"])->GetPosition();
}

void Player::UpdateSkinnedAnimation(const float fTimeDelta)
{
}

void Player::KeyInput()
{
	KeyPress(); // 누르고 있음
	KeyDown(); // 누름 1번
	KeyUp(); //  뗌
}

void Player::KeyPress()
{
	DWORD dkey = 0;
	// 고민되네 키매니저 어케하고 move 어케할지
	if (KeyMgr::GetInstance()->KeyPressing('W')) dkey |= KEY_W;
	if (KeyMgr::GetInstance()->KeyPressing('A')) dkey |= KEY_A;
	if (KeyMgr::GetInstance()->KeyPressing('S')) dkey |= KEY_S;
	if (KeyMgr::GetInstance()->KeyPressing('D')) dkey |= KEY_D;
	if (KeyMgr::GetInstance()->KeyPressing(VK_SPACE)) dkey |= KEY_SPACE;

	// 상태 패턴 구현해야할듯
	int check = dynamic_cast<Transform*>(m_mapComponent["Transform"])->KeyInput(dkey);
	
}

void Player::KeyDown()
{


}

void Player::KeyUp()
{


}
