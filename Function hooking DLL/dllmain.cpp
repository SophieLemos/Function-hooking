// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <cstdio>

int(*ReturnAValueOriginal)();

int ReturnsAValueHooked() {
    return 99;
    return ReturnAValueOriginal();
}

const int FUNCTION_OFFSET = 0x11730;
HANDLE BASE_ADDRESS = GetModuleHandle(NULL);
const int ADDRESS_WITH_OFFSET = (int)BASE_ADDRESS + FUNCTION_OFFSET;

void HookFunction() {
    void* originalFunction = (int*)ADDRESS_WITH_OFFSET;
    void* hookedFunction = (int*)ReturnsAValueHooked;
    int length = 9;
    LPVOID trampoline = VirtualAlloc(0, length + 5, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    memcpy(trampoline, originalFunction, length);
    *(char*)((int)trampoline + length) = 0xE9;
    *(int*)((int)trampoline + length + 1) = (int)originalFunction - (int)trampoline - 5;
    DWORD memoryProtection;
    VirtualProtect(originalFunction, length, PAGE_EXECUTE_READWRITE, &memoryProtection);
    *(char*)((int)originalFunction) = 0xE9;
    *(int*)((int)originalFunction + 1) = (int)hookedFunction - (int)originalFunction - 5;
    VirtualProtect(originalFunction, length, memoryProtection, &memoryProtection);
    ReturnAValueOriginal = (int (*)())trampoline;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
        HookFunction();
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

