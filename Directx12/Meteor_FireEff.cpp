#include "Meteor_FireEff.h"
#include "Transform.h"
#include "Mesh.h"
#include "Material.h"
#include "Shader.h"

#include "Renderer.h"

Meteor_FireEff::Meteor_FireEff(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, Object* pParent, XMFLOAT3 rotate, int dir)
	: SkillEff(device, cmdLst, pRenderer, pParent)
{
	Initialize();
	dynamic_cast<Transform*>(m_mapComponent["Transform"])->SetRotate(rotate);
	m_iRotate = dir;
	//m_eRenderType = RENDER_TYPE::RENDER_DEFFSKILL;
}

Meteor_FireEff::~Meteor_FireEff()
{
}

void Meteor_FireEff::BuildComponent()
{
	Component* pComponent = new Transform(XMFLOAT3(0.05f, 0.05f, 0.05f), XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(0.f, 0.f, -2.f));
	m_mapComponent["Transform"] = pComponent;

	dynamic_cast<Transform*>(m_mapComponent["Transform"])->SetParentMatrix(dynamic_cast<Transform*>(m_pParent->GetTransController())->GetWorldMatrixPointer());


	pComponent = new Mesh(m_pDevice, m_pCmdLst, m_pRenderer->GetHeap(), "Eff_Mesh_FireTail");
	m_mapComponent["Mesh"] = pComponent;
	pComponent = new MaterialCom("MeteorBodyEff", XMFLOAT4(1.f, 1.f, 1.f, 1.f), XMFLOAT4(1.f, 1.f, 1.f, 1.f), XMFLOAT4(0.f, 0.f, 0.f, 0.f));
	m_mapComponent["Material"] = pComponent;

	m_MaterialIndex = dynamic_cast<MaterialCom*>(m_mapComponent["Material"])->GetMaterialIndex();
}

void Meteor_FireEff::AddTexturesName()
{
	m_lstTextureName.push_back("FireMake");
	m_lstTextureName.push_back("Noise4");
}

void Meteor_FireEff::BuildShaders()
{
	m_strSkillName = "Meteor_FireEff";
}

void Meteor_FireEff::BuildConstantBuffers()
{
	m_ObjectCB = make_unique<UploadBuffer<ObjectCB>>(m_pDevice, 1, true);
	m_SkillCB = make_unique<UploadBuffer<SkillCB>>(m_pDevice, 1, true);
}

int Meteor_FireEff::Update(const float& fTimeDelta)
{
	SkillEff::Update(fTimeDelta);
	m_fTime += fTimeDelta;
	XMFLOAT3 fRotate = dynamic_cast<Transform*>(GetTransController())->GetRotate();
	fRotate.z += fTimeDelta * 100.f * m_iRotate;
	dynamic_cast<Transform*>(GetTransController())->SetRotate(fRotate);

	return 0;
}

void Meteor_FireEff::LateUpdate(const float& fTimeDelta)
{
	SkillEff::LateUpdate(fTimeDelta);

	ObjectCB objCB;
	XMMATRIX world = dynamic_cast<Transform*>(m_mapComponent["Transform"])->GetWorldMatrix();
	XMStoreFloat4x4(&objCB.World, (world));
	objCB.MaterialIndex = m_MaterialIndex;
	m_ObjectCB->CopyData(0, objCB);


	SkillCB skillcb;
	skillcb.fTime = m_fTime;
	skillcb.DissolveC = 0.f;
	m_SkillCB->CopyData(0, skillcb);
}

void Meteor_FireEff::Render(const float& fTimeDelta, int _instanceCount)
{
	SkillEff::Render(fTimeDelta);
}
