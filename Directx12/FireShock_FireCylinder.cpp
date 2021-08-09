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
	m_DissolveTime = 6.5f;
	m_Time1 = 0;
}

FireShock_FireCylinder::~FireShock_FireCylinder()
{
}

void FireShock_FireCylinder::BuildComponent()
{
	Component* pComponent = new Transform(XMFLOAT3(0.2f, 0.2f, 1.f), XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(0.f, 0.f, 0.f));
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
	m_lstTextureName.push_back("Lava");
	m_lstTextureName.push_back("Noise");
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
	XMFLOAT3 Scale = dynamic_cast<Transform*>(m_mapComponent["Transform"])->GetScale();
	XMFLOAT3 Rotate = dynamic_cast<Transform*>(m_mapComponent["Transform"])->GetRotate();

	if (m_DissolveTime < 7.8f) { // 디졸브 생성
		m_Time1 += 0.01;
		m_DissolveTime += (pow(10, m_Time1) * 0.1f) * fTimeDelta;
	}
	else if (m_DissolveTime <= 9.f) {
		m_Time1 -= 0.01;
		m_DissolveTime += (pow(10, m_Time1) * 0.1f) * fTimeDelta;
	}
	else if (m_DissolveTime >= 9.f) {
		m_DissolveTime = 6.5f;
		m_Time1 = 0;
		m_fTime = 0.f;
	}
	//6.5 에서 7.6
	//cout << "현재 시간 - " << m_fTime << endl;
	//Scale.z += fTimeDelta;
	//if (Scale.z >= 1.f) {
	//	Scale.z = 0.f;
	//}
	//Rotate.z += 180.f*fTimeDelta;

	//cout << "( " << Rotate.x << ", " << Rotate.y << ", " << Rotate.z << " )" << endl;
	//if (Rotate.y >= 1.f) {
	//	//Rotate.y = 0.f;
	//}
	dynamic_cast<Transform*>(m_mapComponent["Transform"])->SetScale(Scale);
	dynamic_cast<Transform*>(m_mapComponent["Transform"])->SetRotate(Rotate);
	SkillEff::Update(fTimeDelta);
	return 0;
}

void FireShock_FireCylinder::LateUpdate(const float& fTimeDelta)
{
	SkillEff::LateUpdate(fTimeDelta);

	ObjectCB objCB;
	XMMATRIX world = dynamic_cast<Transform*>(m_mapComponent["Transform"])->GetWorldMatrix();
	XMStoreFloat4x4(&objCB.World, (world));
	objCB.MaterialIndex = m_MaterialIndex;
	m_ObjectCB->CopyData(0, objCB);


	SkillCB skillcb;
	skillcb.fTime = m_fTime;
	skillcb.DissolveC = m_DissolveTime;
	m_SkillCB->CopyData(0, skillcb);

}

void FireShock_FireCylinder::Render(const float& fTimeDelta, int _instanceCount)
{
	SkillEff::Render(fTimeDelta);
}
