#include "CLight.h"
#include "LightGeo.h"
#include "Renderer.h"
#include "Core.h"

CLight::CLight(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, ID3D12DescriptorHeap* heap, Renderer* pRenderer,
	XMFLOAT4 diff, XMFLOAT4 ambi, XMFLOAT4 spec, XMFLOAT4 pos, XMFLOAT4 dir)
{
	m_pDevice = device;
	m_pCmdLst = cmdLst;
	m_pDescriptorHeap = heap;

	m_pRenderer = pRenderer;

	m_xmfDiffuse = diff;
	m_xmfAmbient = ambi;
	m_xmfSpecular = spec;
	m_xmfPosition = pos;
	XMStoreFloat4(&m_xmfDirection, XMVector4Normalize(XMLoadFloat4(&dir)));

	Initialize();
}

CLight::~CLight()
{
}

void CLight::Initialize()
{
	Core::GetInstance()->CmdLstReset();
	m_pLightGeo = new LightGeo(m_pDevice, m_pCmdLst, m_pDescriptorHeap, WINCX, WINCY);
	Core::GetInstance()->CmdLstExecute();
	Core::GetInstance()->WaitForGpuComplete();

	BuildConstantBuffer();
}

void CLight::BuildConstantBuffer()
{
	int boxCBufIndex = 0;

	XMMATRIX proj = XMMatrixPerspectiveFovLH(XMConvertToRadians(90.f), (float)WINCX / (float)WINCY, 0.1f, 1000.0f);
	XMMATRIX ortho = XMMatrixOrthographicLH(WINCX, WINCY, 0.1f, 1000.f);


	XMVECTOR pos = XMVectorSet(m_xmfPosition.x, m_xmfPosition.y, m_xmfPosition.z, 1.0f);
	XMVECTOR target = XMVectorSet(m_xmfPosition.x + m_xmfDirection.x, m_xmfPosition.y + m_xmfDirection.y, m_xmfPosition.z + m_xmfDirection.z, 1.0f);
	//XMVECTOR target = XMVectorSet(m_xmfLookVec.x, m_xmfLookVec.y, m_xmfLookVec.z, 1.0f);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // XMVectorSet(m_xmfUpVec.x, m_xmfUpVec.y, m_xmfUpVec.z, 0.0f);

	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);


	///Diffuse
	m_LightCB = make_unique<UploadBuffer<LightCB>>(m_pDevice, 1, true);

	LightCB		lightcb;
	XMStoreFloat4(&lightcb.Diffuse, XMLoadFloat4(&m_xmfDiffuse));
	XMStoreFloat4(&lightcb.Ambient, XMLoadFloat4(&m_xmfAmbient));
	XMStoreFloat4(&lightcb.Specular, XMLoadFloat4(&m_xmfSpecular));
	XMStoreFloat4(&lightcb.Position, XMLoadFloat4(&m_xmfPosition));
	XMStoreFloat4(&lightcb.Direction, XMLoadFloat4(&m_xmfDirection));
	XMStoreFloat4x4(&lightcb.LightView, (view));
	XMStoreFloat4x4(&lightcb.LightProj, (proj));
	m_LightCB->CopyData(0, lightcb);

}

void CLight::SetLightMaterial()
{
	m_pCmdLst->SetGraphicsRootConstantBufferView(3, m_LightCB->Resource()->GetGPUVirtualAddress());
}

void CLight::RenderLight()
{
	SetLightMaterial();
	m_pLightGeo->Render(0.f);
}
