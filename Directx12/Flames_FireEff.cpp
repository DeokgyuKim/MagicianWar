#include "Flames_FireEff.h"

#include "Renderer.h"

#include "Transform.h"
#include "Mesh.h"
#include "Material.h"
#include "Plane.h"
#include "Shader.h"

Flames_FireEff::Flames_FireEff(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, Object* pParent, 
	XMFLOAT3 pos, UINT idx)
	: SkillEff(device, cmdLst, pRenderer, pParent)
{
	Initialize();
	dynamic_cast<Transform*>(m_mapComponent["Transform"])->SetPosition(pos);
	m_iSkillIdx = idx;
}

Flames_FireEff::~Flames_FireEff()
{
}

void Flames_FireEff::BuildComponent()
{
	Component* pComponent = new Transform(XMFLOAT3(1.f, 4.f, 10.f), XMFLOAT3(0.f, 90.f, 0.f), XMFLOAT3(0.f, 0.f, 0.f));
	m_mapComponent["Transform"] = pComponent;

	dynamic_cast<Transform*>(m_mapComponent["Transform"])->SetParentMatrix(dynamic_cast<Transform*>(m_pParent->GetTransController())->GetWorldMatrixPointer());


	pComponent = new Plane(m_pDevice, m_pCmdLst, m_pRenderer->GetHeap(), 0.f, 0.f, false);//new Mesh(m_pDevice, m_pCmdLst, m_pRenderer->GetHeap(), "FireWall");
	m_mapComponent["Mesh"] = pComponent;

	pComponent = new MaterialCom("FireEff", XMFLOAT4(1.f, 1.f, 1.f, 1.f), XMFLOAT4(1.f, 1.f, 1.f, 1.f), XMFLOAT4(0.f, 0.f, 0.f, 0.f));
	m_mapComponent["Material"] = pComponent;

	m_MaterialIndex = dynamic_cast<MaterialCom*>(m_mapComponent["Material"])->GetMaterialIndex();
}

void Flames_FireEff::AddTexturesName()
{
	m_lstTextureName.push_back("FireMakeLong");
	m_lstTextureName.push_back("Noise");
	m_lstTextureName.push_back("Noise2");
	m_lstTextureName.push_back("Noise3");
	m_lstTextureName.push_back("Noise4");
}

void Flames_FireEff::BuildShaders()
{
	m_strSkillName = "Flames_FireEff"; 
}

void Flames_FireEff::BuildConstantBuffers()
{
	m_ObjectCB = make_unique<UploadBuffer<ObjectCB>>(m_pDevice, 1, true);
	m_SkillCB = make_unique<UploadBuffer<SkillCB>>(m_pDevice, 1, true);
}

int Flames_FireEff::Update(const float& fTimeDelta)
{
	m_fTime += fTimeDelta;
	SkillEff::Update(fTimeDelta);
	return 0;
}

void Flames_FireEff::LateUpdate(const float& fTimeDelta)
{
	SkillEff::LateUpdate(fTimeDelta);

	ObjectCB objCB;
	XMMATRIX world = dynamic_cast<Transform*>(m_mapComponent["Transform"])->GetWorldMatrix();
	XMStoreFloat4x4(&objCB.World, (world));
	objCB.MaterialIndex = m_MaterialIndex;
	m_ObjectCB->CopyData(0, objCB);


	SkillCB skillcb;
	skillcb.fTime = m_fTime;
	skillcb.SkillIdx = m_iSkillIdx;
	m_SkillCB->CopyData(0, skillcb);

}

void Flames_FireEff::Render(const float& fTimeDelta, int _instanceCount)
{
	SkillEff::Render(fTimeDelta);
}
