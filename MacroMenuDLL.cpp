#include "MacroMenu.h"
#include <windows.h>

// Global macro menu instance
MacroMenu* g_MacroMenu = nullptr;

// DLL Entry Point
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
            // Initialize macro menu on DLL load
            g_MacroMenu = new MacroMenu();
            break;
        case DLL_PROCESS_DETACH:
            // Cleanup on DLL unload
            if (g_MacroMenu) {
                delete g_MacroMenu;
                g_MacroMenu = nullptr;
            }
            break;
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
            break;
    }
    return TRUE;
}

// Export functions for external use
extern "C" {
    __declspec(dllexport) MacroMenu* GetMacroMenuInstance() {
        return g_MacroMenu;
    }

    __declspec(dllexport) void StartRecording(const char* macroName) {
        if (g_MacroMenu) {
            g_MacroMenu->StartRecording(macroName);
        }
    }

    __declspec(dllexport) void StopRecording() {
        if (g_MacroMenu) {
            g_MacroMenu->StopRecording();
        }
    }

    __declspec(dllexport) void RecordMacroFrame(float timestamp, 
                                               float hx, float hy, float hz,
                                               float hrx, float hry, float hrz, float hrw,
                                               float lhx, float lhy, float lhz,
                                               float lhrx, float lhry, float lhrz, float lhrw,
                                               float rhx, float rhy, float rhz,
                                               float rhrx, float rhry, float rhrz, float rhrw,
                                               float bx, float by, float bz,
                                               float brx, float bry, float brz, float brw) {
        if (g_MacroMenu) {
            MacroFrame frame;
            frame.timestamp = timestamp;
            frame.headPosition = glm::vec3(hx, hy, hz);
            frame.headRotation = glm::quat(hrw, hrx, hry, hrz);
            frame.leftHandPosition = glm::vec3(lhx, lhy, lhz);
            frame.leftHandRotation = glm::quat(lhrw, lhrx, lhry, lhrz);
            frame.rightHandPosition = glm::vec3(rhx, rhy, rhz);
            frame.rightHandRotation = glm::quat(rhrw, rhrx, rhry, rhrz);
            frame.bodyPosition = glm::vec3(bx, by, bz);
            frame.bodyRotation = glm::quat(brw, brx, bry, brz);
            
            g_MacroMenu->RecordFrame(frame);
        }
    }

    __declspec(dllexport) void PlayMacro(const char* macroName) {
        if (g_MacroMenu) {
            g_MacroMenu->PlayMacro(macroName);
        }
    }

    __declspec(dllexport) void StopMacroPlayback() {
        if (g_MacroMenu) {
            g_MacroMenu->StopPlayback();
        }
    }

    __declspec(dllexport) void UpdateMacroPlayback(float deltaTime) {
        if (g_MacroMenu) {
            g_MacroMenu->UpdatePlayback(deltaTime);
        }
    }

    __declspec(dllexport) bool IsRecording() {
        if (g_MacroMenu) {
            return g_MacroMenu->IsRecording();
        }
        return false;
    }

    __declspec(dllexport) bool IsPlayingBack() {
        if (g_MacroMenu) {
            return g_MacroMenu->IsPlayingBack();
        }
        return false;
    }

    __declspec(dllexport) void SaveMacroToFile(const char* macroName, const char* filePath) {
        if (g_MacroMenu) {
            g_MacroMenu->SaveMacro(macroName, filePath);
        }
    }

    __declspec(dllexport) void LoadMacroFromFile(const char* filePath) {
        if (g_MacroMenu) {
            g_MacroMenu->LoadMacro(filePath);
        }
    }

    __declspec(dllexport) void DeleteMacro(const char* macroName) {
        if (g_MacroMenu) {
            g_MacroMenu->DeleteMacro(macroName);
        }
    }

    __declspec(dllexport) void ListAllMacros() {
        if (g_MacroMenu) {
            g_MacroMenu->ListMacros();
        }
    }

    __declspec(dllexport) void ExportMacroLibrary(const char* filePath) {
        if (g_MacroMenu) {
            g_MacroMenu->ExportMacroLibrary(filePath);
        }
    }

    __declspec(dllexport) void ImportMacroLibrary(const char* filePath) {
        if (g_MacroMenu) {
            g_MacroMenu->ImportMacroLibrary(filePath);
        }
    }

    __declspec(dllexport) void SetMacroLooping(const char* macroName, bool shouldLoop) {
        if (g_MacroMenu) {
            Macro* macro = g_MacroMenu->GetMacro(macroName);
            if (macro) {
                macro->isLooping = shouldLoop;
            }
        }
    }
}
