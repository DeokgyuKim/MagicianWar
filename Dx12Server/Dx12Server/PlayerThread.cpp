#pragma comment(lib, "ws2_32")
#pragma warning(disable : 4996)
#include <iostream>
#include <unordered_map>
#include <map>
#include <vector>
#include <winsock2.h>

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#pragma comment(lib, "dxguid.lib")
#include "d3dUtil.h"

using namespace DirectX;
using namespace DirectX::PackedVector;

#include "PlayerThread.h"
#include "define.h"

//unordered_map<DWORD, bool> fds;

//unordered_map<int, SERVERPLAYER*>		g_Clients;
//vector<int> dsaf;

using namespace std;



unordered_map<DWORD, bool> ReadyCheck;
unordered_map<DWORD, SendToClientPlayerInfo*> g_MapRecvPlayerInfo;
unordered_map<DWORD, DWORD> playerKeyInput;
extern unordered_map<int, SERVERPLAYER*>	g_Clients;
unsigned int curScene = 1; // SCENE_LOGO





int recvn(SOCKET s, char* buf, int len, int flags)
{
	int received;		//recv()함수의 리턴 값 저장, 오류가 발생하지 않는다면 수신된 바이트 수를 리턴
	char* ptr = buf;	//buf의 시작 주소
	int left = len;		//수신해야할 데이터 크기, 아직 읽지 않은 데이터의 양

	while (left > 0)							//수신해야할 데이터가 남아있다면
	{
		received = recv(s, ptr, left, flags);	//recv()를 통해 데이터를 수신
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0)					//연결 정상 종료
			break;
		left -= received;						//수신해야할 데이터 크기 - 현재 받은 데이터
		ptr += received;						//buf의 시작 주소 + 현재 받은 데이터
	}

	return (len - left);						//읽어야할 총 길이 - 수신해야할 데이터 양 = 읽은 데이터 양
}


DWORD WINAPI PlayerThread(LPVOID arg)
{
	SERVERPLAYER* player = (SERVERPLAYER*)arg;
	SOCKET client_sock = player->socket;

	ReadyCheck[player->info.dwPlayerNum] = false;
	PlayerInfo myInfo = player->info; // 나 자신

	g_MapRecvPlayerInfo[player->info.dwPlayerNum] = new SendToClientPlayerInfo; // player들이 들어올때마다 new
	g_MapRecvPlayerInfo[player->info.dwPlayerNum]->playerInfo = myInfo;

	while (true) // loop
	{

		while (true) 
		{
			DWORD buf;
			// 레디상태를 계속 받아줘야지
			recvn(client_sock, (char*)&buf, sizeof(buf), 0);
			if (buf == 1) { std::cout << "Ready 완료" << endl; }
			//else cout << "레디 안함" << endl;
			std::cout << buf << endl;

			if (buf == 1)
				ReadyCheck[player->info.dwPlayerNum] = true; // 해당 플레이어는 레디
			else
				ReadyCheck[player->info.dwPlayerNum] = false; // 해당 플레이어는 레디 안함

			bool Ready = true;
			int PlayerCount = 0;
			for (auto it = ReadyCheck.begin(); it != ReadyCheck.end(); ++it) { // 전체 순회
				if (it->second == false) { // 들어온 누구라도 false라면
					Ready = false; // 스타트 안할거고
				}
				++PlayerCount;
			}
			

			send(client_sock, (char*)&Ready, sizeof(Ready), 0); // 레디 체크한 것을 보내주고

			if (Ready) { // 순회해서 다 레디라면
				--PlayerCount; // 나 자신은 뺴
				myInfo.xmfPosition = XMFLOAT3(20.f, 0.f, 10.f);
				send(client_sock, (char*)&myInfo, sizeof(myInfo), 0); // 나 자신의 정보
				send(client_sock, (char*)&PlayerCount, sizeof(PlayerCount), 0); // 인원수 보내고
				for (auto it = g_Clients.begin(); it != g_Clients.end(); ++it) {

					if (it->second->info.dwPlayerNum != player->info.dwPlayerNum) { // 나 자신 제외라면
						PlayerInfo _pinfo;
						_pinfo = it->second->info;
						send(client_sock, (char*)&_pinfo, sizeof(_pinfo), 0); // 나 자신 제외한 애들의 playerinfo 넘겨줘
					}
				}
				curScene = 2; // 다음 씬
				break;
			}
		}

			while(true){

			SendToServerPlayerInfo _pInfo;
			DWORD _keyInput;
			recvn(client_sock, (char*)&_pInfo, sizeof(_pInfo), 0);
			recvn(client_sock, (char*)&_keyInput, sizeof(_keyInput), 0);

			UpdatePlayerInfo(_pInfo, player->info.dwPlayerNum); // player 위치 받아주기
			playerKeyInput[player->info.dwPlayerNum] = _keyInput; // player 키입력 받아주기
			
			UpdatePosition(player->info.dwPlayerNum);


			for (auto it = g_MapRecvPlayerInfo.begin(); it != g_MapRecvPlayerInfo.end(); ++it) {
				send(client_sock, (char*)it->second, sizeof(*it->second), 0); // 모든 정보
			}
			//cout << "키 - " << playerKeyInput[player->info.dwPlayerNum]<<"\n";
			//cout << "x - " << g_MapRecvPlayerInfo[player->info.dwPlayerNum]->matWorld._41 << ", " <<
			//	"y - " << g_MapRecvPlayerInfo[player->info.dwPlayerNum]->matWorld._42 << ", " <<
			//	"z - " << g_MapRecvPlayerInfo[player->info.dwPlayerNum]->matWorld._43 << "\n";


		//	break;
		//}

		}

	}

}

//DWORD WINAPI WorkThread(LPVOID arg)
//{
//	for (auto it = g_MapRecvPlayerInfo.begin(); it != g_MapRecvPlayerInfo.end(); ++it) {
//
//	}
//}

void UpdatePlayerInfo(SendToServerPlayerInfo _pInfo, DWORD _playerNumber)
{
	//std::cout << "( " << _pInfo.matWorld._41 << ", " << _pInfo.matWorld._42 << ", " << _pInfo.matWorld._43 << " )" << endl;
	g_MapRecvPlayerInfo[_playerNumber]->eAnimBlendType = _pInfo.eAnimBlendType;
	g_MapRecvPlayerInfo[_playerNumber]->ePlayerState = _pInfo.ePlayerState;
	g_MapRecvPlayerInfo[_playerNumber]->fAnimTime = _pInfo.fAnimTime;
	g_MapRecvPlayerInfo[_playerNumber]->eAnimType = _pInfo.eAnimType;
	g_MapRecvPlayerInfo[_playerNumber]->fWeight = _pInfo.fWeight;
	g_MapRecvPlayerInfo[_playerNumber]->matWorld = _pInfo.matWorld;
}

void UpdatePosition(DWORD _playerNumber)
{
	if (playerKeyInput[_playerNumber] & 0x0001) {
		if (playerKeyInput[_playerNumber] & 0x0002) {
			MoveForward(_playerNumber, (M_MoveForward_Speed / sqrtf(2.f)));
			MoveLeft(_playerNumber, (M_MoveLeft_Speed / sqrtf(2.f)));
		}
		else if (playerKeyInput[_playerNumber] & 0x0008) {
			MoveForward(_playerNumber, (M_MoveForward_Speed / sqrtf(2.f)));
			MoveRight(_playerNumber, (M_MoveRight_Speed / sqrtf(2.f)));
		}
		else {
			MoveForward(_playerNumber, M_MoveForward_Speed);
		}
	} 
	else if (playerKeyInput[_playerNumber] & 0x0004) {
		if (playerKeyInput[_playerNumber] & 0x0002) {
			MoveBackward(_playerNumber, (M_MoveBackward_Speed / sqrtf(2.f)));
			MoveLeft(_playerNumber, (M_MoveLeft_Speed / sqrtf(2.f)));
		}
		else if (playerKeyInput[_playerNumber] & 0x0008) {
			MoveBackward(_playerNumber, (M_MoveBackward_Speed / sqrtf(2.f)));
			MoveRight(_playerNumber, (M_MoveRight_Speed / sqrtf(2.f)));
		}
		else {
			MoveBackward(_playerNumber, M_MoveBackward_Speed);
		}
	}
	else if (playerKeyInput[_playerNumber] & 0x0002) {
		MoveLeft(_playerNumber, M_MoveLeft_Speed);
	}
	else if (playerKeyInput[_playerNumber] & 0x0008) {
		MoveRight(_playerNumber, M_MoveRight_Speed);
	}
}

void MoveForward(DWORD _playerNumber, float speed)
{
	XMFLOAT3 look;
	XMFLOAT3 pos = { g_MapRecvPlayerInfo[_playerNumber]->matWorld._41 ,g_MapRecvPlayerInfo[_playerNumber]->matWorld._42 ,g_MapRecvPlayerInfo[_playerNumber]->matWorld._43 };
	memcpy(&look, &g_MapRecvPlayerInfo[_playerNumber]->matWorld._21, sizeof(XMFLOAT3));
	XMStoreFloat3(&look, XMVector3Normalize(XMLoadFloat3(&look)));
	//XMStoreFloat3(&look, XMVector3TransformNormal(XMLoadFloat3(&look), XMMatrixRotationX(XMConvertToRadians(90.f))));
	
	XMStoreFloat3(&pos, XMLoadFloat3(&pos) - XMLoadFloat3(&look) * speed);
	g_MapRecvPlayerInfo[_playerNumber]->matWorld._41 = pos.x;
	g_MapRecvPlayerInfo[_playerNumber]->matWorld._42 = pos.y;
	g_MapRecvPlayerInfo[_playerNumber]->matWorld._43 = pos.z;
}

void MoveBackward(DWORD _playerNumber, float speed)
{
	XMFLOAT3 look;
	XMFLOAT3 pos = { g_MapRecvPlayerInfo[_playerNumber]->matWorld._41 ,g_MapRecvPlayerInfo[_playerNumber]->matWorld._42 ,g_MapRecvPlayerInfo[_playerNumber]->matWorld._43 };
	memcpy(&look, &g_MapRecvPlayerInfo[_playerNumber]->matWorld._21, sizeof(XMFLOAT3));
	XMStoreFloat3(&look, XMVector3Normalize(XMLoadFloat3(&look)));
	//XMStoreFloat3(&look, XMVector3TransformNormal(XMLoadFloat3(&look), XMMatrixRotationX(XMConvertToRadians(90.f))));

	XMStoreFloat3(&pos, XMLoadFloat3(&pos) + XMLoadFloat3(&look) * speed);
	
	g_MapRecvPlayerInfo[_playerNumber]->matWorld._41 = pos.x;
	g_MapRecvPlayerInfo[_playerNumber]->matWorld._42 = pos.y;
	g_MapRecvPlayerInfo[_playerNumber]->matWorld._43 = pos.z;
}

void MoveLeft(DWORD _playerNumber, float speed)
{
	XMFLOAT3 left;
	XMFLOAT3 pos = { g_MapRecvPlayerInfo[_playerNumber]->matWorld._41 ,g_MapRecvPlayerInfo[_playerNumber]->matWorld._42 ,g_MapRecvPlayerInfo[_playerNumber]->matWorld._43 };
	memcpy(&left, &g_MapRecvPlayerInfo[_playerNumber]->matWorld._11, sizeof(XMFLOAT3));
	XMStoreFloat3(&left, XMVector3Normalize(XMLoadFloat3(&left)));

	XMStoreFloat3(&pos, XMLoadFloat3(&pos) - XMLoadFloat3(&left) * speed);

	g_MapRecvPlayerInfo[_playerNumber]->matWorld._41 = pos.x;
	g_MapRecvPlayerInfo[_playerNumber]->matWorld._42 = pos.y;
	g_MapRecvPlayerInfo[_playerNumber]->matWorld._43 = pos.z;
}

void MoveRight(DWORD _playerNumber, float speed)
{
	XMFLOAT3 right;
	XMFLOAT3 pos = { g_MapRecvPlayerInfo[_playerNumber]->matWorld._41 ,g_MapRecvPlayerInfo[_playerNumber]->matWorld._42 ,g_MapRecvPlayerInfo[_playerNumber]->matWorld._43 };
	memcpy(&right, &g_MapRecvPlayerInfo[_playerNumber]->matWorld._11, sizeof(XMFLOAT3));
	XMStoreFloat3(&right, XMVector3Normalize(XMLoadFloat3(&right)));

	XMStoreFloat3(&pos, XMLoadFloat3(&pos) + XMLoadFloat3(&right) * speed);

	g_MapRecvPlayerInfo[_playerNumber]->matWorld._41 = pos.x;
	g_MapRecvPlayerInfo[_playerNumber]->matWorld._42 = pos.y;
	g_MapRecvPlayerInfo[_playerNumber]->matWorld._43 = pos.z;
}
