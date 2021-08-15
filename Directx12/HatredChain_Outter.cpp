#include "HatredChain_Outter.h"

#include "Renderer.h"

#include "Transform.h"
#include "Mesh.h"
#include "Material.h"
#include "Plane.h"
#include "Shader.h"

#include "MainApp.h"
#include "Scene.h"
#include "Camera.h"

HatredChain_Outter::HatredChain_Outter(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, Object* pParent)
	: SkillEff(device, cmdLst, pRenderer, pParent)
{
	Initialize();
}

HatredChain_Outter::~HatredChain_Outter()
{
}

void HatredChain_Outter::BuildComponent()
{
	Component* pComponent = new Transform(XMFLOAT3(3.f, 3.f, 3.f), XMFLOAT3(0.f, 90.f, 0.f), XMFLOAT3(0.f, 0.f, 0.f));
	m_mapComponent["Transform"] = pComponent;

	dynamic_cast<Transform*>(m_mapComponent["Transform"])->SetParentMatrix(dynamic_cast<Transform*>(m_pParent->GetTransController())->GetWorldMatrixPointer());


	pComponent = new Plane(m_pDevice, m_pCmdLst, m_pRenderer->GetHeap(), 0.f, 0.f, false);
	m_mapComponent["Mesh"] = pComponent;

	pComponent = new MaterialCom("HatredChainOutterEff", XMFLOAT4(1.f, 1.f, 1.f, 1.f), XMFLOAT4(1.f, 1.f, 1.f, 1.f), XMFLOAT4(0.f, 0.f, 0.f, 0.f));
	m_mapComponent["Material"] = pComponent;

	m_MaterialIndex = dynamic_cast<MaterialCom*>(m_mapComponent["Material"])->GetMaterialIndex();
}

void HatredChain_Outter::AddTexturesName()
{
	m_lstTextureName.push_back("Noise2");
	m_lstTextureName.push_back("Noise4");
}

void HatredChain_Outter::BuildShaders()
{
	m_pShader = new Shader;
	vector<D3D12_INPUT_ELEMENT_DESC> layout = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
	m_pShader->BuildShadersAndInputLayout(L"Skill.hlsl", "VS_Flames_FireEff", L"Skill.hlsl", "PS_HatredChain_Outter", layout);
	m_pShader->BuildPipelineStateBlendOp(m_pDevice, m_pRenderer->GetRootSignature(), 2, true, true, false);
}

void HatredChain_Outter::BuildConstantBuffers()
{
	m_ObjectCB = make_unique<UploadBuffer<ObjectCB>>(m_pDevice, 1, true);
	m_SkillCB = make_unique<UploadBuffer<SkillCB>>(m_pDevice, 1, true);
}

int HatredChain_Outter::Update(const float& fTimeDelta)
{
	m_fTime += fTimeDelta;
	SkillEff::Update(fTimeDelta);
	return 0;
}

void HatredChain_Outter::LateUpdate(const float& fTimeDelta)
{
	SkillEff::LateUpdate(fTimeDelta);

	ObjectCB objCB;
	//XMMATRIX world = dynamic_cast<Transform*>(m_mapComponent["Transform"])->GetWorldMatrix();


	XMMATRIX world = dynamic_cast<Transform*>(m_mapComponent["Transform"])->GetWorldMatrix();
	XMMATRIX scale = dynamic_cast<Transform*>(m_mapComponent["Transform"])->GetScaleMatrix();
	XMMATRIX invscale = dynamic_cast<Transform*>(m_mapComponent["Transform"])->GetInvScaleMatrix();
	XMFLOAT4X4 view = dynamic_cast<Camera*>(MainApp::GetInstance()->GetScene()->GetCamera())->GetViewMatrix();

	XMFLOAT4X4 rot = MathHelper::Identity4x4();
	rot._11 = view._11; /*rot._12 = view._12;*/ rot._13 = view._13;
	//rot._21 = view._21; rot._22 = view._22; rot._33 = view._33;
	rot._31 = view._31; /*rot._32 = view._32;*/ rot._33 = view._33;

	XMMATRIX bill = XMMatrixInverse(nullptr, XMLoadFloat4x4(&rot));

	world = scale * bill * invscale * world;
	XMStoreFloat4x4(&objCB.World, (world));
	objCB.MaterialIndex = m_MaterialIndex;
	m_ObjectCB->CopyData(0, objCB);


	SkillCB skillcb;
	skillcb.fTime = m_fTime;
	skillcb.SkillIdx = m_iSkillIdx;
	skillcb.SkillPos = dynamic_cast<Transform*>(m_mapComponent["Transform"])->GetPosition();
	m_SkillCB->CopyData(0, skillcb);

}

void HatredChain_Outter::Render(const float& fTimeDelta, int _instanceCount)
{
	SkillEff::Render(fTimeDelta);
}
