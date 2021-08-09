#pragma once

#ifndef DEFINE_H__
#define DEFINE_H__

//Scene
#define LOADING_Scene	0
#define TITLE_Scene		1
#define LOBBY_Scene		2
#define ROOM_Scene		3
#define STAGE_Scene		4
#define RESULT_Scene	5
#define ENDING_Scene	6

// RootSignature
#define ParameterCount 6

#define GRAVITY 9.8f

// Model
	// Character
#define CHARACTER_WIZARD_FIRE "wizard_Fire"
#define CHARACTER_WIZARD_COLD "wizard_Cold"
#define CHARACTER_WIZARD_DARKNESS "wizard_Darkness"
	// Tree
#define TREE_01 "Tree_01"
	// Rock
#define ROCK_02 "Rock_02"
	// House
#define HOUSE_02 "House_02"
	// Tile
#define TILE_01 "Tile_01"

// Button


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
#define PHYS

#define MAX_BUFFER 4096
#define MIN_BUFFER 256

#define BulletCB_Count 70

// skill Type 51 ~ 59 
#define FIRE_RING 51 


// eventType
// Loading Scene Event  0 ~ 30
#define EVENT_LOADING_LOADINGEND	0
#define EVENT_CTOS_CONNECT_OK		1
#define EVENT_SCENE_CHANGE			2

// Title Scene Event	31 ~ 60
#define EVENT_TITLE_LOGIN 31


// Lobby Scene Event	61 ~ 90
#define EVENT_LOBBY_ROOMLIST_SHOW			61
#define EVENT_LOBBY_ROOM_MAKE_REQUEST		62
#define EVENT_LOBBY_ROOM_JOIN_REQUEST		63



// Room Scene Event		91 ~ 120
#define EVENT_ROOM_PLAYER_ENTER				91
#define EVENT_ROOM_PLAYER_READY_ON			92
#define EVENT_ROOM_PLAYER_READY_OFF			93
#define EVENT_ROOM_PLAYER_SELECT_CHARACTER	94
#define EVENT_ROOM_PLAYER_EXIT				95
#define EVENT_ROOM_PLAYER_SELECT_TEAM		96


// Stage Scene Event	121 ~ 150
#define	EVENT_STAGE_GAME_START		121
#define EVENT_STAGE_INPUT_MOVE		122
#define EVENT_STAGE_INPUT_BASIC_ATTACK	123
#define EVENT_STAGE_PLAYER_INFO			124
#define	EVENT_STAGE_PLAYER_ANIMATE		125
#define EVENT_STAGE_CAMERA_UPDATE		126

// Result Scene Event	151 ~ 180


// Ending Scene Event	180 ~ 210

#endif