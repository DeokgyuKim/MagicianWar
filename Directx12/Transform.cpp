#include "Transform.h"

Transform::Transform(XMFLOAT3 xmfScale, XMFLOAT3 xmfRotate, XMFLOAT3 xmfPosition)
{
	m_xmfScale = xmfScale;
	m_xmfRotate = xmfRotate;
	m_xmfPosition = xmfPosition;
	m_xmfMeshRotate = XMFLOAT3(0.f, 0.f, 0.f);
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

	XMStoreFloat3(&look, XMVector3Normalize(XMLoadFloat3(&look)));
	XMStoreFloat3(&right, XMVector3Normalize(XMLoadFloat3(&right)));

	if (GetAsyncKeyState('W') & 0x8000)
		XMStoreFloat3(&m_xmfPosition, XMLoadFloat3(&m_xmfPosition) + XMLoadFloat3(&look) * 0.1f);
	if (GetAsyncKeyState('S') & 0x8000)
		XMStoreFloat3(&m_xmfPosition, XMLoadFloat3(&m_xmfPosition) - XMLoadFloat3(&look) * 0.1f);
	if (GetAsyncKeyState('A') & 0x8000)
		XMStoreFloat3(&m_xmfPosition, XMLoadFloat3(&m_xmfPosition) - XMLoadFloat3(&right) * 0.1f);
	if (GetAsyncKeyState('D') & 0x8000)
		XMStoreFloat3(&m_xmfPosition, XMLoadFloat3(&m_xmfPosition) + XMLoadFloat3(&right) * 0.1f);

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