#pragma once

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <glm/glm.hpp>

// Macro frame structure to store rig pose data
struct MacroFrame {
    float timestamp;
    glm::vec3 headPosition;
    glm::quat headRotation;
    glm::vec3 leftHandPosition;
    glm::quat leftHandRotation;
    glm::vec3 rightHandPosition;
    glm::quat rightHandRotation;
    glm::vec3 bodyPosition;
    glm::quat bodyRotation;
};

// Macro structure containing recorded frames
struct Macro {
    std::string name;
    std::vector<MacroFrame> frames;
    float duration;
    bool isLooping;
    
    Macro() : duration(0.0f), isLooping(false) {}
};

// Main macro menu class
class MacroMenu {
public:
    MacroMenu();
    ~MacroMenu();

    // Recording functions
    void StartRecording(const std::string& macroName);
    void StopRecording();
    void RecordFrame(const MacroFrame& frame);
    bool IsRecording() const { return isRecording; }

    // Macro playback functions
    void PlayMacro(const std::string& macroName);
    void StopPlayback();
    void UpdatePlayback(float deltaTime);
    bool IsPlayingBack() const { return isPlayingBack; }

    // Macro management
    void SaveMacro(const std::string& macroName, const std::string& filePath);
    void LoadMacro(const std::string& filePath);
    void DeleteMacro(const std::string& macroName);
    void ListMacros() const;
    Macro* GetMacro(const std::string& macroName);

    // Rig following functions
    void ApplyFrameToRig(const MacroFrame& frame);
    MacroFrame InterpolateFrames(const MacroFrame& frame1, const MacroFrame& frame2, float t);

    // Menu UI functions
    void Render();
    void HandleInput();

    // Export/Import functions
    void ExportMacroLibrary(const std::string& filePath);
    void ImportMacroLibrary(const std::string& filePath);

private:
    std::map<std::string, Macro> macros;
    std::string currentRecordingName;
    bool isRecording;
    bool isPlayingBack;
    std::string currentPlaybackMacro;
    float playbackTime;
    
    MacroFrame RecordingBuffer;
};
