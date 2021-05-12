#pragma once

#ifndef DEFINE_H__
#define DEFINE_H__

// RootSignature
#define ParameterCount 6

#define GRAVITY 9.8f

// Model
	// Character
#define CHARACTER_WIZARD_01 "wizard_01"
#define WIZARD_FIRE 99
	// Tree
#define TREE_01 "Tree_01"
	// Rock
#define ROCK_02 "Rock_02"
	// House
#define HOUSE_02 "House_02"
	// Tile
#define TILE_01 "Tile_01"


#define KEY_W		0x00000001 // 0000 0000 0000 0000 0000 0000 0000 0001
#define KEY_A		0x00000002 // 0000 0000 0000 0000 0000 0000 0000 0010
#define KEY_S		0x00000004 // 0000 0000 0000 0000 0000 0000 0000 0100
#define KEY_D		0x00000008 // 0000 0000 0000 0000 0000 0000 0000 1000
#define KEY_SPACE	0x00000010 // 0000 0000 0000 0000 0000 0000 0001 0000
#define KEY_LBUTTON	0x00000020 // 0000 0000 0000 0000 0000 0000 0010 0000
#define KEY_E		0x00000040 // 0000 0000 0000 0000 0000 0000 0100 0000

#define SERVERPORT 9000

// function
#define SCint static_cast<int> 

// Magician Character 
#define M_MoveForward_Speed 0.05f
#define M_MoveLeft_Speed 0.05f
#define M_MoveRight_Speed 0.05f
#define M_MoveBackward_Speed 0.03f


#define NETWOR
#define PHYSX

#define MAX_BUFFER 16384
#define MIN_BUFFER 256

// protocol
#define packet_skill 21
#define packet_keyInput 22
#define packet_ready 23
#define ctos_playerInfo 25
#define stoc_startInfo 26
#define stoc_sceneChange 27
#define stoc_otherPlayerNum 29
#define stoc_OtherstartInfo 30
#define stoc_playerInfo 31
#define ctos_LoadingEnd 33
#define stoc_SkillPacket 34
#define stoc_bullet 35

// skill Type 51 ~ 59 
#define FIRE_RING 51 

#endif