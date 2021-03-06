#include "Transform.h"

Transform::Transform(XMFLOAT3 xmfScale, XMFLOAT3 xmfRotate, XMFLOAT3 xmfPosition)
{
	m_xmfScale = xmfScale;
	m_xmfRotate = xmfRotate;
	m_xmfPosition = xmfPosition;
	XMStoreFloat4x4(&m_xmmWorld, XMMatrixIdentity());
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

void Transform::KeyInput()
{
	XMFLOAT3 look, right;
	memcpy(&look, &m_xmmWorld._31, sizeof(XMFLOAT3));
	memcpy(&right, &m_xmmWorld._11, sizeof(XMFLOAT3));

	if (GetAsyncKeyState('W') & 0x8000)
		XMStoreFloat3(&m_xmfPosition, XMLoadFloat3(&m_xmfPosition) + XMLoadFloat3(&look) * 0.1f);
	if (GetAsyncKeyState('S') & 0x8000)
		XMStoreFloat3(&m_xmfPosition, XMLoadFloat3(&m_xmfPosition) - XMLoadFloat3(&look) * 0.1f);
	if (GetAsyncKeyState('A') & 0x8000)
		XMStoreFloat3(&m_xmfPosition, XMLoadFloat3(&m_xmfPosition) - XMLoadFloat3(&right) * 0.1f);
	if (GetAsyncKeyState('D') & 0x8000)
		XMStoreFloat3(&m_xmfPosition, XMLoadFloat3(&m_xmfPosition) + XMLoadFloat3(&right) * 0.1f);

}
