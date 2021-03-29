#include "Transform.h"


Transform::Transform(XMFLOAT3 xmfScale, XMFLOAT3 xmfRotate, XMFLOAT3 xmfPosition)
{
	m_xmfScale = xmfScale;
	m_xmfRotate = xmfRotate;
	m_xmfPosition = xmfPosition;
	m_xmfMeshRotate = XMFLOAT3(0.f, 0.f, 0.f);
	XMStoreFloat4x4(&m_xmmWorld, XMMatrixIdentity());

	m_bJump = false;
	m_fJumpTotal = 45.f;
	m_fJumpPower = 9.f;
	Gravity = 3.8f;

}

Transform::~Transform()
{
}

void Transform::LateUpdate(const float& fTimeDelta)
{
	XMMATRIX scale, rotateX, rotateY, rotateZ, transform;

	scale = XMMatrixScalingFromVector(XMLoadFloat3(&m_xmfScale));

	rotateX = XMMatrixRotationX(XMConvertToRadians(m_xmfRotate.x));
	rotateY = XMMatrixRotationY(XMConvertToRadians(m_xmfRotate.y));
	rotateZ = XMMatrixRotationZ(XMConvertToRadians(m_xmfRotate.z));

	transform = XMMatrixTranslationFromVector(XMLoadFloat3(&m_xmfPosition));

	XMMATRIX world = scale * rotateX * rotateY * rotateZ * transform;// *view* proj;

	XMStoreFloat4x4(&m_xmmWorld, world);
}

int Transform::KeyInput(DWORD iKey)
{
	XMFLOAT3 look, right, up;
	memcpy(&look, &m_xmmWorld._31, sizeof(XMFLOAT3));
	memcpy(&right, &m_xmmWorld._11, sizeof(XMFLOAT3));

	XMStoreFloat3(&look, XMVector3Normalize(XMLoadFloat3(&look)));
	XMStoreFloat3(&right, XMVector3Normalize(XMLoadFloat3(&right)));
	XMStoreFloat3(&up, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&look), XMLoadFloat3(&right))));

	if (m_bJump) {
		Next_tick = ((clock() - Current_tick) + 100) / 100;
		m_fJumpTotal = (m_fJumpPower * Next_tick) - (Gravity * Next_tick * Next_tick * 0.5f);
		up.y = m_fJumpTotal;
		if (InputKey & KEY_W) XMStoreFloat3(&up, XMLoadFloat3(&up) + XMLoadFloat3(&look) * 7.f);
		if (InputKey & KEY_S) XMStoreFloat3(&up, XMLoadFloat3(&up) - XMLoadFloat3(&look) * 7.f);
		if (InputKey & KEY_A) XMStoreFloat3(&up, XMLoadFloat3(&up) - XMLoadFloat3(&right) * 7.f);
		if (InputKey & KEY_D) XMStoreFloat3(&up, XMLoadFloat3(&up) + XMLoadFloat3(&right) * 7.f);
		XMStoreFloat3(&m_xmfPosition, XMLoadFloat3(&m_xmfPosition) + XMLoadFloat3(&up) * 0.005f);

		if (m_xmfPosition.y <= 0.f) { // 땅이다 라고 판별이 되면 애니메이션 처리를 해줘야하는데
			m_xmfPosition.y = 0.f;  // return 을 통해 플레이어에게 알려줘야하나 흠..
			m_bJump = false;
			return 1;
		}
	}
	else
	{
		if (iKey & KEY_W) XMStoreFloat3(&m_xmfPosition, XMLoadFloat3(&m_xmfPosition) + XMLoadFloat3(&look) * 0.07f);
		if (iKey & KEY_S) XMStoreFloat3(&m_xmfPosition, XMLoadFloat3(&m_xmfPosition) - XMLoadFloat3(&look) * 0.02f);
		if (iKey & KEY_A) XMStoreFloat3(&m_xmfPosition, XMLoadFloat3(&m_xmfPosition) - XMLoadFloat3(&right) * 0.05f);
		if (iKey & KEY_D) XMStoreFloat3(&m_xmfPosition, XMLoadFloat3(&m_xmfPosition) + XMLoadFloat3(&right) * 0.05f);
		if (iKey & KEY_SPACE) {
			InputKey = 0;
			m_bJump = true; // 점프중
			Current_tick = clock();
			if (iKey & KEY_W) InputKey |= KEY_W;
			if (iKey & KEY_S) InputKey |= KEY_S;
			if (iKey & KEY_A) InputKey |= KEY_A;
			if (iKey & KEY_D) InputKey |= KEY_D;
		}
	}


	//switch (iKey)
	//{
	//case KEY_W:
	//	XMStoreFloat3(&m_xmfPosition, XMLoadFloat3(&m_xmfPosition) + XMLoadFloat3(&look) * 0.1f);
	//	break;
	//case KEY_S:
	//	XMStoreFloat3(&m_xmfPosition, XMLoadFloat3(&m_xmfPosition) - XMLoadFloat3(&look) * 0.1f);
	//	break;
	//case KEY_A:
	//	XMStoreFloat3(&m_xmfPosition, XMLoadFloat3(&m_xmfPosition) - XMLoadFloat3(&right) * 0.1f);
	//	break;
	//case KEY_D:
	//	XMStoreFloat3(&m_xmfPosition, XMLoadFloat3(&m_xmfPosition) + XMLoadFloat3(&right) * 0.1f);
	//	break;

	//}

	//if (GetAsyncKeyState('W') & 0x8000)
	//	XMStoreFloat3(&m_xmfPosition, XMLoadFloat3(&m_xmfPosition) + XMLoadFloat3(&look) * 0.1f);
	//if (GetAsyncKeyState('S') & 0x8000)
	//	XMStoreFloat3(&m_xmfPosition, XMLoadFloat3(&m_xmfPosition) - XMLoadFloat3(&look) * 0.1f);
	//if (GetAsyncKeyState('A') & 0x8000)
	//	XMStoreFloat3(&m_xmfPosition, XMLoadFloat3(&m_xmfPosition) - XMLoadFloat3(&right) * 0.1f);
	//if (GetAsyncKeyState('D') & 0x8000)
	//	XMStoreFloat3(&m_xmfPosition, XMLoadFloat3(&m_xmfPosition) + XMLoadFloat3(&right) * 0.1f);
	return 0;
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
	XMMATRIX scale, rotateX, rotateY, rotateZ, transform, world;

	scale = XMMatrixScalingFromVector(XMLoadFloat3(&m_xmfScale));

	rotateX = XMMatrixRotationX(XMConvertToRadians(m_xmfRotate.x + m_xmfMeshRotate.x));
	rotateY = XMMatrixRotationY(XMConvertToRadians(m_xmfRotate.y + m_xmfMeshRotate.y));
	rotateZ = XMMatrixRotationZ(XMConvertToRadians(m_xmfRotate.z + m_xmfMeshRotate.z));

	transform = XMMatrixTranslationFromVector(XMLoadFloat3(&m_xmfPosition));

	world = scale * rotateX * rotateY * rotateZ * transform;

	return world;
}

void Transform::MoveForward(float speed)
{
	XMFLOAT3 look;
	memcpy(&look, &m_xmmWorld._31, sizeof(XMFLOAT3));
	XMStoreFloat3(&look, XMVector3Normalize(XMLoadFloat3(&look)));

	XMStoreFloat3(&m_xmfPosition, XMLoadFloat3(&m_xmfPosition) + XMLoadFloat3(&look) * speed);
}
