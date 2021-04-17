#pragma once

#include "struct.h"

int recvn(SOCKET s, char* buf, int len, int flags);
DWORD WINAPI PlayerThread(LPVOID arg);
//DWORD WINAPI WorkThread(LPVOID arg);
void UpdatePlayerInfo(SendToServerPlayerInfo _pInfo, DWORD _playerNumber);

void UpdatePosition(DWORD _playerNumber);
void MoveForward(DWORD _playerNumber, float speed);
void MoveBackward(DWORD _playerNumber, float speed);
void MoveLeft(DWORD _playerNumber, float speed);
void MoveRight(DWORD _playerNumber, float speed);
