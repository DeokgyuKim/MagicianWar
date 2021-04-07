#include "Camera.h"
#include "Renderer.h"
#include "Player.h"

Camera::Camera(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer)
{
	m_fRotX = 0.f;
	m_fRotY = 0.f;
	m_pDevice = device;
	m_pCmdLst = cmdLst;
	m_pRenderer = pRenderer;

	m_fOffset = 3.f;
	Initialize();
}

Camera::~Camera()
{
}

int Camera::Update(const float& fTimeDelta)
{
	m_fTime += fTimeDelta;
	if (GetAsyncKeyState(VK_F1) & 0x0001)
		m_eCamMode = CAMERA_MODE::CAMERA_NONE;
	if (GetAsyncKeyState(VK_F2) & 0x0001)
		m_eCamMode = CAMERA_MODE::CAMERA_FREE;
	if (GetAsyncKeyState(VK_F3) & 0x0001)
		m_eCamMode = CAMERA_MODE::CAMERA_THIRD;

	if (m_eCamMode == CAMERA_MODE::CAMERA_FREE || m_eCamMode == CAMERA_MODE::CAMERA_THIRD)
	{
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

		SetCursorPos(WINCX / 2, WINCY / 2);

		if (m_eCamMode == CAMERA_MODE::CAMERA_THIRD)
		{
			XMFLOAT3 xmfPlayerPos = m_pPlayer->GetPosition();
			xmfPlayerPos.y += 1.f;
			XMStoreFloat3(&m_xmfPosition, XMLoadFloat3(&xmfPlayerPos) - XMLoadFloat3(&m_xmfLookVec) * m_fOffset);
		}
		else
		{
			///Movement
			if (GetAsyncKeyState('W') & 0x8000)
				XMStoreFloat3(&m_xmfPosition, XMLoadFloat3(&m_xmfPosition) + XMLoadFloat3(&m_xmfLookVec) * 0.1f);
			if (GetAsyncKeyState('S') & 0x8000)
				XMStoreFloat3(&m_xmfPosition, XMLoadFloat3(&m_xmfPosition) - XMLoadFloat3(&m_xmfLookVec) * 0.1f);
			if (GetAsyncKeyState('A') & 0x8000)
				XMStoreFloat3(&m_xmfPosition, XMLoadFloat3(&m_xmfPosition) - XMLoadFloat3(&m_xmfRightVec) * 0.1f);
			if (GetAsyncKeyState('D') & 0x8000)
				XMStoreFloat3(&m_xmfPosition, XMLoadFloat3(&m_xmfPosition) + XMLoadFloat3(&m_xmfRightVec) * 0.1f);
		}
	}
	else if (m_eCamMode == CAMERA_MODE::CAMERA_THIRD)
	{
		
	}

    return 0;
}

void Camera::LateUpdate(const float& fTimeDelta)
{
	XMMATRIX proj = XMMatrixPerspectiveFovLH(XMConvertToRadians(90.f), 800.f / 600.f, 0.1f, 1000.0f);

	XMStoreFloat4x4(&m_xmmProj, proj);


	XMStoreFloat3(&m_xmfTarget, XMLoadFloat3(&m_xmfPosition) + XMLoadFloat3(&m_xmfLookVec));

	XMVECTOR pos = XMVectorSet(m_xmfPosition.x, m_xmfPosition.y, m_xmfPosition.z, 1.0f);
	XMVECTOR target = XMVectorSet(m_xmfTarget.x, m_xmfTarget.y, m_xmfTarget.z, 1.0f); 
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // XMVectorSet(m_xmfUpVec.x, m_xmfUpVec.y, m_xmfUpVec.z, 0.0f);

	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);

	XMStoreFloat4x4(&m_xmmView, view);

	CameraCB camCB;
	XMStoreFloat4x4(&camCB.View, XMMatrixTranspose(XMLoadFloat4x4(&m_xmmView)));
	XMStoreFloat4x4(&camCB.Proj, XMMatrixTranspose(XMLoadFloat4x4(&m_xmmProj)));
	XMStoreFloat4x4(&camCB.InvView, XMMatrixTranspose(XMMatrixInverse(NULL, XMLoadFloat4x4(&m_xmmView))));
	XMStoreFloat4x4(&camCB.InvProj, XMMatrixTranspose(XMMatrixInverse(NULL, XMLoadFloat4x4(&m_xmmProj))));
	XMFLOAT4 position = XMFLOAT4(m_xmfPosition.x, m_xmfPosition.y, m_xmfPosition.z, 0.f);
	XMStoreFloat4(&camCB.Position, XMLoadFloat4(&position));
	camCB.Time = m_fTime;


	m_ObjectCB->CopyData(0, camCB);
}

void Camera::Render(const float& fTimeDelta)
{
	m_pCmdLst->SetGraphicsRootConstantBufferView(1, m_ObjectCB->Resource()->GetGPUVirtualAddress());
}



void Camera::Initialize()
{
	SetCursorPos(WINCX / 2, WINCY / 2);
	m_ptOld.x = WINCX / 2; m_ptOld.y = WINCY / 2;

	m_xmfPosition = XMFLOAT3(0.f, 3.f, 0.f);

	m_xmfRightVec = XMFLOAT3(1.f, 0.f, 0.f);
	m_xmfUpVec = XMFLOAT3(0.f, 1.f, 0.f);
	m_xmfLookVec = XMFLOAT3(0.f, 0.f, 1.f);
	
	XMStoreFloat3(&m_xmfTarget, XMLoadFloat3(&m_xmfPosition) + XMLoadFloat3(&m_xmfLookVec));

	BuildConstantBuffer();
}

void Camera::BuildConstantBuffer()
{
	m_ObjectCB = make_unique<UploadBuffer<CameraCB>>(m_pDevice, 1, true);
}