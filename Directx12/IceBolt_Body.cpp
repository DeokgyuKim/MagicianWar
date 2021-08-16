#include "IceBolt_Body.h"
#include "Transform.h"
#include "Mesh.h"
#include "Material.h"
#include "Shader.h"

#include "Renderer.h"

IceBolt_Body::IceBolt_Body(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, Object* pParent)
	: SkillEff(device, cmdLst, pRenderer, pParent)
{
	Initialize();
	m_eRenderType = RENDER_TYPE::RENDER_DEFFSKILL;
}

IceBolt_Body::~IceBolt_Body()
{
}

void IceBolt_Body::BuildComponent()
{
	Component* pComponent = new Transform(XMFLOAT3(0.35f, 0.35f, 0.35f), XMFLOAT3(90.f, 0.f, 0.f), XMFLOAT3(0.f, 2.f, 0.f));
	m_mapComponent["Transform"] = pComponent;

	dynamic_cast<Transform*>(m_mapComponent["Transform"])->SetParentMatrix(dynamic_cast<Transform*>(m_pParent->GetTransController())->GetWorldMatrixPointer());


	pComponent = new Mesh(m_pDevice, m_pCmdLst, m_pRenderer->GetHeap(), "Sphere");
	m_mapComponent["Mesh"] = pComponent;
	pComponent = new MaterialCom("IceBoltBodyEff", XMFLOAT4(1.f, 1.f, 1.f, 1.f), XMFLOAT4(1.f, 1.f, 1.f, 1.f), XMFLOAT4(0.f, 0.f, 0.f, 0.f));
	m_mapComponent["Material"] = pComponent;

	m_MaterialIndex = dynamic_cast<MaterialCom*>(m_mapComponent["Material"])->GetMaterialIndex();
}

void IceBolt_Body::AddTexturesName()
{
	m_lstTextureName.push_back("Lava");
	m_lstTextureName.push_back("Ice2");
	m_lstTextureName.push_back("Noise4");
}

void IceBolt_Body::BuildShaders()
{
	m_pShader = new Shader;
	vector<D3D12_INPUT_ELEMENT_DESC> layout = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
	m_pShader->BuildShadersAndInputLayout(L"Skill.hlsl", "VS_ICESphere_MESH", L"Skill.hlsl", "PS_IceBolt_Body", layout);
	m_pShader->BuildPipelineState(m_pDevice, m_pRenderer->GetRootSignature(), 7, true, true, false, 4);
}

void IceBolt_Body::BuildConstantBuffers()
{
	m_ObjectCB = make_unique<UploadBuffer<ObjectCB>>(m_pDevice, 1, true);
	m_SkillCB = make_unique<UploadBuffer<SkillCB>>(m_pDevice, 1, true);
}

int IceBolt_Body::Update(const float& fTimeDelta)
{
	SkillEff::Update(fTimeDelta);
	m_fTime += fTimeDelta;
	return 0;
}

void IceBolt_Body::LateUpdate(const float& fTimeDelta)
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

void IceBolt_Body::Render(const float& fTimeDelta, int _instanceCount)
{
	SkillEff::Render(fTimeDelta);
}
