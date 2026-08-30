# Setup and Usage Guide

## Quick Start

### Step 1: Build the DLL

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

The DLL will be at: `build/bin/GorillaTagMacroMenu.dll`

### Step 2: Build the Injector

```bash
# From the root directory
mkdir build_injector
cd build_injector
cmake -f ../CMakeLists_Injector.txt ..
cmake --build . --config Release
```

The injector will be at: `build_injector/bin/GorillaTagInjector.exe`

### Step 3: Run the Injector

1. **Start Gorilla Tag** (launch the game)
2. **Run the injector:**
   ```bash
   GorillaTagInjector.exe
   ```
3. **When prompted, enter the full path to the DLL:**
   ```
   C:\path\to\GorillaTagMacroMenu.dll
   ```
   
   Or paste the DLL file path directly into the console.

4. **If successful, you'll see:**
   ```
   ✓ DLL injected successfully!
   The macro menu should now be active in Gorilla Tag.
   ```

---

## Important Notes

### ⚠️ Administrator Privileges
The injector **requires administrator privileges** to inject the DLL. 

**How to run as admin:**
- Right-click `GorillaTagInjector.exe` → **Run as administrator**
- Or set it to always run as admin in Properties

### 🎮 Game Must Be Running
Gorilla Tag **must be running** before you inject the DLL. 

**Steps:**
1. Launch Gorilla Tag first
2. Then run the injector
3. Enter the DLL path

### 📁 DLL Path Format
- Use the **full absolute path**: `C:\Users\YourName\Documents\GorillaTagMacroMenu.dll`
- You can also drag-and-drop the DLL into the console window
- Don't use relative paths like `./GorillaTagMacroMenu.dll`

### ✅ Verification

After injection, the macro menu should be active. To verify:
1. Check if the game doesn't crash
2. The DLL's `DllMain()` will initialize the `MacroMenu` instance
3. All exported functions are now available to the game

---

## Troubleshooting

### "Failed to open process with ID"
- **Solution:** Run the injector as **administrator**
- Make sure Gorilla Tag is running

### "DLL file not found"
- **Solution:** Double-check the file path
- Make sure the DLL exists at that location
- Use the full absolute path (e.g., `C:\Users\...`)

### "Failed to create remote thread"
- **Solution:** The target process might be protected
- Try running the injector as administrator
- Close anti-virus software temporarily if it blocks the injection

### Injector closes immediately
- The program is waiting for you to press Enter
- Make sure you've entered the DLL path correctly
- Check the error messages in the console

### DLL loads but doesn't work
- The DLL may be missing dependencies (GLM, jsoncpp)
- Make sure all dependencies are properly linked
- Check if the game has compatible architecture (32-bit vs 64-bit)

---

## For Developers

### Using the Macro Menu in Your Code

Once injected, you can call exported functions from anywhere:

```cpp
#include <windows.h>

typedef void (*StartRecordingFunc)(const char*);
typedef void (*RecordMacroFrameFunc)(float, float, float, float, ...);
typedef void (*PlayMacroFunc)(const char*);

// Load the DLL (it's already injected)
HMODULE macroMenuDll = GetModuleHandle("GorillaTagMacroMenu.dll");

if (macroMenuDll) {
    // Get function pointers
    StartRecordingFunc startRecording = (StartRecordingFunc)GetProcAddress(macroMenuDll, "StartRecording");
    PlayMacroFunc playMacro = (PlayMacroFunc)GetProcAddress(macroMenuDll, "PlayMacro");
    
    // Use the functions
    startRecording("MyMacro");
    // ... record frames ...
    playMacro("MyMacro");
}
```

### Building a Custom Injector

You can modify `Injector.cpp` to:
- Auto-inject on game startup
- Create a GUI for easier usage
- Add logging and debugging
- Support multiple DLL injection

---

## Security Warning

⚠️ DLL injection can be flagged by anti-virus software. This is normal for game modding. 

**Be aware:**
- Only inject DLLs from trusted sources
- Injecting malicious DLLs can harm your system
- Some online games ban players for using injectors
- Use responsibly!

---

## Next Steps

1. ✅ Build the DLL: `cmake --build . --config Release`
2. ✅ Build the injector: `cmake --build build_injector --config Release`
3. ✅ Launch Gorilla Tag
4. ✅ Run injector and provide the DLL path
5. ✅ The macro menu is now active!

Enjoy your macro recording! 🎮
