#include <windows.h>
#include <stdio.h>
#include <tlhelp32.h> // Module snapshot için

// Hedef process ID ile module base adresi al
HMODULE GetModuleBase(DWORD pid, const wchar_t* moduleName) {
    HMODULE hModule = NULL;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
    if (hSnap != INVALID_HANDLE_VALUE) {
        MODULEENTRY32 me = { 0 };
        me.dwSize = sizeof(MODULEENTRY32);
        if (Module32First(hSnap, &me)) {
            do {
                if (wcscmp(me.szModule, moduleName) == 0) {
                    hModule = me.hModule;
                    break;
                }
            } while (Module32Next(hSnap, &me));
        }
        CloseHandle(hSnap);
    }
    return hModule;
}

int main() {
    HANDLE stdOutput = GetStdHandle(STD_OUTPUT_HANDLE);

    DWORD pid = 29308; // hedef process ID
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (!hProcess) {
        printf("Process can't open! Error: %lu\n", GetLastError());
        return 1;
    }

    HMODULE base = GetModuleBase(pid, L"GameAssembly.dll");
    if (!base) {
        printf("Module not found\n");
        CloseHandle(hProcess);
        return 1;
    }

    uintptr_t ptr = (uintptr_t)base + 0x03FA61D8;
    SIZE_T bytesRead;
    SIZE_T bytesWrite;
    uintptr_t temp;

    // pointer zincirini takip et
    DWORD offsets[] = { 0x78, 0xB8, 0x8, 0x10, 0x38, 0xA0, 0x168 };
    for (int i = 0; i < sizeof(offsets) / sizeof(offsets[0]); i++) {
        if (!ReadProcessMemory(hProcess, (LPCVOID)ptr, &temp, sizeof(temp), &bytesRead) || bytesRead != sizeof(temp)) {
            printf("ReadProcessMemory error!\n");
            CloseHandle(hProcess);
            return 1;
        }
        ptr = temp + offsets[i];
    }

    float *can = (float*)malloc(sizeof(uintptr_t));

    while (TRUE) {
        ReadProcessMemory(hProcess, (LPCVOID)ptr, can, sizeof(float), &bytesRead);
        printf("can: %f\n", *can);
        *can += 5.f;
        WriteProcessMemory(hProcess, (LPVOID)ptr, can, sizeof(float), &bytesWrite);
        SetConsoleCursorPosition(stdOutput, { 0,0 });
        Sleep(1000);
    }


    CloseHandle(hProcess);
    return 0;
}
