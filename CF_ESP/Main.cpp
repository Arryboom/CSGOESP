#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#include "data.h"
#pragma warning(disable:4996)
HWND GhWnd;
UINT Width = 300, Height = 300, GameWidth, GameHeight;
MainPlayerInfo MainPlayerInfoH = {0};
PlayerInfo EnemyInfo[80];
BoneMatrix EInfo[60][100];
float ViewMatrix[4][4] = { 0 };
RECT rect = { 0 };
int Lenr = 0, MaxPlayer = 0;
float DanViewY = 0, DanViewX = 0, ViewX = 0, ViewY = 0, mx = 0, my = 0, outx = 0, ouy = 0;
DWORD Attack = 0;
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPervInstance, LPSTR lCmdLine, int nCmdShow)
{
	HWND ClientHwnd;
	ClientHwnd = FindWindow(L"Valve001", NULL);
	GetClientRect(ClientHwnd, &rect);
	if (rect.right != 0 && rect.bottom != 0) {
		GameWidth = rect.right - rect.left;
		GameHeight = rect.bottom - rect.top;
	}
	GetWindowRect(ClientHwnd, &rect);
	if (rect.right != 0 && rect.bottom != 0) {
		Width = rect.right - rect.left+10;
		Height = rect.bottom - rect.top+10;
	}
	GhWnd = initgraph(Width, Height);
	SetWindowPos(GhWnd, HWND_TOPMOST, rect.left-5, rect.top-5, Width, Height, NULL);
	//SetWindowLong(GhWnd, GWL_STYLE, WS_VISIBLE);
	SetWindowLong(GhWnd, GWL_EXSTYLE, WS_EX_LAYERED);
	SetLayeredWindowAttributes(GhWnd, RGB(0, 0, 0), 255, LWA_COLORKEY);
	MSG msg;
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ReadPlayerInfoThread, NULL, NULL, NULL);
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)DrawThread, NULL, NULL, NULL);
	while (!GetMessage(&msg,NULL,NULL,NULL))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

//绘制线程
void WINAPI DrawThread()
{
	while (TRUE)
	{
		cleardevice();//清空屏幕
		BeginBatchDraw();
		float Vector[3] = { 0 }, D3DDistance = 0, QuadRant[2] = { 0 }, Fov = 0, Scale, D2DDistance;
		if (ViewY < -3.f)ViewY += 6.5f;
		float ox = (int)GameWidth / 2 - ViewX, oy = (int)GameHeight / 2 + ViewY;
		setfillcolor(RGB(255, 0, 0));
		solidcircle(ox, oy, 2);
		/*WCHAR buf[50] = { 0 };
		swprintf(buf, L"x:%f,y:%f", DanViewX, DanViewY);
		outtextxy((int)500, (int)300, buf);*/
		if (Attack == 5) {
			DWORD pid = GetProcessIDD(L"csgo.exe");
			DWORD EnBase = GetModuleBase(L"engine.dll", pid);
			EnBase += 0x58BCFC;
			HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
			if (!hProcess) {
				MessageBox(GhWnd, L"进程打开失败!", NULL, MB_ICONERROR);
				ExitProcess(0);
			}
			DWORD Basaddress = 0;
			float box = DanViewX * 1.9f, boy = DanViewY * 1.9f;
			if (outx < 0 && box < 0) {
				box = outx - box;
			}
			else if (outx > 0 && box < 0) {
				box = outx - box;
			}
			else if (outx < 0 && box > 0) {
				box = outx - box;
			}
			else if (outx > 0 && box > 0) {
				box = outx - box;
			}
			boy = ouy - boy;
			/*if (ouy < 0)boy = ouy - boy;
			else boy = ouy + boy;*/
			ReadProcessMemory(hProcess, LPVOID(EnBase), &Basaddress, sizeof(Basaddress), NULL); //获取房间人物地址
			if (DanViewY < -1.f && boy != 0) {
				WriteProcessMemory(hProcess, (LPVOID)(Basaddress + 0x4D10), &boy, sizeof(boy), nullptr);
			}
			if (DanViewX != 0 && box != 0) {
				WriteProcessMemory(hProcess, (LPVOID)(Basaddress + 0x4D14), &box, sizeof(box), nullptr);
			}
			CloseHandle(hProcess);
		}
		else {
			if (DanViewY > -1.f) {
				outx = mx;
				ouy = my;
			}
		}
		for (int i = 0; i < MaxPlayer; i++) {
			Vector[0] = MainPlayerInfoH.x - EnemyInfo[i].x;
			Vector[1] = MainPlayerInfoH.y - EnemyInfo[i].y;
			Vector[2] = MainPlayerInfoH.z - EnemyInfo[i].z;
			D2DDistance = sqrtf(powf(Vector[0], 2.0f) + powf(Vector[1], 2.0f));
			D3DDistance = sqrtf(powf(Vector[0], 2.0f) + powf(Vector[1], 2.0f) + powf(Vector[2], 2.0f));
			Scale = D3DDistance / 1000;
			float rew[2] = { 0 };
			rew[0] = EnemyInfo[i].x*ViewMatrix[0][0] + EnemyInfo[i].y*ViewMatrix[0][1] + EnemyInfo[i].z*ViewMatrix[0][2] + ViewMatrix[0][3];
			rew[1] = EnemyInfo[i].x*ViewMatrix[1][0] + EnemyInfo[i].y*ViewMatrix[1][1] + EnemyInfo[i].z*ViewMatrix[1][2] + ViewMatrix[1][3];
			float w = EnemyInfo[i].x*ViewMatrix[3][0] + EnemyInfo[i].y*ViewMatrix[3][1] + EnemyInfo[i].z*ViewMatrix[3][2] + ViewMatrix[3][3];
			
			if (w > 0 && EnemyInfo[i].Camp != MainPlayerInfoH.Camp && EnemyInfo[i].Health > 0 && EnemyInfo[i].Camp > 0 && EnemyInfo[i].Slp == false) {
				float x = (float)GameWidth / 2;
				float y = (float)GameHeight / 2;
				rew[0] *= 1.0f / w;
				rew[1] *= 1.0f / w;
				x += 0.5f*rew[0] * GameWidth + 0.5f;
				y -= 0.5f*rew[1] * GameHeight + 0.5f;
				setcolor(RGB(255, 156, 0));
				rectangle((int)x - 10 / Scale, (int)y, (int)x + 10 / Scale, (int)y - 45 / Scale);

			}
//#pragma region 绘制骨骼
//			float Ire[13][2] = { 0 };
//			for (int ic = 0; ic < Lenr; ic++) {
//				if (ic!=0
//					) {
//					if (EInfo[i][ic].x != 0 && EInfo[i][ic].y != 0 && EInfo[i][ic].z != 0) {
//						float rew2[2] = { 0 };
//						rew2[0] = EInfo[i][ic].x*ViewMatrix[0][0] + EInfo[i][ic].y*ViewMatrix[0][1] + EInfo[i][ic].z*ViewMatrix[0][2] + ViewMatrix[0][3];
//						rew2[1] = EInfo[i][ic].x*ViewMatrix[1][0] + EInfo[i][ic].y*ViewMatrix[1][1] + EInfo[i][ic].z*ViewMatrix[1][2] + ViewMatrix[1][3];
//						float w2 = EInfo[i][ic].x*ViewMatrix[3][0] + EInfo[i][ic].y*ViewMatrix[3][1] + EInfo[i][ic].z*ViewMatrix[3][2] + ViewMatrix[3][3];
//						if (w2 > 0) {
//							float x2 = (float)GameWidth / 2;
//							float y2 = (float)GameHeight / 2;
//							rew2[0] *= 1.0f / w2;
//							rew2[1] *= 1.0f / w2;
//							x2 += 0.5f*rew2[0] * GameWidth + 0.5f;
//							y2 -= 0.5f*rew2[1] * GameHeight + 0.5f;
//							WCHAR res[25] = { 0 };
//							if (EnemyInfo[i].Camp != MainPlayerInfoH.Camp) {
//								wsprintf(res, L"%d,%d", ic,i);
//								//outtextxy((int)x2, (int)y2, res);
//								//setfillcolor(RGB(155, 156, 134));
//								solidcircle((int)x2, (int)y2, 1);
//								//line((int)x2, (int)y2, x2, y2);
//							}
////#pragma region 骨骼连线
////							switch (ic)
////							{
////							case 0:
////								setfillcolor(RGB(155, 156, 134));
////								solidcircle((int)x2, (int)y2, 1);
////								Ire[0][0] = x2;
////								Ire[0][1] = y2;
////								line(Ire[0][0], Ire[0][1], x2, y2);
////								break;
////							case 6:
////								setfillcolor(RGB(155, 156, 0));
////								solidcircle((int)x2, (int)y2, 1);
////								Ire[1][0] = x2;
////								Ire[1][1] = y2;
////								line(Ire[0][0], Ire[0][1], x2, y2);
////								break;
////							case 8:
////								setfillcolor(RGB(155, 156, 0));
////								solidcircle((int)x2, (int)y2, 1);
////								Ire[2][0] = x2;
////								Ire[2][1] = y2;
////								line(Ire[1][0], Ire[1][1], x2, y2);
////								break;
////							case 10:
////								setfillcolor(RGB(155, 156, 0));
////								solidcircle((int)x2, (int)y2, 1);
////								Ire[3][0] = x2;
////								Ire[3][1] = y2;
////								line(Ire[1][0], Ire[1][1], x2, y2);
////								break;
////							case 11:
////								setfillcolor(RGB(155, 156, 0));
////								solidcircle((int)x2, (int)y2, 1);
////								Ire[4][0] = x2;
////								Ire[4][1] = y2;
////								line(Ire[3][0], Ire[3][1], x2, y2);
////								break;
////							case 12:
////								setfillcolor(RGB(155, 156, 0));
////								solidcircle((int)x2, (int)y2, 1);
////								Ire[5][0] = x2;
////								Ire[5][1] = y2;
////								line(Ire[4][0], Ire[4][1], x2, y2);
////								break;
////							case 38:
////								setfillcolor(RGB(155, 156, 0));
////								solidcircle((int)x2, (int)y2, 1);
////								Ire[6][0] = x2;
////								Ire[6][1] = y2;
////								line(Ire[1][0], Ire[1][1], x2, y2);
////								break;
////							case 39:
////								setfillcolor(RGB(155, 156, 0));
////								solidcircle((int)x2, (int)y2, 1);
////								Ire[7][0] = x2;
////								Ire[7][1] = y2;
////								line(Ire[6][0], Ire[6][1], x2, y2);
////								break;
////							case 40:
////								setfillcolor(RGB(155, 156, 0));
////								solidcircle((int)x2, (int)y2, 1);
////								Ire[8][0] = x2;
////								Ire[8][1] = y2;
////								line(Ire[7][0], Ire[7][1], x2, y2);
////								break;
////							case 66:
////								setfillcolor(RGB(155, 156, 0));
////								solidcircle((int)x2, (int)y2, 1);
////								Ire[9][0] = x2;
////								Ire[9][1] = y2;
////								line(Ire[0][0], Ire[0][1], x2, y2);
////								break;
////							case 67:
////								setfillcolor(RGB(155, 156, 0));
////								solidcircle((int)x2, (int)y2, 1);
////								Ire[10][0] = x2;
////								Ire[10][1] = y2;
////								line(Ire[9][0], Ire[9][1], x2, y2);
////								break;
////							case 73:
////								setfillcolor(RGB(155, 156, 0));
////								solidcircle((int)x2, (int)y2, 1);
////								Ire[11][0] = x2;
////								Ire[11][1] = y2;
////								line(Ire[0][0], Ire[0][1], x2, y2);
////								break;
////							case 74:
////								setfillcolor(RGB(155, 156, 0));
////								solidcircle((int)x2, (int)y2, 1);
////								Ire[12][0] = x2;
////								Ire[12][1] = y2;
////								line(Ire[11][0], Ire[11][1], x2, y2);
////								break;
////							default:
////								break;
////							}
////#pragma endregion
//						}
//					}
//				}
//			}
//#pragma endregion
		}
		EndBatchDraw();
		Sleep(1);
	}
}
//读取玩家信息处理线程
void WINAPI ReadPlayerInfoThread()
{
	HANDLE hToken;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
		MessageBox(GhWnd, L"权限提升失败!", NULL, MB_ICONERROR);
		ExitProcess(0);
	}
	LUID luid;
	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid)) {
		MessageBox(GhWnd, L"权限提升失败!", NULL, MB_ICONERROR);
		ExitProcess(0);
	}
	TOKEN_PRIVILEGES Token_Priv;
	Token_Priv.PrivilegeCount = 1;
	Token_Priv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	Token_Priv.Privileges[0].Luid = luid;
	if (!AdjustTokenPrivileges(hToken, FALSE, &Token_Priv, sizeof(TOKEN_PRIVILEGES), NULL, NULL)) {
		CloseHandle(hToken);
		MessageBox(GhWnd, L"权限提升失败!", NULL, MB_ICONERROR);
		ExitProcess(0);
	}
	CloseHandle(hToken);
	DWORD pid = GetProcessIDD(L"csgo.exe");
	DWORD ModuleBase = GetModuleBase(L"client_panorama.dll", pid);
	DWORD EnemyPlayerBase = ModuleBase;
	EnemyPlayerBase += 0x4CCDBFC;
	DWORD MainPlayerBase = ModuleBase;
	MainPlayerBase += 0xCBD6B4;
	DWORD MaxPlayerBase = ModuleBase;
	DWORD ViewBase = ModuleBase;
	ViewBase += 0x4CBF614;
	MaxPlayerBase += 0x0507113C;
	DWORD BoneMatrixBase = ModuleBase;
	BoneMatrixBase += 0x04C5C48c;
	DWORD EnBase = GetModuleBase(L"engine.dll", pid);
	EnBase += 0x58BCFC;
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (!hProcess) {
		MessageBox(GhWnd, L"进程打开失败!", NULL, MB_ICONERROR);
		ExitProcess(0);
	}
	while (TRUE)
	{
		DWORD BaseAddress = MainPlayerBase, TmpAddress = 0, MainPlayerTmp = 0;
		int Maincamp = 0;
		float Mainxyz[3];
		int MainHealth;
		ReadProcessMemory(hProcess, LPVOID(MainPlayerBase), &BaseAddress, sizeof(BaseAddress), NULL); //获取房间人物地址
		ReadProcessMemory(hProcess, (LPVOID)(BaseAddress + 0x100), &MainHealth, sizeof(MainHealth), NULL);//主玩家血量
		ReadProcessMemory(hProcess, (LPVOID)(BaseAddress + 0xF4), &Maincamp, sizeof(Maincamp), NULL);//主玩家阵营信息
		ReadProcessMemory(hProcess, (LPVOID)(BaseAddress + 0x138), &Mainxyz, sizeof(Mainxyz), NULL);//主玩家坐标
		ReadProcessMemory(hProcess, (LPVOID)(BaseAddress + 0x3030), &DanViewX, sizeof(DanViewX), NULL);
		ReadProcessMemory(hProcess, (LPVOID)(BaseAddress + 0x302C), &DanViewY, sizeof(DanViewY), NULL);
		ReadProcessMemory(hProcess, (LPVOID)(BaseAddress + 0x3038), &ViewY, sizeof(ViewY), NULL);
		ReadProcessMemory(hProcess, (LPVOID)(BaseAddress + 0x303C), &ViewX, sizeof(ViewX), NULL);
		ReadProcessMemory(hProcess, (LPVOID)(ModuleBase + 0x30FF2A0), &Attack, sizeof(Attack), NULL);
		ReadProcessMemory(hProcess, LPVOID(ViewBase), &ViewMatrix, sizeof(ViewMatrix), NULL); //获取房间人物地址
		DWORD Basaddress = 0;
		ReadProcessMemory(hProcess, LPVOID(EnBase), &Basaddress, sizeof(Basaddress), NULL); //获取房间人物地址
		ReadProcessMemory(hProcess, (LPVOID)(Basaddress + 0x4D14), &mx, sizeof(mx), NULL);
		ReadProcessMemory(hProcess, (LPVOID)(Basaddress + 0x4D10), &my, sizeof(my), NULL);
		//获取房间玩家人数
		ReadProcessMemory(hProcess, LPVOID(MaxPlayerBase), &BaseAddress, sizeof(BaseAddress), NULL); //获取房间最大玩家人数地址
		ReadProcessMemory(hProcess, (LPVOID)(BaseAddress + 0x6c), &BaseAddress, sizeof(BaseAddress), NULL);
		ReadProcessMemory(hProcess, (LPVOID)(BaseAddress + 0x208), &MaxPlayer, sizeof(MaxPlayer), NULL);//获取房间最大玩家人数
		if (MaxPlayer <= 0 || MaxPlayer>0x100) {
			MaxPlayer = 65;
		}
		MainPlayerInfoH.Health = MainHealth;
		MainPlayerInfoH.Camp = Maincamp;
		MainPlayerInfoH.x = Mainxyz[0];
		MainPlayerInfoH.y = Mainxyz[1];
		MainPlayerInfoH.z = Mainxyz[2];
		int OL = 0;
		for (int i = 0; i < MaxPlayer+1; i++) {
			int Health = 0, Camp = 0;
			ReadProcessMemory(hProcess, LPVOID(EnemyPlayerBase + (i * 0x10)), &BaseAddress, sizeof(BaseAddress), NULL); //获取房间人物地址
			ReadProcessMemory(hProcess, (LPVOID)(BaseAddress + 0x100), &Health, sizeof(Health), NULL);//血量
			ReadProcessMemory(hProcess, (LPVOID)(BaseAddress + 0xF4), &Camp, sizeof(Camp), NULL);//阵营
			float xyz[3] = { 0 };
			ReadProcessMemory(hProcess, (LPVOID)(BaseAddress + 0x138), &xyz, sizeof(xyz), NULL);//坐标
			DWORD Slp = 0;
			ReadProcessMemory(hProcess, (LPVOID)(BaseAddress + 0xED), &Slp, sizeof(Slp), NULL);//坐标
			if (fabsf(xyz[0] - MainPlayerInfoH.x) > 1.0f&&fabsf(xyz[1] - MainPlayerInfoH.y) > 1.0f&& Camp != 0) {
				EnemyInfo[OL].x = xyz[0];
				EnemyInfo[OL].y = xyz[1];
				EnemyInfo[OL].z = xyz[2];
				EnemyInfo[OL].Health = Health;
				EnemyInfo[OL].Camp = Camp;
				EnemyInfo[OL].Slp = Slp;
				OL++;
			}

			////=============获取骨骼矩阵;
			//Lenr = 94;
			/*ReadProcessMemory(hProcess, LPVOID(BoneMatrixBase + (i * 0x8)), &BaseAddress, sizeof(BaseAddress), NULL); 
			ReadProcessMemory(hProcess, (LPVOID)(BaseAddress + 0x1C), &BaseAddress, sizeof(BaseAddress), NULL);
			ReadProcessMemory(hProcess, (LPVOID)(BaseAddress + 0x5C), &BaseAddress, sizeof(BaseAddress), NULL);
			ReadProcessMemory(hProcess, (LPVOID)(BaseAddress + 0x50), &BaseAddress, sizeof(BaseAddress), NULL);*/
			//ReadProcessMemory(hProcess, (LPVOID)(BaseAddress + 0x26A8), &BaseAddress, sizeof(BaseAddress), NULL);//坐标
			//BaseAddress -= 0x4;
			//for (int ir = 0; ir < Lenr; ++ir) {
			//	ReadProcessMemory(hProcess, (LPVOID)(BaseAddress + 0x10), &EInfo[i][ir].x, sizeof(float), NULL);//坐标
			//	ReadProcessMemory(hProcess, (LPVOID)(BaseAddress + 0x20), &EInfo[i][ir].y, sizeof(float), NULL);//坐标
			//	ReadProcessMemory(hProcess, (LPVOID)(BaseAddress + 0x30), &EInfo[i][ir].z, sizeof(float), NULL);//坐标
			//	BaseAddress += 0x30;
			//}
		}
		
		Sleep(1);
	}
	CloseHandle(hProcess);
}
//获取进程ID
DWORD WINAPI GetProcessIDD(LPCWSTR ProcessName)
{
	DWORD pid = 0;
	HANDLE SnapShot;
	SnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	PROCESSENTRY32 ProcessEntry32;
	ProcessEntry32.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(SnapShot, &ProcessEntry32)) {
		return 0;
	}
	do {
		if (wcscmp(ProcessName, ProcessEntry32.szExeFile) == 0) {
			pid = ProcessEntry32.th32ProcessID;
			break;
		}
	} while (Process32Next(SnapShot, &ProcessEntry32));
	return pid;
}
//获取模块地址
DWORD_PTR WINAPI GetModuleBase(LPCWSTR ModuleName,DWORD pid)
{
	HANDLE SnapShot;
	SnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
	MODULEENTRY32 ModuleEntry32;
	ModuleEntry32.dwSize = sizeof(MODULEENTRY32);
	if(!Module32First(SnapShot,&ModuleEntry32)){
		return 0;
	}
	DWORD_PTR Base = 0;
	do {
		if (wcscmp(ModuleEntry32.szModule, ModuleName) == 0) {
			Base = (DWORD_PTR)ModuleEntry32.modBaseAddr;
			break;
		}
	} while (Module32Next(SnapShot, &ModuleEntry32));
	return Base;
}