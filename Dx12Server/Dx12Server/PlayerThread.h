#pragma once

#include "include.h"

class Player;

int recvn(SOCKET s, char* buf, int len, int flags);
void PlayerThread(STOC_ServerPlayer arg);
void packetProcessing(STOC_ServerPlayer arg);
void WorkThread();
void Physics_Collision(int id);
void UpdatePlayerInfoPacket(int id, Player& _player);



//void UpdatePosition(DWORD _playerNumber);
//void MoveForward(DWORD _playerNumber, float speed);
//void MoveBackward(DWORD _playerNumber, float speed);
//void MoveLeft(DWORD _playerNumber, float speed);
//void MoveRight(DWORD _playerNumber, float speed);
