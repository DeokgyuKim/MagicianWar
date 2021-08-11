#include "Camera.h"
#include "Renderer.h"
#include "Player.h"
#include "Network.h"

#include "PlayerFSM.h"

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
	//if (m_eCamMode == CAMERA_MODE::CAMERA_THIRD)
	//	ShowCursor(FALSE);
	//else
	//	ShowCursor(TRUE);


	m_fTime += fTimeDelta;
	if (GetAsyncKeyState(VK_F1) & 0x8000)
		m_eCamMode = CAMERA_MODE::CAMERA_NONE;
	if (GetAsyncKeyState(VK_F2) & 0x8000)
		m_eCamMode = CAMERA_MODE::CAMERA_FREE;
	if (GetAsyncKeyState(VK_F3) & 0x8000)
		m_eCamMode = CAMERA_MODE::CAMERA_THIRD;

	if ((m_eCamMode == CAMERA_MODE::CAMERA_FREE || m_eCamMode == CAMERA_MODE::CAMERA_THIRD))
	{
#ifdef NETWORK
		if (Network::GetInstance()->GetRoundStart())
		{
			CameraRotation();
		}
#else
		CameraRotation();
#endif

		if (m_eCamMode == CAMERA_MODE::CAMERA_THIRD)
		{
			
#ifdef PHYSX
			XMFLOAT3 xmfPlayerPos = m_pPlayer->GetPosition();
			xmfPlayerPos.y += 1.f;
			XMFLOAT3 xmfOff = XMFLOAT3(0.f, 1.f, 0.f);
			XMStoreFloat3(&m_xmfPosition, XMLoadFloat3(&xmfPlayerPos) - XMLoadFloat3(&m_xmfLookVec) * m_fOffset + XMLoadFloat3(&xmfOff));

			float fNearOff = 0.f;
			float fDist = 0.f;
			bool bdynamicObj = false;
			bool bstaticObj = false;

			while (true)
			{
				XMStoreFloat3(&m_xmfPosition, XMLoadFloat3(&xmfPlayerPos) - XMLoadFloat3(&m_xmfLookVec) * (m_fOffset - fNearOff) + XMLoadFloat3(&xmfOff));
				XMVECTOR xmfLook = XMVector3Normalize(XMLoadFloat3(&xmfPlayerPos) - XMLoadFloat3(&m_xmfPosition));
				XMVECTOR xmvLength = XMVector3Length(XMLoadFloat3(&xmfPlayerPos) - XMLoadFloat3(&m_xmfPosition));
				XMFLOAT3 xmfLength;
				XMStoreFloat3(&xmfLength, xmvLength);
				float distance = xmfLength.x;
				bool bColl = CPhysXMgr::GetInstance()->ShotRay(XMLoadFloat3(&m_xmfPosition), xmfLook, distance, bdynamicObj, bstaticObj);
				if (!(bColl && bstaticObj))
				{
					XMStoreFloat3(&m_xmfPosition, XMLoadFloat3(&xmfPlayerPos) - XMLoadFloat3(&m_xmfLookVec) * (m_fOffset - fNearOff - 0.2f) + XMLoadFloat3(&xmfOff));
					break;
				}
				fNearOff += 0.05f;
				if (fNearOff > 2.8)
				{
					XMStoreFloat3(&m_xmfPosition, XMLoadFloat3(&xmfPlayerPos) - XMLoadFloat3(&m_xmfLookVec) * (0.f) + XMLoadFloat3(&xmfOff));
					break;
				}
			}

			//XMMATRIX matTestWorld = XMMatrixTranslationFromVector(XMLoadFloat3(&m_xmfPosition));
			//XMFLOAT4X4 xmf4x4Testworld;
			//XMStoreFloat4x4(&xmf4x4Testworld, matTestWorld);
			//if (m_pRigidBody != nullptr)
			//{
			//	float fNearOff = 0.f;
			//	m_pRigidBody->setGlobalPose(CPhysXMgr::GetInstance()->MakePxTransform(xmf4x4Testworld));
			//	while (CPhysXMgr::GetInstance()->CollisionForStaticObjects(m_pRigidBody))
			//	{
			//		fNearOff += 0.03f;
			//		XMStoreFloat3(&m_xmfPosition, XMLoadFloat3(&xmfPlayerPos) - XMLoadFloat3(&m_xmfLookVec) * (m_fOffset - fNearOff) + XMLoadFloat3(&xmfOff));
			//		matTestWorld = XMMatrixTranslationFromVector(XMLoadFloat3(&m_xmfPosition));
			//		XMStoreFloat4x4(&xmf4x4Testworld, matTestWorld);
			//		m_pRigidBody->setGlobalPose(CPhysXMgr::GetInstance()->MakePxTransform(xmf4x4Testworld));
			//	}
			//}
#else
			XMFLOAT3 xmfPlayerPos = m_pPlayer->GetPosition();
			xmfPlayerPos.y += 1.f;
			XMFLOAT3 xmfOff = XMFLOAT3(0.f, 1.f, 0.f);
			XMStoreFloat3(&m_xmfPosition, XMLoadFloat3(&xmfPlayerPos) - XMLoadFloat3(&m_xmfLookVec) * m_fOffset + XMLoadFloat3(&xmfOff));
#endif
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
	XMMATRIX proj = XMMatrixPerspectiveFovLH(XMConvertToRadians(90.f), (float)WINCX / (float)WINCY, 0.1f, 1000.0f);

	XMStoreFloat4x4(&m_xmmProj, proj);

	XMStoreFloat3(&m_xmfTarget, XMLoadFloat3(&m_xmfPosition) + XMLoadFloat3(&m_xmfLookVec));

	XMVECTOR pos = XMVectorSet(m_xmfPosition.x, m_xmfPosition.y, m_xmfPosition.z, 1.0f);
	XMVECTOR target = XMVectorSet(m_xmfTarget.x, m_xmfTarget.y, m_xmfTarget.z, 1.0f); 
	//XMVECTOR target = XMVectorSet(m_xmfLookVec.x, m_xmfLookVec.y, m_xmfLookVec.z, 1.0f);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // XMVectorSet(m_xmfUpVec.x, m_xmfUpVec.y, m_xmfUpVec.z, 0.0f);

	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&m_xmmView, view);

	CameraCB camCB;
	XMStoreFloat4x4(&camCB.View, (XMLoadFloat4x4(&m_xmmView)));
	XMStoreFloat4x4(&camCB.Proj, (XMLoadFloat4x4(&m_xmmProj)));
	XMStoreFloat4x4(&camCB.InvView, XMMatrixInverse(NULL, (XMLoadFloat4x4(&m_xmmView)))); 
	XMStoreFloat4x4(&camCB.InvProj, XMMatrixInverse(NULL, (XMLoadFloat4x4(&m_xmmProj))));
	XMFLOAT4 position = XMFLOAT4(m_xmfPosition.x, m_xmfPosition.y, m_xmfPosition.z, 1.f);
	//cout << position.x << "\t" << position.y << "\t" << position.z << endl;
	XMStoreFloat4(&camCB.Position, XMLoadFloat4(&position));
	camCB.Time = m_fTime;


	m_ObjectCB->CopyData(0, camCB);
}

void Camera::Render(const float& fTimeDelta, int _instanceCount)
{ // 카메라 설정
	m_pCmdLst->SetGraphicsRootConstantBufferView(1, m_ObjectCB->Resource()->GetGPUVirtualAddress());
}

void Camera::SetMode(CAMERA_MODE eMode)
{ 
	m_eCamMode = eMode;
	//if (m_eCamMode == CAMERA_MODE::CAMERA_THIRD)
	//	ShowCursor(FALSE);
	//else
	//	ShowCursor(TRUE);
}



void Camera::CameraRotation()
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
	m_bInstanced = false;

	BuildConstantBuffer();

#ifdef PHYSX
	m_pRigidBody = CPhysXMgr::GetInstance()->CreateSphere(this, m_xmfPosition, 0.2f);
#endif
}

void Camera::BuildConstantBuffer()
{
	m_ObjectCB = make_unique<UploadBuffer<CameraCB>>(m_pDevice, 1, true);
}