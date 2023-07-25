#include "pch.h"
#include "utils/updater.h"
#include "utils/rpc.h"

LONG WINAPI CrashHandler(PEXCEPTION_POINTERS pInfo)
{
    DWORD code = pInfo->ExceptionRecord->ExceptionCode;
    if (code > 0x80000000)
    {
        Log::Print<eLogLevel::None>("");
        Log::Print<eLogLevel::Error>("Unhandled exception at {} (0x{:x})", pInfo->ExceptionRecord->ExceptionAddress, code);
    }

    return EXCEPTION_CONTINUE_SEARCH;
}

void MenuThread(void* param)
{
    // SetUnhandledExceptionFilter can get replaced by other dlls
    AddVectoredExceptionHandler(0, CrashHandler);
    while (true)
    {
        // FontMgr::Process();
        RPC::Process();
        Updater::Process();
        Sleep(2000);
    }
}

BOOL WINAPI DllMain(HINSTANCE hDllHandle, DWORD nReason, LPVOID Reserved)
{
    if (nReason == DLL_PROCESS_ATTACH)
    {
        uint gameVer = GetGameVersion();

#ifdef GTASA
        if (gameVer == GAME_10US_HOODLUM || gameVer == GAME_10US_COMPACT)
#else
        if (gameVer == BY_GAME(NULL, GAME_10EN, GAME_10EN))
#endif
        {
            CreateThread(nullptr, NULL, (LPTHREAD_START_ROUTINE)&MenuThread, nullptr, NULL, nullptr);
        }
        else
        {
            Log::Print<eLogLevel::Error>("Unknown game version. GTA " BY_GAME("SA v1.0 US Hoodlum or Compact", "VC v1.0 EN", "III v1.0 EN") " is required.");
            MessageBox(HWND_DESKTOP, "Unknown game version. GTA " BY_GAME("SA v1.0 US Hoodlum or Compact", "VC v1.0 EN", "III v1.0 EN") " is required.", FILE_NAME, MB_ICONERROR);
        }
    }

    return TRUE;
}