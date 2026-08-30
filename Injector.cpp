#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

// Get process ID by executable name
DWORD GetProcessID(const std::string& processName) {
    DWORD processID = 0;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    
    if (snapshot == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to create process snapshot" << std::endl;
        return 0;
    }
    
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);
    
    if (Process32First(snapshot, &entry)) {
        do {
            if (processName == entry.szExeFile) {
                processID = entry.th32ProcessID;
                break;
            }
        } while (Process32Next(snapshot, &entry));
    }
    
    CloseHandle(snapshot);
    return processID;
}

// Inject DLL into target process
bool InjectDLL(DWORD processID, const std::string& dllPath) {
    // Verify DLL exists
    if (!fs::exists(dllPath)) {
        std::cerr << "DLL file not found: " << dllPath << std::endl;
        return false;
    }
    
    // Open the target process
    HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
    if (!process) {
        std::cerr << "Failed to open process with ID: " << processID << std::endl;
        std::cerr << "Error code: " << GetLastError() << std::endl;
        return false;
    }
    
    // Allocate memory in the target process for the DLL path
    size_t dllPathSize = dllPath.length() + 1;
    void* allocatedMemory = VirtualAllocEx(process, NULL, dllPathSize, 
                                           MEM_COMMIT, PAGE_READWRITE);
    if (!allocatedMemory) {
        std::cerr << "Failed to allocate memory in target process" << std::endl;
        CloseHandle(process);
        return false;
    }
    
    // Write DLL path to the allocated memory
    if (!WriteProcessMemory(process, allocatedMemory, (void*)dllPath.c_str(), 
                           dllPathSize, NULL)) {
        std::cerr << "Failed to write DLL path to target process" << std::endl;
        VirtualFreeEx(process, allocatedMemory, 0, MEM_RELEASE);
        CloseHandle(process);
        return false;
    }
    
    // Get address of LoadLibraryA in kernel32.dll
    HMODULE kernel32 = GetModuleHandle("kernel32.dll");
    FARPROC loadLibraryAddr = GetProcAddress(kernel32, "LoadLibraryA");
    
    if (!loadLibraryAddr) {
        std::cerr << "Failed to get LoadLibraryA address" << std::endl;
        VirtualFreeEx(process, allocatedMemory, 0, MEM_RELEASE);
        CloseHandle(process);
        return false;
    }
    
    // Create a remote thread that calls LoadLibraryA with our DLL path
    HANDLE thread = CreateRemoteThread(process, NULL, 0, 
                     (LPTHREAD_START_ROUTINE)loadLibraryAddr, 
                     allocatedMemory, 0, NULL);
    
    if (!thread) {
        std::cerr << "Failed to create remote thread" << std::endl;
        VirtualFreeEx(process, allocatedMemory, 0, MEM_RELEASE);
        CloseHandle(process);
        return false;
    }
    
    // Wait for the thread to complete
    WaitForSingleObject(thread, INFINITE);
    
    // Check if DLL was loaded successfully
    DWORD exitCode = 0;
    GetExitCodeThread(thread, &exitCode);
    
    if (!exitCode) {
        std::cerr << "DLL injection may have failed (LoadLibrary returned NULL)" << std::endl;
    }
    
    // Cleanup
    VirtualFreeEx(process, allocatedMemory, 0, MEM_RELEASE);
    CloseHandle(thread);
    CloseHandle(process);
    
    return true;
}

int main() {
    std::cout << "================================" << std::endl;
    std::cout << "Gorilla Tag Macro Menu Injector" << std::endl;
    std::cout << "================================" << std::endl << std::endl;
    
    // Common Gorilla Tag executable names
    std::string processNames[] = {
        "GorillaTag.exe",
        "GorillaTag",
        "Gorilla Tag.exe"
    };
    
    std::string dllPath;
    std::cout << "Enter the full path to GorillaTagMacroMenu.dll:" << std::endl;
    std::cout << "(Example: C:\\Users\\YourName\\GorillaTagMacroMenu.dll)" << std::endl;
    std::cout << "> ";
    std::getline(std::cin, dllPath);
    
    // Trim whitespace
    dllPath.erase(0, dllPath.find_first_not_of(" \t\n\r"));
    dllPath.erase(dllPath.find_last_not_of(" \t\n\r") + 1);
    
    // Remove quotes if present
    if (dllPath.front() == '"' && dllPath.back() == '"') {
        dllPath = dllPath.substr(1, dllPath.length() - 2);
    }
    
    std::cout << "\nSearching for Gorilla Tag process..." << std::endl;
    
    DWORD processID = 0;
    for (const auto& processName : processNames) {
        processID = GetProcessID(processName);
        if (processID) {
            std::cout << "Found Gorilla Tag! (PID: " << processID << ")" << std::endl;
            break;
        }
    }
    
    if (!processID) {
        std::cerr << "\nError: Could not find Gorilla Tag process!" << std::endl;
        std::cerr << "Make sure Gorilla Tag is running." << std::endl;
        std::cout << "\nPress Enter to exit...";
        std::cin.get();
        return 1;
    }
    
    std::cout << "\nInjecting DLL..." << std::endl;
    
    if (InjectDLL(processID, dllPath)) {
        std::cout << "\n✓ DLL injected successfully!" << std::endl;
        std::cout << "The macro menu should now be active in Gorilla Tag." << std::endl;
        return 0;
    } else {
        std::cerr << "\n✗ DLL injection failed!" << std::endl;
        std::cerr << "Make sure:" << std::endl;
        std::cerr << "  1. The DLL path is correct" << std::endl;
        std::cerr << "  2. Gorilla Tag is running" << std::endl;
        std::cerr << "  3. You have administrator privileges" << std::endl;
        return 1;
    }
    
    std::cout << "\nPress Enter to exit...";
    std::cin.get();
    return 0;
}
