#include "Boom_Inner.h"

#include "Renderer.h"

#include "Transform.h"
#include "Mesh.h"
#include "Material.h"
#include "Plane.h"
#include "Shader.h"

#include "MainApp.h"
#include "Scene.h"
#include "Camera.h"

Boom_Inner::Boom_Inner(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, float goalScale, Object* pParent)
	: SkillEff(device, cmdLst, pRenderer, pParent)
{
	Initialize();
	m_eRenderType = RENDER_TYPE::RENDER_DEFFSKILL;
	m_fGoalScale = goalScale;
}

Boom_Inner::~Boom_Inner()
{
}

void Boom_Inner::BuildComponent()
{
	Component* pComponent = new Transform(XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(0.f, 0.f, 0.f));
	m_mapComponent["Transform"] = pComponent;

	dynamic_cast<Transform*>(m_mapComponent["Transform"])->SetParentMatrix(dynamic_cast<Transform*>(m_pParent->GetTransController())->GetWorldMatrixPointer());


	pComponent = new Mesh(m_pDevice, m_pCmdLst, m_pRenderer->GetHeap(), "Sphere");
	m_mapComponent["Mesh"] = pComponent;
	pComponent = new MaterialCom("BoomInnerEff", XMFLOAT4(1.f, 1.f, 1.f, 1.f), XMFLOAT4(1.f, 1.f, 1.f, 1.f), XMFLOAT4(0.f, 0.f, 0.f, 0.f));
	m_mapComponent["Material"] = pComponent;

	m_MaterialIndex = dynamic_cast<MaterialCom*>(m_mapComponent["Material"])->GetMaterialIndex();
}

void Boom_Inner::AddTexturesName()
{
	m_lstTextureName.push_back("IceBall");
	//m_lstTextureName.push_back("Noise5");
}

void Boom_Inner::BuildShaders()
{
	m_pShader = new Shader;
	vector<D3D12_INPUT_ELEMENT_DESC> layout = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
	m_pShader->BuildShadersAndInputLayout(L"Skill.hlsl", "VS_ICESphere_MESH", L"Skill.hlsl", "PS_Boom_Inner", layout);
	m_pShader->BuildPipelineState(m_pDevice, m_pRenderer->GetRootSignature(), 7, true, true, false, 4);
}

void Boom_Inner::BuildConstantBuffers()
{
	m_ObjectCB = make_unique<UploadBuffer<ObjectCB>>(m_pDevice, 1, true);
	m_SkillCB = make_unique<UploadBuffer<SkillCB>>(m_pDevice, 1, true);
}

int Boom_Inner::Update(const float& fTimeDelta)
{
	SkillEff::Update(fTimeDelta);
	m_fTime += fTimeDelta * 2.f;
	if (m_fTime >= 1.f)
		m_fTime -= 1.f;

	m_fCurScale = (sin(m_fTime * XMConvertToRadians(180.f))) * m_fGoalScale * 2.f;
	m_fCurScale = min(m_fCurScale, m_fGoalScale);
	
	dynamic_cast<Transform*>(m_mapComponent["Transform"])->SetScale(XMFLOAT3(m_fCurScale, m_fCurScale, m_fCurScale));

	return 0;
}

void Boom_Inner::LateUpdate(const float& fTimeDelta)
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
	skillcb.SkillPos = dynamic_cast<Transform*>(m_mapComponent["Transform"])->GetPosition();
	m_SkillCB->CopyData(0, skillcb);

}

void Boom_Inner::Render(const float& fTimeDelta, int _instanceCount)
{
	SkillEff::Render(fTimeDelta);
}
