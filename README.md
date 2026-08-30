# Gorilla Tag Macro Menu DLL

A powerful C++ DLL for Gorilla Tag that enables macro recording, playback, and management. Record player movements and poses, then replay them with full rig animation support.

## Features

✅ **Macro Recording** - Record player head, hand, and body movements frame-by-frame
✅ **Macro Playback** - Replay recorded macros with smooth interpolation
✅ **Rig Following** - Automatically apply recorded poses to the player rig
✅ **Import/Export** - Save and load individual macros or entire macro libraries
✅ **Looping Support** - Set macros to loop continuously during playback
✅ **Frame Interpolation** - Smooth SLERP rotation and linear position interpolation
✅ **JSON Storage** - Human-readable macro format for easy sharing and editing

## Building

### Prerequisites

- Visual Studio 2019 or newer
- CMake 3.10+
- GLM (header-only math library)
- jsoncpp

### Build Instructions

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

The compiled DLL will be in `build/bin/GorillaTagMacroMenu.dll`

## Usage

### C++ Integration

```cpp
#include "MacroMenu.h"

MacroMenu menu;

// Start recording
menu.StartRecording("MyMacro");

// Record frames (call this in your game loop)
MacroFrame frame;
frame.timestamp = currentTime;
frame.headPosition = playerHeadPos;
frame.headRotation = playerHeadRot;
// ... set other positions and rotations
menu.RecordFrame(frame);

// Stop recording
menu.StopRecording();

// Save the macro
menu.SaveMacro("MyMacro", "macros/MyMacro.json");

// Load a macro
menu.LoadMacro("macros/MyMacro.json");

// Play the macro
menu.PlayMacro("MyMacro");

// Update playback in game loop
menu.UpdatePlayback(deltaTime);

// Stop playback
menu.StopPlayback();
```

### DLL Export Functions

The DLL exports the following functions for external use:

```cpp
// Get the global macro menu instance
MacroMenu* GetMacroMenuInstance();

// Recording
void StartRecording(const char* macroName);
void StopRecording();
void RecordMacroFrame(float timestamp, 
                      float hx, float hy, float hz,
                      float hrx, float hry, float hrz, float hrw,
                      float lhx, float lhy, float lhz,
                      float lhrx, float lhry, float lhrz, float lhrw,
                      float rhx, float rhy, float rhz,
                      float rhrx, float rhry, float rhrz, float rhrw,
                      float bx, float by, float bz,
                      float brx, float bry, float brz, float brw);
bool IsRecording();

// Playback
void PlayMacro(const char* macroName);
void StopMacroPlayback();
void UpdateMacroPlayback(float deltaTime);
bool IsPlayingBack();

// File Management
void SaveMacroToFile(const char* macroName, const char* filePath);
void LoadMacroFromFile(const char* filePath);
void DeleteMacro(const char* macroName);
void ListAllMacros();

// Library Management
void ExportMacroLibrary(const char* filePath);
void ImportMacroLibrary(const char* filePath);

// Settings
void SetMacroLooping(const char* macroName, bool shouldLoop);
```

## Macro File Format

Macros are stored as JSON files for easy sharing and editing:

```json
{
   "name" : "MyMacro",
   "duration" : 10.5,
   "isLooping" : false,
   "frames" : [
      {
         "timestamp" : 0.0,
         "headPos" : { "x" : 0.0, "y" : 1.7, "z" : 0.0 },
         "headRot" : { "x" : 0.0, "y" : 0.0, "z" : 0.0, "w" : 1.0 },
         "leftHandPos" : { "x" : -0.3, "y" : 1.2, "z" : 0.5 },
         "leftHandRot" : { "x" : 0.0, "y" : 0.0, "z" : 0.0, "w" : 1.0 },
         "rightHandPos" : { "x" : 0.3, "y" : 1.2, "z" : 0.5 },
         "rightHandRot" : { "x" : 0.0, "y" : 0.0, "z" : 0.0, "w" : 1.0 },
         "bodyPos" : { "x" : 0.0, "y" : 0.0, "z" : 0.0 },
         "bodyRot" : { "x" : 0.0, "y" : 0.0, "z" : 0.0, "w" : 1.0 }
      }
   ]
}
```

## API Reference

### MacroFrame Structure

```cpp
struct MacroFrame {
    float timestamp;                  // Time in seconds
    glm::vec3 headPosition;          // Head XYZ position
    glm::quat headRotation;          // Head quaternion rotation
    glm::vec3 leftHandPosition;      // Left hand XYZ position
    glm::quat leftHandRotation;      // Left hand quaternion rotation
    glm::vec3 rightHandPosition;     // Right hand XYZ position
    glm::quat rightHandRotation;     // Right hand quaternion rotation
    glm::vec3 bodyPosition;          // Body XYZ position
    glm::quat bodyRotation;          // Body quaternion rotation
};
```

### Macro Structure

```cpp
struct Macro {
    std::string name;                       // Macro name
    std::vector<MacroFrame> frames;         // Recorded frames
    float duration;                         // Total duration in seconds
    bool isLooping;                         // Whether macro loops on playback
};
```

## Recording Tips

1. **Frame Rate**: Record at a consistent frame rate for smooth playback (60 FPS recommended)
2. **Duration**: Keep macros under 30 seconds for best performance
3. **Precision**: Use high-precision position and rotation data for smooth interpolation
4. **Testing**: Test playback immediately after recording to ensure quality

## Interpolation

The system uses:
- **Linear interpolation** for position vectors
- **Spherical linear interpolation (SLERP)** for rotations

This ensures smooth, natural-looking movement between recorded frames.

## Performance

- **Memory**: ~500 bytes per frame (roughly 30KB per second of recording)
- **CPU**: Minimal overhead, primarily during playback updates
- **Supported Macros**: Unlimited (limited by available disk space)

## Troubleshooting

### DLL Not Loading
- Ensure Visual C++ Runtime is installed
- Check that all dependencies (GLM, jsoncpp) are properly linked
- Verify the DLL is 64-bit or 32-bit matching your application

### Jerky Playback
- Increase frame recording rate
- Ensure `UpdatePlayback()` is called every frame
- Check that delta time is accurate

### Lost Macros
- Verify save/load file paths are correct
- Check file permissions
- Ensure JSON format is valid

## License

Open source - feel free to modify and distribute

## Contributing

Pull requests welcome! Please ensure:
- Code follows existing style
- CMake builds without errors
- All exported functions are documented

## Support

For issues or questions, open an issue on GitHub or check the documentation above.
