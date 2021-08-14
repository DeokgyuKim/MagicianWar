#include "BillBoardEff.h"

#include "Core.h"
#include "Renderer.h"

#include "TextureMgr.h"
#include "DDSTexture.h"

#include "Transform.h"
#include "Mesh.h"
#include "Material.h"

#include "Shader.h"

BillBoardEff::BillBoardEff(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, Object* pParent)
    : SkillEff(device, cmdLst, pRenderer, pParent)
{
    m_pDevice = device;
    m_pCmdLst = cmdLst;
    m_pRenderer = pRenderer;
    m_pParent = pParent;
}

BillBoardEff::~BillBoardEff()
{
    Release();
}

void BillBoardEff::Initialize()
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

void BillBoardEff::Release()
{
    delete m_pShader;
}

int BillBoardEff::Update(const float& fTimeDelta)
{
    if (m_bDead)
        return -1;
    Object::Update(fTimeDelta);
    return 0;
}

void BillBoardEff::LateUpdate(const float& fTimeDelta)
{
    Object::LateUpdate(fTimeDelta);
    m_pRenderer->PushObject(m_eRenderType, this);
}

void BillBoardEff::Render(const float& fTimeDelta, int _instanceCount)
{
    m_pShader->PreRender(m_pCmdLst);
    int idx = 0;
    for (auto iter = m_lstTextureName.begin(); iter != m_lstTextureName.end(); ++iter, ++idx)
        TextureMgr::GetInstance()->GetTexture(*iter)->PreRender(m_pCmdLst, m_pRenderer->GetHeap(), idx + 13);
    m_pCmdLst->SetGraphicsRootConstantBufferView(19, m_ObjectCB->Resource()->GetGPUVirtualAddress());
    m_pCmdLst->SetGraphicsRootConstantBufferView(18, m_SkillCB->Resource()->GetGPUVirtualAddress());

    Object::Render(fTimeDelta);
}
