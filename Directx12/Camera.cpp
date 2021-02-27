#include "Camera.h"
#include "Renderer.h"

Camera::Camera(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer)
{
	m_fRotX = 0.f;
	m_fRotY = 0.f;
	m_pDevice = device;
	m_pCmdLst = cmdLst;
	m_pRenderer = pRenderer;
	Initialize();
}

Camera::~Camera()
{
}

int Camera::Update(const float& fTimeDelta)
{
	if (GetAsyncKeyState(VK_F1) & 0x0001)
		m_bFree = !m_bFree;

	if (!m_bFree)
		return 0;
	///Rotate
	GetCursorPos(&m_ptCur);
	//if ((m_ptCur.x - m_ptOld.x) == 0.f && (m_ptCur.y - m_ptOld.y) == 0.f)
	//	return 0;

	m_fRotX += (m_ptCur.x - m_ptOld.x) * 0.2f;
	m_fRotY += (m_ptCur.y - m_ptOld.y) * 0.2f;

	if (m_fRotX >= 180.f)
		m_fRotX -= 360.f;
	if (m_fRotX <= -180.f)
		m_fRotX += 360.f;

	m_fRotY = min(m_fRotY, 85.f);
	m_fRotY = max(m_fRotY, -85.f);


	XMMATRIX matRotX = XMMatrixRotationY(XMConvertToRadians(m_fRotX));
	XMMATRIX matRotY = XMMatrixRotationX(XMConvertToRadians(m_fRotY));
	XMMATRIX matRot = matRotY * matRotX;

	XMFLOAT3 look = XMFLOAT3(0.f, 0.f, 1.f);
	XMFLOAT3 right = XMFLOAT3(1.f, 0.f, 0.f);
	XMFLOAT3 up = XMFLOAT3(0.f, 1.f, 0.f);
	XMStoreFloat3(&m_xmfLookVec, XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&look), matRot)));
	XMStoreFloat3(&m_xmfRightVec, XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&right), matRot)));
	XMStoreFloat3(&m_xmfUpVec, XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&up), matRot)));
	
	SetCursorPos(WINCX, WINCY);

	///Movement
	if (GetAsyncKeyState('W') & 0x8000)
		XMStoreFloat3(&m_xmfPosition, XMLoadFloat3(&m_xmfPosition) + XMLoadFloat3(&m_xmfLookVec) * 0.1f);
	if (GetAsyncKeyState('S') & 0x8000)
		XMStoreFloat3(&m_xmfPosition, XMLoadFloat3(&m_xmfPosition) - XMLoadFloat3(&m_xmfLookVec) * 0.1f);
	if (GetAsyncKeyState('A') & 0x8000)
		XMStoreFloat3(&m_xmfPosition, XMLoadFloat3(&m_xmfPosition) - XMLoadFloat3(&m_xmfRightVec) * 0.1f);
	if (GetAsyncKeyState('D') & 0x8000)
		XMStoreFloat3(&m_xmfPosition, XMLoadFloat3(&m_xmfPosition) + XMLoadFloat3(&m_xmfRightVec) * 0.1f);
    return 0;
}

void Camera::LateUpdate(const float& fTimeDelta)
{
	XMMATRIX proj = XMMatrixPerspectiveFovLH(XMConvertToRadians(90.f), 800.f / 600.f, 1.0f, 1000.0f);

	XMStoreFloat4x4(&m_xmmProj, proj);


	XMStoreFloat3(&m_xmfTarget, XMLoadFloat3(&m_xmfPosition) + XMLoadFloat3(&m_xmfLookVec));

	XMVECTOR pos = XMVectorSet(m_xmfPosition.x, m_xmfPosition.y, m_xmfPosition.z, 1.0f);
	XMVECTOR target = XMVectorSet(m_xmfTarget.x, m_xmfTarget.y, m_xmfTarget.z, 1.0f); 
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // XMVectorSet(m_xmfUpVec.x, m_xmfUpVec.y, m_xmfUpVec.z, 0.0f);

	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);

	XMStoreFloat4x4(&m_xmmView, view);

	ObjectCB	ObjCBView, ObjCBProj;
	XMStoreFloat4x4(&ObjCBView.World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmmView)));
	m_ObjectCBView->CopyData(0, ObjCBView);

	XMStoreFloat4x4(&ObjCBProj.World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmmProj)));
	m_ObjectCBProj->CopyData(0, ObjCBProj);
}

void Camera::Render(const float& fTimeDelta)
{
	m_pCmdLst->SetGraphicsRootConstantBufferView(1, m_ObjectCBView->Resource()->GetGPUVirtualAddress());
	m_pCmdLst->SetGraphicsRootConstantBufferView(2, m_ObjectCBProj->Resource()->GetGPUVirtualAddress());
}



void Camera::Initialize()
{
	SetCursorPos(WINCX, WINCY);
	m_ptOld.x = WINCX; m_ptOld.y = WINCY;
	m_bFree = false;

	m_xmfPosition = XMFLOAT3(0.f, 3.f, 0.f);

	m_xmfRightVec = XMFLOAT3(1.f, 0.f, 0.f);
	m_xmfUpVec = XMFLOAT3(0.f, 1.f, 0.f);
	m_xmfLookVec = XMFLOAT3(0.f, 0.f, 1.f);
	
	XMStoreFloat3(&m_xmfTarget, XMLoadFloat3(&m_xmfPosition) + XMLoadFloat3(&m_xmfLookVec));

	BuildConstantBufferViewMatrix();
	BuildConstantBufferProjMatrix();
}

void Camera::BuildConstantBufferViewMatrix()
{
	m_ObjectCBView = make_unique<UploadBuffer<ObjectCB>>(m_pDevice, 1, true);

	UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectCB));

	D3D12_GPU_VIRTUAL_ADDRESS cbAddress = m_ObjectCBView->Resource()->GetGPUVirtualAddress();
	// Offset to the ith object constant buffer in the buffer.
	int boxCBufIndex = 0;
	cbAddress += boxCBufIndex * objCBByteSize;

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = cbAddress;
	cbvDesc.SizeInBytes = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectCB));

	m_pRenderer->CreateConstantBufferView(cbvDesc);
	return;
}
void Camera::BuildConstantBufferProjMatrix()
{
	m_ObjectCBProj = make_unique<UploadBuffer<ObjectCB>>(m_pDevice, 1, true);

	UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectCB));

	D3D12_GPU_VIRTUAL_ADDRESS cbAddress = m_ObjectCBProj->Resource()->GetGPUVirtualAddress();
	// Offset to the ith object constant buffer in the buffer.
	int boxCBufIndex = 0;
	cbAddress += boxCBufIndex * objCBByteSize;

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = cbAddress;
	cbvDesc.SizeInBytes = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectCB));

	m_pRenderer->CreateConstantBufferView(cbvDesc);
	return;
}