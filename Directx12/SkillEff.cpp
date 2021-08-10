#include "SkillEff.h"

#include "Core.h"
#include "Renderer.h"

#include "TextureMgr.h"
#include "DDSTexture.h"

#include "Transform.h"
#include "Mesh.h"
#include "Material.h"

#include "Shader.h"

SkillEff::SkillEff(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, Object* pParent)
    : Object()
{
    m_pDevice = device;
    m_pCmdLst = cmdLst;
    m_pRenderer = pRenderer;
    m_pParent = pParent;
}

SkillEff::~SkillEff()
{
    Release();
}

void SkillEff::Initialize()
{
	Core::GetInstance()->CmdLstReset();

    BuildComponent();
    BuildShaders();
    AddTexturesName();
    BuildConstantBuffers();

    m_bInstanced = false;

	Core::GetInstance()->CmdLstExecute();
	Core::GetInstance()->WaitForGpuComplete();
}

void SkillEff::Release()
{
    delete m_pShader;
}

int SkillEff::Update(const float& fTimeDelta)
{
    if (m_bDead)
        return -1;
    Object::Update(fTimeDelta);
    return 0;
}

void SkillEff::LateUpdate(const float& fTimeDelta)
{
    Object::LateUpdate(fTimeDelta);
    m_pRenderer->PushObject(m_eRenderType, this);
}

void SkillEff::Render(const float& fTimeDelta, int _instanceCount)
{
    m_pShader->PreRender(m_pCmdLst);
    int idx = 0;
    for (auto iter = m_lstTextureName.begin(); iter != m_lstTextureName.end(); ++iter, ++idx)
        TextureMgr::GetInstance()->GetTexture(*iter)->PreRender(m_pCmdLst, m_pRenderer->GetHeap(), idx + 13);
    m_pCmdLst->SetGraphicsRootConstantBufferView(19, m_ObjectCB->Resource()->GetGPUVirtualAddress());
    m_pCmdLst->SetGraphicsRootConstantBufferView(18, m_SkillCB->Resource()->GetGPUVirtualAddress());

    Object::Render(fTimeDelta);
}
