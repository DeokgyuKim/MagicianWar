#include "Player.h"

Player::Player()
{
	ZeroMemory(&Info, sizeof(Info));
	XMStoreFloat4x4(&matWorld, XMMatrixIdentity());
	ePlayerState = PLAYER_STATE::NONE;
	eAnimType = ANIMATION_TYPE::NONE;
	fAnimTime = 0.f;
	eAnimBlendType = ANIMATION_TYPE::NONE;
	fWeight = 0.f;
	m_isConnected = false;

	m_keyInput = 0;
}

Player::~Player()
{
	// ���� �ݰ� �������� ���� ����
	closesocket(Info.socket);
	m_isConnected = false;
}

void Player::Initialize(SOCKET& sock, int ID) // sock �� id�� �޾Ƽ� �ʱ�ȭ
{
	Info.socket = sock;
	Info.info.dwPlayerNum = ID;
	Info.info.dwTeamNum = ID;

	m_Ready = false; // �ʱ� unReady
	m_isConnected = true; // �������� ����


	recv_start_ptr = recvBuf;
	packet_start_ptr = recvBuf;
}

void Player::Update()
{
	// ���⼭ �̵��� ���� ������Ʈ �ϸ� �ǰ���..
	UpdatePosition();
}

void Player::UpdatePlayerInfo(CTOS_PlayerInfo* pInfo)
{
	m_mutex.lock();
	//	
	eAnimBlendType = pInfo->eAnimBlendType;
	ePlayerState = pInfo->ePlayerState;
	eAnimType = pInfo->eAnimType;
	fAnimTime = pInfo->fAnimTime;
	fWeight = pInfo->fWeight;

	noTransWorldUpdate(pInfo->matWorld);
	//
	m_mutex.unlock();
}

void Player::UpdatePosition()
{
	float globalSpeed = 3.f;
	if (m_keyInput & 0x0001) {
		if (m_keyInput & 0x0002) {
			MoveForward((M_MoveForward_Speed / sqrtf(2.f)) * globalSpeed);
			MoveLeft((M_MoveLeft_Speed / sqrtf(2.f)) * globalSpeed);
		}
		else if (m_keyInput & 0x0008) {
			MoveForward((M_MoveForward_Speed / sqrtf(2.f)) * globalSpeed);
			MoveRight((M_MoveRight_Speed / sqrtf(2.f)) * globalSpeed);
		}
		else {
			MoveForward(M_MoveForward_Speed * globalSpeed);
		}
	}
	else if (m_keyInput & 0x0004) {
		if (m_keyInput & 0x0002) {
			MoveBackward((M_MoveBackward_Speed / sqrtf(2.f)) * globalSpeed);
			MoveLeft((M_MoveLeft_Speed / sqrtf(2.f)) * globalSpeed);
		}
		else if (m_keyInput & 0x0008) {
			MoveBackward((M_MoveBackward_Speed / sqrtf(2.f)) * globalSpeed);
			MoveRight((M_MoveRight_Speed / sqrtf(2.f)) * globalSpeed);
		}
		else {
			MoveBackward(M_MoveBackward_Speed * globalSpeed);
		}
	}
	else if (m_keyInput & 0x0002) {
		MoveLeft(M_MoveLeft_Speed * globalSpeed);
	}
	else if (m_keyInput & 0x0008) {
		MoveRight(M_MoveRight_Speed * globalSpeed);
	}
}

void Player::noTransWorldUpdate(XMFLOAT4X4 _world)
{
	matWorld._11 = _world._11;
	matWorld._12 = _world._12;
	matWorld._13 = _world._13;
	matWorld._14 = _world._14;

	matWorld._21 = _world._21;
	matWorld._22 = _world._22;
	matWorld._23 = _world._23;
	matWorld._24 = _world._24;

	matWorld._31 = _world._31;
	matWorld._32 = _world._32;
	matWorld._33 = _world._33;
	matWorld._34 = _world._34;
}

void Player::MoveForward(float speed)
{
	XMFLOAT3 look;
	XMFLOAT3 pos = { matWorld._41 ,matWorld._42 ,matWorld._43 };
	memcpy(&look, &matWorld._21, sizeof(XMFLOAT3));
	XMStoreFloat3(&look, XMVector3Normalize(XMLoadFloat3(&look)));
	//XMStoreFloat3(&look, XMVector3TransformNormal(XMLoadFloat3(&look), XMMatrixRotationX(XMConvertToRadians(90.f))));

	XMStoreFloat3(&pos, XMLoadFloat3(&pos) - XMLoadFloat3(&look) * speed);
	matWorld._41 = pos.x;
	matWorld._42 = pos.y;
	matWorld._43 = pos.z;
}

void Player::MoveBackward(float speed)
{
	XMFLOAT3 look;
	XMFLOAT3 pos = { matWorld._41 ,matWorld._42 ,matWorld._43 };
	memcpy(&look, &matWorld._21, sizeof(XMFLOAT3));
	XMStoreFloat3(&look, XMVector3Normalize(XMLoadFloat3(&look)));
	//XMStoreFloat3(&look, XMVector3TransformNormal(XMLoadFloat3(&look), XMMatrixRotationX(XMConvertToRadians(90.f))));

	XMStoreFloat3(&pos, XMLoadFloat3(&pos) + XMLoadFloat3(&look) * speed);
	matWorld._41 = pos.x;
	matWorld._42 = pos.y;
	matWorld._43 = pos.z;
}

void Player::MoveLeft(float speed)
{
	XMFLOAT3 left;
	XMFLOAT3 pos = { matWorld._41 ,matWorld._42 ,matWorld._43 };
	memcpy(&left, &matWorld._11, sizeof(XMFLOAT3));
	XMStoreFloat3(&left, XMVector3Normalize(XMLoadFloat3(&left)));
	//XMStoreFloat3(&look, XMVector3TransformNormal(XMLoadFloat3(&look), XMMatrixRotationX(XMConvertToRadians(90.f))));

	XMStoreFloat3(&pos, XMLoadFloat3(&pos) - XMLoadFloat3(&left) * speed);
	matWorld._41 = pos.x;
	matWorld._42 = pos.y;
	matWorld._43 = pos.z;
}

void Player::MoveRight(float speed)
{
	XMFLOAT3 right;
	XMFLOAT3 pos = { matWorld._41 ,matWorld._42 ,matWorld._43 };
	memcpy(&right, &matWorld._11, sizeof(XMFLOAT3));
	XMStoreFloat3(&right, XMVector3Normalize(XMLoadFloat3(&right)));
	//XMStoreFloat3(&look, XMVector3TransformNormal(XMLoadFloat3(&look), XMMatrixRotationX(XMConvertToRadians(90.f))));

	XMStoreFloat3(&pos, XMLoadFloat3(&pos) + XMLoadFloat3(&right) * speed);
	matWorld._41 = pos.x;
	matWorld._42 = pos.y;
	matWorld._43 = pos.z;
}

int Player::recvPacket()
{
	return recv(Info.socket, recv_start_ptr, MAX_BUFFER, 0);
}

int Player::sendPacket(void* _packet, int _size)
{
	return send(Info.socket, (char*)(_packet), _size, 0);
}

void Player::setKeyInput(DWORD _key)
{
	m_mutex.lock();
	m_keyInput = _key;
	m_mutex.unlock();
}

void Player::setReady(DWORD _ready)
{
	m_mutex.lock();
	m_Ready = _ready;
	m_mutex.unlock();
}
