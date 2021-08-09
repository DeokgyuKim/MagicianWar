#include "Transform.h"


Transform::Transform(XMFLOAT3 xmfScale, XMFLOAT3 xmfRotate, XMFLOAT3 xmfPosition)
{
	m_xmfScale = xmfScale;
	m_xmfRotate = xmfRotate;
	m_xmfPosition = xmfPosition;
	m_xmfMeshRotate = XMFLOAT3(0.f, 0.f, 0.f);
	XMStoreFloat4x4(&m_xmmWorld, XMMatrixIdentity());
	XMStoreFloat4x4(&m_xmmOriginWorld, XMMatrixIdentity());

	m_bJump = false;
	m_fJumpTotal = 45.f;
	m_fJumpPower = 9.f;
	

	m_jumpForwardSpeed = M_MoveForward_Speed * 10000.f;
	m_jumpBackwardSpeed = M_MoveBackward_Speed * 10000.f;
	m_jumpLeftSpeed = M_MoveLeft_Speed * 10000.f;
	m_jumpRightSpeed = M_MoveRight_Speed * 10000.f;
	m_fJumpSpeed = 0.01f;
}

Transform::~Transform()
{
}

void Transform::LateUpdate(const float& fTimeDelta)
{
	XMMATRIX scale, rotateX, rotateY, rotateZ, transform;
	
	scale = XMMatrixScalingFromVector(XMLoadFloat3(&m_xmfScale));
	
	rotateX = XMMatrixRotationX(XMConvertToRadians(m_xmfRotate.x + m_xmfMeshRotate.x));
	rotateY = XMMatrixRotationY(XMConvertToRadians(m_xmfRotate.y + m_xmfMeshRotate.y));
	rotateZ = XMMatrixRotationZ(XMConvertToRadians(m_xmfRotate.z + m_xmfMeshRotate.z));
	
	
	
	
	transform = XMMatrixTranslationFromVector(XMLoadFloat3(&m_xmfPosition));
	
	XMMATRIX world = scale * rotateX * rotateY * rotateZ * transform;
	if (m_pxmmParentWorld != NULL)
		world *= XMLoadFloat4x4(m_pxmmParentWorld);
	
	XMStoreFloat4x4(&m_xmmWorld, world);
	
	rotateX = XMMatrixRotationX(XMConvertToRadians(m_xmfRotate.x));
	rotateY = XMMatrixRotationY(XMConvertToRadians(m_xmfRotate.y));
	rotateZ = XMMatrixRotationZ(XMConvertToRadians(m_xmfRotate.z));
	
	world = scale * rotateX * rotateY * rotateZ * transform;
	XMStoreFloat4x4(&m_xmmOriginWorld, world);

}



XMFLOAT3 Transform::GetLook()
{
	XMFLOAT3 lookVector = { m_xmmWorld._31,m_xmmWorld._32, m_xmmWorld._33 };
	XMFLOAT3 normalizeVec;
	XMStoreFloat3(&normalizeVec, XMVector3Normalize(XMLoadFloat3(&lookVector)));
	return normalizeVec;
}

XMMATRIX Transform::GetWorldMatrix()
{
	return XMLoadFloat4x4(&m_xmmWorld);
}

void Transform::MoveForward(float speed)
{
#ifdef NETWORK
	return;
#endif
	XMFLOAT3 look;
	memcpy(&look, &m_xmmOriginWorld._31, sizeof(XMFLOAT3));
	XMStoreFloat3(&look, XMVector3Normalize(XMLoadFloat3(&look)));

	XMStoreFloat3(&m_xmfPosition, XMLoadFloat3(&m_xmfPosition) + XMLoadFloat3(&look) * speed);
}

void Transform::MoveBackward(float speed)
{
#ifdef NETWORK
	return;
#endif
	XMFLOAT3 look;
	memcpy(&look, &m_xmmOriginWorld._31, sizeof(XMFLOAT3));
	XMStoreFloat3(&look, XMVector3Normalize(XMLoadFloat3(&look)));

	XMStoreFloat3(&m_xmfPosition, XMLoadFloat3(&m_xmfPosition) - XMLoadFloat3(&look) * speed);
}

void Transform::MoveLeft(float speed)
{
#ifdef NETWORK
	return;
#endif
	XMFLOAT3 right;
	memcpy(&right, &m_xmmWorld._11, sizeof(XMFLOAT3));
	XMStoreFloat3(&right, XMVector3Normalize(XMLoadFloat3(&right)));

	XMStoreFloat3(&m_xmfPosition, XMLoadFloat3(&m_xmfPosition) - XMLoadFloat3(&right) * speed);
}

void Transform::MoveRight(float speed)
{
#ifdef NETWORK
	return;
#endif
	XMFLOAT3 right;
	memcpy(&right, &m_xmmWorld._11, sizeof(XMFLOAT3));
	XMStoreFloat3(&right, XMVector3Normalize(XMLoadFloat3(&right)));

	XMStoreFloat3(&m_xmfPosition, XMLoadFloat3(&m_xmfPosition) + XMLoadFloat3(&right) * speed);
}

XMFLOAT3 Transform::GetForwardVector()
{
	XMFLOAT3 look;
	memcpy(&look, &m_xmmOriginWorld._31, sizeof(XMFLOAT3));
	XMStoreFloat3(&look, XMVector3Normalize(XMLoadFloat3(&look)));
	return look;
}

XMFLOAT3 Transform::GetBackwardVector()
{
	XMFLOAT3 look;
	memcpy(&look, &m_xmmOriginWorld._31, sizeof(XMFLOAT3));
	XMStoreFloat3(&look, XMVector3Normalize(XMLoadFloat3(&look)) * -1.f);
	return look;
}

XMFLOAT3 Transform::GetLeftVector()
{
	XMFLOAT3 right;
	memcpy(&right, &m_xmmWorld._11, sizeof(XMFLOAT3));
	XMStoreFloat3(&right, XMVector3Normalize(XMLoadFloat3(&right)) * -1.f);
	return right;
}

XMFLOAT3 Transform::GetRightVector()
{
	XMFLOAT3 right;
	memcpy(&right, &m_xmmWorld._11, sizeof(XMFLOAT3));
	XMStoreFloat3(&right, XMVector3Normalize(XMLoadFloat3(&right)));
	return right;
}

int Transform::Jump(DWORD iKey)
{
#ifdef NETWORK
	return 0;
#endif
	XMFLOAT3 look, right, up;
	memcpy(&look, &m_xmmOriginWorld._31, sizeof(XMFLOAT3));
	memcpy(&right, &m_xmmWorld._11, sizeof(XMFLOAT3));
	up = XMFLOAT3(0.f, 1.f, 0.f);

	

	if (m_bJump) {
		Next_tick = ((clock() - Current_tick) + 100) / 100;
		m_fJumpTotal = (m_fJumpPower * Next_tick) - (GRAVITY * Next_tick * Next_tick * 0.2f);
		up.y = m_fJumpTotal;
		if (InputKey & KEY_W) {
			if (InputKey & KEY_A) {
				XMStoreFloat3(&up, XMLoadFloat3(&up) + XMLoadFloat3(&look) * m_jumpForwardSpeed / sqrtf(2.f));
				XMStoreFloat3(&up, XMLoadFloat3(&up) - XMLoadFloat3(&right) * m_jumpLeftSpeed / sqrtf(2.f));
			}
			else if (InputKey & KEY_D) {
				XMStoreFloat3(&up, XMLoadFloat3(&up) + XMLoadFloat3(&look) * m_jumpForwardSpeed / sqrtf(2.f));
				XMStoreFloat3(&up, XMLoadFloat3(&up) + XMLoadFloat3(&right) * m_jumpRightSpeed / sqrtf(2.f));
			}
			else
				XMStoreFloat3(&up, XMLoadFloat3(&up) + XMLoadFloat3(&look) * m_jumpForwardSpeed);
		}
		else if (InputKey & KEY_S)
		{
			if (InputKey & KEY_A) {
				XMStoreFloat3(&up, XMLoadFloat3(&up) - XMLoadFloat3(&look) * m_jumpBackwardSpeed / sqrtf(2.f));
				XMStoreFloat3(&up, XMLoadFloat3(&up) - XMLoadFloat3(&right) * m_jumpLeftSpeed / sqrtf(2.f));
			}
			else if (InputKey & KEY_D) {
				XMStoreFloat3(&up, XMLoadFloat3(&up) - XMLoadFloat3(&look) * m_jumpBackwardSpeed / sqrtf(2.f));
				XMStoreFloat3(&up, XMLoadFloat3(&up) + XMLoadFloat3(&right) * m_jumpRightSpeed / sqrtf(2.f));
			}
			else
				XMStoreFloat3(&up, XMLoadFloat3(&up) - XMLoadFloat3(&look) * m_jumpBackwardSpeed);
		}
		else if (InputKey & KEY_A) XMStoreFloat3(&up, XMLoadFloat3(&up) - XMLoadFloat3(&right) * m_jumpLeftSpeed);
		else if (InputKey & KEY_D) XMStoreFloat3(&up, XMLoadFloat3(&up) + XMLoadFloat3(&right) * m_jumpRightSpeed);


		XMStoreFloat3(&m_xmfPosition, XMLoadFloat3(&m_xmfPosition) + XMLoadFloat3(&up) * m_fJumpSpeed);

		if (m_xmfPosition.y <= 0.f) { // 땅이다 라고 판별이 되면 애니메이션 처리를 해줘야하는데
			m_xmfPosition.y = 0.f;  // return 을 통해 플레이어에게 알려줘야하나 흠..
			m_bJump = false;
			return 1;
		}
	}
	else
	{
		InputKey = 0;
		m_bJump = true; // 점프중
		Current_tick = clock();
		if (iKey & KEY_W) InputKey |= KEY_W;
		if (iKey & KEY_S) InputKey |= KEY_S;
		if (iKey & KEY_A) InputKey |= KEY_A;
		if (iKey & KEY_D) InputKey |= KEY_D;
	}
	return 0;

}
