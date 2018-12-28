// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include <Psapi.h>
DWORD APIENTRY FindPatternV2(char* moduleName, char* pattern)
{
	const char* pat = pattern;
	DWORD firstMatch = 0;
	DWORD rangeStart = (DWORD)GetModuleHandleA(moduleName);
	//DWORD pid = GetProcessIDD(L"csgo.exe");
	//HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	MODULEINFO miModInfo; GetModuleInformation(GetCurrentProcess(), (HMODULE)rangeStart, &miModInfo, sizeof(MODULEINFO));
	DWORD rangeEnd = rangeStart + miModInfo.SizeOfImage;
	for (DWORD pCur = rangeStart; pCur < rangeEnd; pCur++)
	{
		if (!*pat) {
			//CloseHandle(hProcess);
			return firstMatch;
		}

		if (*(PBYTE)pat == '\?' || *(BYTE*)pCur == (BYTE)pat)
		{
			if (!firstMatch)
				firstMatch = pCur;

			if (!pat[2])
				return firstMatch;

			if (*(PWORD)pat == '\?\?' || *(PBYTE)pat != '\?')
				pat += 3;

			else
				pat += 2;    //one ?
		}
		else
		{
			pat = pattern;
			firstMatch = 0;
		}
	}
	//CloseHandle(hProcess);
	return NULL;
}
void APIENTRY Thread2() {
	OutputDebugStringA("sdwW");
	DWORD ir = FindPatternV2("client_panorama.dll", "0F 10 05 ? ? ? ? 8D 85 ? ? ? ? B9");
	char buf[1024] = { 0 };
	wsprintfA(buf, "0x%08X", ir);
	OutputDebugStringA(buf);
}
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		OutputDebugStringA("ISNFKW");
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Thread2, NULL, NULL, NULL);
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

