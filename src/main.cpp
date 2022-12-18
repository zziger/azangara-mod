#include <iostream>
#include <locale>
#include <Windows.h>
#include <thread>

#include "main.h"

#include <filesystem>
#include <sstream>
#include <memory>
#include "memory/Memory.h"

bool enabled = false;

int (__thiscall *render_ui_orig)(void* this_);
int __fastcall render_ui(void* this_, void*) {
    // auto state = GetAsyncKeyState(0x31);
    // if ((state & 0x8001) == 0x8001) enabled = !enabled;
    return enabled ? render_ui_orig(this_) : 0;
}

LRESULT (__thiscall *handle_wnd_msg_orig)(void* this_, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
LRESULT __fastcall handle_wnd_msg(void* this_, void*, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
    if (Msg == WM_KEYDOWN && wParam == 0x31) {
        enabled = !enabled;
    }
    return handle_wnd_msg_orig(this_, hWnd, Msg, wParam, lParam);
}

BOOL APIENTRY main(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        const auto base = (uintptr_t) GetModuleHandle(nullptr);
        
        Memory::Base() = base;
        Memory::OnPatternNotFound([](const std::string& pattern) {
            std::stringstream ss;
            ss << "Failed to find pattern " << pattern;
            MessageBoxA(nullptr, ss.str().c_str(), "azangara mod error", MB_OK);
        });

        MH_Initialize();
        // MessageBoxA(nullptr, "mod init", "azangara-mod", MB_OKCANCEL);
        constexpr Memory::Pattern renderUiPat("55 8B EC 83 E4 ? 83 EC ? 55");
        renderUiPat.Search().Detour((void*) render_ui, (void**) &render_ui_orig);

        constexpr Memory::Pattern handleWndMsgPat("55 8B E9 80 7D ? ? 75");
        handleWndMsgPat.Search().Detour((void*) handle_wnd_msg, (void**) &handle_wnd_msg_orig);
    }
    return TRUE;
}