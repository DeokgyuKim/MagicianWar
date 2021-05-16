#pragma once

#ifndef DEFINE_H__
#define DEFINE_H__

//Scene
#define LOADING_Scene	0
#define LOBBY_Scene		1
#define STAGE_Scene		2
#define ENDING_Scene	3

// RootSignature
#define ParameterCount 6

#define GRAVITY 9.8f

// Model
	// Character
#define CHARACTER_WIZARD_FIRE "wizard_Fire"
#define WIZARD_FIRE 99
#define CHARACTER_WIZARD_COLD "wizard_Cold"
#define WIZARD_COLD 98
#define CHARACTER_WIZARD_DARKNESS "wizard_Darkness"
#define WIZARD_DARKNESS 96
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


#define NETWORK
#define PHYS

#define MAX_BUFFER 16384
#define MIN_BUFFER 256

#define BulletCB_Count 70

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
#define stoc_gameend 36

// skill Type 51 ~ 59 
#define FIRE_RING 51 

#endif