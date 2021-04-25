#include "FireShock_FireCylinder.h"

#include "Renderer.h"

#include "Transform.h"
#include "Mesh.h"
#include "Material.h"
#include "Shader.h"

FireShock_FireCylinder::FireShock_FireCylinder(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, Object* pParent)
	: SkillEff(device, cmdLst, pRenderer, pParent)
{
	Initialize();
}

FireShock_FireCylinder::~FireShock_FireCylinder()
{
}

void FireShock_FireCylinder::BuildComponent()
{
	Component* pComponent = new Transform(XMFLOAT3(1.f, 1.f, 1.f), XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(0.f, 0.f, 0.f));
	m_mapComponent["Transform"] = pComponent;

	dynamic_cast<Transform*>(m_mapComponent["Transform"])->SetParentMatrix(dynamic_cast<Transform*>(m_pParent->GetTransController())->GetWorldMatrixPointer());


	pComponent = new Mesh(m_pDevice, m_pCmdLst, m_pRenderer->GetHeap(), "Cylinder");
	m_mapComponent["Mesh"] = pComponent;
	pComponent = new MaterialCom("FireEff", XMFLOAT4(1.f, 1.f, 1.f, 1.f), XMFLOAT4(1.f, 1.f, 1.f, 1.f), XMFLOAT4(0.f, 0.f, 0.f, 0.f));
	m_mapComponent["Material"] = pComponent;

	m_MaterialIndex = dynamic_cast<MaterialCom*>(m_mapComponent["Material"])->GetMaterialIndex();
}

void FireShock_FireCylinder::AddTexturesName()
{
	m_lstTextureName.push_back("FireBall3");
	m_lstTextureName.push_back("BoomParticle");
}

void FireShock_FireCylinder::BuildShaders()
{
	m_pShader = new Shader;
	vector<D3D12_INPUT_ELEMENT_DESC> layout = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
	m_pShader->BuildShadersAndInputLayout(L"Skill.hlsl", "VS_FireShock_FireCylinder", L"Skill.hlsl", "PS_FireShock_FireCylinder", layout);
	m_pShader->BuildPipelineState(m_pDevice, m_pRenderer->GetRootSignature(), 5, true, true, true);
}

void FireShock_FireCylinder::BuildConstantBuffers()
{
	m_ObjectCB = make_unique<UploadBuffer<ObjectCB>>(m_pDevice, 1, true);
	m_SkillCB = make_unique<UploadBuffer<SkillCB>>(m_pDevice, 1, true);
}

int FireShock_FireCylinder::Update(const float& fTimeDelta)
{
	m_fTime += fTimeDelta;
	SkillEff::Update(fTimeDelta);
	return 0;
}

void FireShock_FireCylinder::LateUpdate(const float& fTimeDelta)
{
	SkillEff::LateUpdate(fTimeDelta);

	ObjectCB objCB;
	XMMATRIX world = dynamic_cast<Transform*>(m_mapComponent["Transform"])->GetWorldMatrix();
	XMStoreFloat4x4(&objCB.World, XMMatrixTranspose(world));
	objCB.MaterialIndex = m_MaterialIndex;
	m_ObjectCB->CopyData(0, objCB);


	SkillCB skillcb;
	skillcb.fTime = m_fTime;
	m_SkillCB->CopyData(0, skillcb);

}

void FireShock_FireCylinder::Render(const float& fTimeDelta, int _instanceCount)
{
	SkillEff::Render(fTimeDelta);
}
