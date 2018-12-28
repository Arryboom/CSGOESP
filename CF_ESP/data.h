#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <graphics.h>
#include <math.h>
#include <Psapi.h>
#include <stdio.h>
#include "resource.h"
#pragma pack(push,1)
typedef struct MainPlayerInfo {
	int Health;
	int Camp;
	float x;
	float y;
	float z;
}MainPlayerInfo,*PMainPlayerInfo;
typedef struct PlayerInfo {
	int Health;
	int Camp;
	float x;
	float y;
	float z;
	bool Slp;
}PlayerInfo,*PPlayerInfo;
typedef struct BoneMatrix {
	float x;
	float y;
	float z;
}BoneMatrix, *PBoneMatrix;
#pragma pack(pop)
//Main.CPP
void WINAPI ReadPlayerInfoThread();
void WINAPI DrawThread();
DWORD_PTR WINAPI GetModuleBase(LPCWSTR ModuleName, DWORD pid);
DWORD WINAPI GetProcessIDD(LPCWSTR ProcessName);

//ListNode.CPP
PPlayerInfo WINAPI InitNode();
void WINAPI FreeNode(PPlayerInfo Node);