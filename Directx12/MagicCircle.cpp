#include "MagicCircle.h"

#include "Renderer.h"

#include "Transform.h"
#include "Mesh.h"
#include "Material.h"
#include "Plane.h"
#include "Shader.h"

MagicCircle::MagicCircle(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, 
	XMFLOAT3 pos, UINT idx)
	: SkillEff(device, cmdLst, pRenderer, NULL)
{
	Initialize();
	dynamic_cast<Transform*>(m_mapComponent["Transform"])->SetPosition(pos);
	m_iSkillIdx = idx;
}

MagicCircle::~MagicCircle()
{
}

void MagicCircle::BuildComponent()
{
	Component* pComponent = new Transform(XMFLOAT3(1.f, 1.f, 1.f), XMFLOAT3(0.f, 90.f, 0.f), XMFLOAT3(0.f, 0.f, 0.f));
	m_mapComponent["Transform"] = pComponent;

	//dynamic_cast<Transform*>(m_mapComponent["Transform"])->SetParentMatrix(dynamic_cast<Transform*>(m_pParent->GetTransController())->GetWorldMatrixPointer());


	pComponent = new Plane(m_pDevice, m_pCmdLst, m_pRenderer->GetHeap(), 0.f, 0.f, false);//new Mesh(m_pDevice, m_pCmdLst, m_pRenderer->GetHeap(), "FireWall");
	m_mapComponent["Mesh"] = pComponent;

	pComponent = new MaterialCom("FireEff", XMFLOAT4(1.f, 1.f, 1.f, 1.f), XMFLOAT4(1.f, 1.f, 1.f, 1.f), XMFLOAT4(0.f, 0.f, 0.f, 0.f));
	m_mapComponent["Material"] = pComponent;

	m_MaterialIndex = dynamic_cast<MaterialCom*>(m_mapComponent["Material"])->GetMaterialIndex();
}

void MagicCircle::AddTexturesName()
{
	m_lstTextureName.push_back("MagicCircle");
	m_lstTextureName.push_back("MagicCircleNoise");
}

void MagicCircle::BuildShaders()
{
	m_pShader = new Shader;
	vector<D3D12_INPUT_ELEMENT_DESC> layout = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
	m_pShader->BuildShadersAndInputLayout(L"Skill.hlsl", "VS_Flames_FireEff", L"Skill.hlsl", "PS_Magic_Circle", layout);
	m_pShader->BuildPipelineState(m_pDevice, m_pRenderer->GetRootSignature(), 5, true, true, true);
}

void MagicCircle::BuildConstantBuffers()
{
	m_ObjectCB = make_unique<UploadBuffer<ObjectCB>>(m_pDevice, 1, true);
	m_SkillCB = make_unique<UploadBuffer<SkillCB>>(m_pDevice, 1, true);
}

int MagicCircle::Update(const float& fTimeDelta)
{
	m_fTime += fTimeDelta;
	if (m_fTime >= 2.f)
	{
		m_fTime -= 2.f;
	}
	SkillEff::Update(fTimeDelta);
	return 0;
}

void MagicCircle::LateUpdate(const float& fTimeDelta)
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

void MagicCircle::Render(const float& fTimeDelta, int _instanceCount)
{
	SkillEff::Render(fTimeDelta);
}
