#include <Windows.h>
#include <stdio.h>

HMODULE mydll_handle = NULL;
HANDLE stdOutputHandle = NULL;

void CreateConsole() {
    if (!AllocConsole()) {
        FreeLibraryAndExitThread(mydll_handle, GetLastError());
    }
    HWND hwnd = GetConsoleWindow();
    ShowWindow(hwnd, SW_SHOW);
    SetForegroundWindow(hwnd);
    FILE* fOut;
    freopen_s(&fOut, "CONOUT$", "w", stdout);
    stdOutputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
}

void init() {
    CreateConsole();
}

int main() {
    init();
    HMODULE base = GetModuleHandleA("GameAssembly.dll");
    uintptr_t ptr = (uintptr_t)base + 0x03FA61D8;
    ptr = *(uintptr_t*)ptr + 0x78;
    ptr = *(uintptr_t*)ptr + 0xB8;
    ptr = *(uintptr_t*)ptr + 0x8;
    ptr = *(uintptr_t*)ptr + 0x10;
    ptr = *(uintptr_t*)ptr + 0x38;
    ptr = *(uintptr_t*)ptr + 0xA0;
    ptr = *(uintptr_t*)ptr + 0x168;

    while (TRUE) {
        SetConsoleCursorPosition(stdOutputHandle, { 0,0 });
        printf("can %f", *(float*)ptr);
        *(float*)ptr += 31.f;
        Sleep(1000);
    }
    
}


BOOL APIENTRY DllMain(HMODULE hModule, uintptr_t  ul_reason_for_call, LPVOID lpReserved)
{
    mydll_handle = hModule;
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:

        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)main, 0, 0, 0);
        break;

    case DLL_THREAD_ATTACH:
        break;

    case DLL_THREAD_DETACH:
        break;

    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}