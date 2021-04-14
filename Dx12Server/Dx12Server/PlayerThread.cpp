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
#include "struct.h"

//unordered_map<DWORD, bool> fds;

//unordered_map<int, SERVERPLAYER*>		g_Clients;
//vector<int> dsaf;

using namespace std;



unordered_map<DWORD, bool> ReadyCheck;
extern unordered_map<int, SERVERPLAYER*>		g_Clients;
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

	while (true) // loop
	{
		switch (curScene)
		{
		case 1:
		{
			DWORD buf;
			// 레디상태를 계속 받아줘야지
			recvn(client_sock, (char*)&buf, sizeof(buf), 0);
			if (buf == 1) { cout << "Ready 완료" << endl; }
			//else cout << "레디 안함" << endl;
			cout << buf << endl;

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
			cout << Ready << endl;

			send(client_sock, (char*)&Ready, sizeof(Ready), 0); // 레디 체크한 것을 보내주고

			if (Ready) { // 순회해서 다 레디라면
				--PlayerCount; // 나 자신은 뺴
				send(client_sock, (char*)&PlayerCount, sizeof(PlayerCount), 0); // 인원수 보내고
				for (auto it = g_Clients.begin(); it != g_Clients.end(); ++it) {

					if (it->second->info.dwPlayerNum != player->info.dwPlayerNum) { // 나 자신 제외라면
						PlayerInfo _pinfo;
						_pinfo = it->second->info;
						send(client_sock, (char*)&_pinfo, sizeof(_pinfo), 0); // 나 자신 제외한 애들의 playerinfo 넘겨줘
					}
				}
				curScene = 2; // 다음 씬
			}
			break;
		}
		case 2:

		}

	}

}
