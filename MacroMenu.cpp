#include "MacroMenu.h"
#include <fstream>
#include <sstream>
#include <json/json.h>
#include <glm/gtc/quaternion.hpp>
#include <iostream>

MacroMenu::MacroMenu() 
    : isRecording(false), isPlayingBack(false), playbackTime(0.0f) {
}

MacroMenu::~MacroMenu() {
}

// Recording functions
void MacroMenu::StartRecording(const std::string& macroName) {
    if (isRecording) {
        StopRecording();
    }
    
    currentRecordingName = macroName;
    isRecording = true;
    
    // Initialize new macro
    if (macros.find(macroName) == macros.end()) {
        macros[macroName] = Macro();
    } else {
        macros[macroName].frames.clear();
    }
    macros[macroName].name = macroName;
    
    std::cout << "Recording macro: " << macroName << std::endl;
}

void MacroMenu::StopRecording() {
    if (!isRecording) return;
    
    isRecording = false;
    
    // Calculate duration
    if (!macros[currentRecordingName].frames.empty()) {
        macros[currentRecordingName].duration = 
            macros[currentRecordingName].frames.back().timestamp;
    }
    
    std::cout << "Stopped recording macro: " << currentRecordingName << std::endl;
    std::cout << "Frames recorded: " << macros[currentRecordingName].frames.size() << std::endl;
}

void MacroMenu::RecordFrame(const MacroFrame& frame) {
    if (!isRecording) return;
    
    macros[currentRecordingName].frames.push_back(frame);
}

// Macro playback functions
void MacroMenu::PlayMacro(const std::string& macroName) {
    if (macros.find(macroName) == macros.end()) {
        std::cerr << "Macro not found: " << macroName << std::endl;
        return;
    }
    
    currentPlaybackMacro = macroName;
    isPlayingBack = true;
    playbackTime = 0.0f;
    std::cout << "Playing macro: " << macroName << std::endl;
}

void MacroMenu::StopPlayback() {
    isPlayingBack = false;
    playbackTime = 0.0f;
    std::cout << "Stopped playback" << std::endl;
}

void MacroMenu::UpdatePlayback(float deltaTime) {
    if (!isPlayingBack) return;
    
    Macro& currentMacro = macros[currentPlaybackMacro];
    playbackTime += deltaTime;
    
    // Check if we've reached the end
    if (playbackTime >= currentMacro.duration) {
        if (currentMacro.isLooping) {
            playbackTime = fmod(playbackTime, currentMacro.duration);
        } else {
            StopPlayback();
            return;
        }
    }
    
    // Find the two frames to interpolate between
    MacroFrame* frame1 = nullptr;
    MacroFrame* frame2 = nullptr;
    
    for (size_t i = 0; i < currentMacro.frames.size(); i++) {
        if (currentMacro.frames[i].timestamp <= playbackTime) {
            frame1 = &currentMacro.frames[i];
            if (i + 1 < currentMacro.frames.size()) {
                frame2 = &currentMacro.frames[i + 1];
            }
        }
    }
    
    if (frame1) {
        if (frame2) {
            // Interpolate between frames
            float t = (playbackTime - frame1->timestamp) / (frame2->timestamp - frame1->timestamp);
            MacroFrame interpolated = InterpolateFrames(*frame1, *frame2, t);
            ApplyFrameToRig(interpolated);
        } else {
            // Use the last frame
            ApplyFrameToRig(*frame1);
        }
    }
}

// Macro management
void MacroMenu::SaveMacro(const std::string& macroName, const std::string& filePath) {
    if (macros.find(macroName) == macros.end()) {
        std::cerr << "Macro not found: " << macroName << std::endl;
        return;
    }
    
    Macro& macro = macros[macroName];
    Json::Value root;
    root["name"] = macro.name;
    root["duration"] = macro.duration;
    root["isLooping"] = macro.isLooping;
    
    Json::Value framesArray(Json::arrayValue);
    for (const auto& frame : macro.frames) {
        Json::Value frameObj;
        frameObj["timestamp"] = frame.timestamp;
        
        // Head data
        frameObj["headPos"]["x"] = frame.headPosition.x;
        frameObj["headPos"]["y"] = frame.headPosition.y;
        frameObj["headPos"]["z"] = frame.headPosition.z;
        frameObj["headRot"]["x"] = frame.headRotation.x;
        frameObj["headRot"]["y"] = frame.headRotation.y;
        frameObj["headRot"]["z"] = frame.headRotation.z;
        frameObj["headRot"]["w"] = frame.headRotation.w;
        
        // Left hand data
        frameObj["leftHandPos"]["x"] = frame.leftHandPosition.x;
        frameObj["leftHandPos"]["y"] = frame.leftHandPosition.y;
        frameObj["leftHandPos"]["z"] = frame.leftHandPosition.z;
        frameObj["leftHandRot"]["x"] = frame.leftHandRotation.x;
        frameObj["leftHandRot"]["y"] = frame.leftHandRotation.y;
        frameObj["leftHandRot"]["z"] = frame.leftHandRotation.z;
        frameObj["leftHandRot"]["w"] = frame.leftHandRotation.w;
        
        // Right hand data
        frameObj["rightHandPos"]["x"] = frame.rightHandPosition.x;
        frameObj["rightHandPos"]["y"] = frame.rightHandPosition.y;
        frameObj["rightHandPos"]["z"] = frame.rightHandPosition.z;
        frameObj["rightHandRot"]["x"] = frame.rightHandRotation.x;
        frameObj["rightHandRot"]["y"] = frame.rightHandRotation.y;
        frameObj["rightHandRot"]["z"] = frame.rightHandRotation.z;
        frameObj["rightHandRot"]["w"] = frame.rightHandRotation.w;
        
        // Body data
        frameObj["bodyPos"]["x"] = frame.bodyPosition.x;
        frameObj["bodyPos"]["y"] = frame.bodyPosition.y;
        frameObj["bodyPos"]["z"] = frame.bodyPosition.z;
        frameObj["bodyRot"]["x"] = frame.bodyRotation.x;
        frameObj["bodyRot"]["y"] = frame.bodyRotation.y;
        frameObj["bodyRot"]["z"] = frame.bodyRotation.z;
        frameObj["bodyRot"]["w"] = frame.bodyRotation.w;
        
        framesArray.append(frameObj);
    }
    root["frames"] = framesArray;
    
    std::ofstream file(filePath);
    file << root.toStyledString();
    file.close();
    
    std::cout << "Macro saved to: " << filePath << std::endl;
}

void MacroMenu::LoadMacro(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open macro file: " << filePath << std::endl;
        return;
    }
    
    Json::Value root;
    file >> root;
    file.close();
    
    Macro macro;
    macro.name = root["name"].asString();
    macro.duration = root["duration"].asFloat();
    macro.isLooping = root["isLooping"].asBool();
    
    for (const auto& frameObj : root["frames"]) {
        MacroFrame frame;
        frame.timestamp = frameObj["timestamp"].asFloat();
        
        // Head
        frame.headPosition = glm::vec3(
            frameObj["headPos"]["x"].asFloat(),
            frameObj["headPos"]["y"].asFloat(),
            frameObj["headPos"]["z"].asFloat()
        );
        frame.headRotation = glm::quat(
            frameObj["headRot"]["w"].asFloat(),
            frameObj["headRot"]["x"].asFloat(),
            frameObj["headRot"]["y"].asFloat(),
            frameObj["headRot"]["z"].asFloat()
        );
        
        // Left hand
        frame.leftHandPosition = glm::vec3(
            frameObj["leftHandPos"]["x"].asFloat(),
            frameObj["leftHandPos"]["y"].asFloat(),
            frameObj["leftHandPos"]["z"].asFloat()
        );
        frame.leftHandRotation = glm::quat(
            frameObj["leftHandRot"]["w"].asFloat(),
            frameObj["leftHandRot"]["x"].asFloat(),
            frameObj["leftHandRot"]["y"].asFloat(),
            frameObj["leftHandRot"]["z"].asFloat()
        );
        
        // Right hand
        frame.rightHandPosition = glm::vec3(
            frameObj["rightHandPos"]["x"].asFloat(),
            frameObj["rightHandPos"]["y"].asFloat(),
            frameObj["rightHandPos"]["z"].asFloat()
        );
        frame.rightHandRotation = glm::quat(
            frameObj["rightHandRot"]["w"].asFloat(),
            frameObj["rightHandRot"]["x"].asFloat(),
            frameObj["rightHandRot"]["y"].asFloat(),
            frameObj["rightHandRot"]["z"].asFloat()
        );
        
        // Body
        frame.bodyPosition = glm::vec3(
            frameObj["bodyPos"]["x"].asFloat(),
            frameObj["bodyPos"]["y"].asFloat(),
            frameObj["bodyPos"]["z"].asFloat()
        );
        frame.bodyRotation = glm::quat(
            frameObj["bodyRot"]["w"].asFloat(),
            frameObj["bodyRot"]["x"].asFloat(),
            frameObj["bodyRot"]["y"].asFloat(),
            frameObj["bodyRot"]["z"].asFloat()
        );
        
        macro.frames.push_back(frame);
    }
    
    macros[macro.name] = macro;
    std::cout << "Macro loaded: " << macro.name << std::endl;
}

void MacroMenu::DeleteMacro(const std::string& macroName) {
    if (macros.find(macroName) != macros.end()) {
        macros.erase(macroName);
        std::cout << "Macro deleted: " << macroName << std::endl;
    } else {
        std::cerr << "Macro not found: " << macroName << std::endl;
    }
}

void MacroMenu::ListMacros() const {
    std::cout << "Available Macros:" << std::endl;
    for (const auto& pair : macros) {
        std::cout << "  - " << pair.first << " (Duration: " << pair.second.duration << "s, Frames: " 
                  << pair.second.frames.size() << ")" << std::endl;
    }
}

Macro* MacroMenu::GetMacro(const std::string& macroName) {
    if (macros.find(macroName) != macros.end()) {
        return &macros[macroName];
    }
    return nullptr;
}

// Rig following functions
void MacroMenu::ApplyFrameToRig(const MacroFrame& frame) {
    // This function would interface with the Gorilla Tag rig system
    // to apply the recorded pose data to the player character
    // Implementation depends on the game's scripting API
}

MacroFrame MacroMenu::InterpolateFrames(const MacroFrame& frame1, const MacroFrame& frame2, float t) {
    MacroFrame result;
    result.timestamp = frame1.timestamp + (frame2.timestamp - frame1.timestamp) * t;
    
    // Linear interpolation for positions
    result.headPosition = glm::mix(frame1.headPosition, frame2.headPosition, t);
    result.leftHandPosition = glm::mix(frame1.leftHandPosition, frame2.leftHandPosition, t);
    result.rightHandPosition = glm::mix(frame1.rightHandPosition, frame2.rightHandPosition, t);
    result.bodyPosition = glm::mix(frame1.bodyPosition, frame2.bodyPosition, t);
    
    // Spherical linear interpolation for rotations
    result.headRotation = glm::slerp(frame1.headRotation, frame2.headRotation, t);
    result.leftHandRotation = glm::slerp(frame1.leftHandRotation, frame2.leftHandRotation, t);
    result.rightHandRotation = glm::slerp(frame1.rightHandRotation, frame2.rightHandRotation, t);
    result.bodyRotation = glm::slerp(frame1.bodyRotation, frame2.bodyRotation, t);
    
    return result;
}

// Menu UI functions
void MacroMenu::Render() {
    // UI rendering implementation
}

void MacroMenu::HandleInput() {
    // Input handling implementation
}

// Export/Import functions
void MacroMenu::ExportMacroLibrary(const std::string& filePath) {
    Json::Value root;
    Json::Value macrosArray(Json::arrayValue);
    
    for (const auto& pair : macros) {
        Json::Value macroObj;
        macroObj["name"] = pair.second.name;
        macroObj["duration"] = pair.second.duration;
        macroObj["isLooping"] = pair.second.isLooping;
        
        Json::Value framesArray(Json::arrayValue);
        for (const auto& frame : pair.second.frames) {
            Json::Value frameObj;
            frameObj["timestamp"] = frame.timestamp;
            frameObj["headPos"]["x"] = frame.headPosition.x;
            frameObj["headPos"]["y"] = frame.headPosition.y;
            frameObj["headPos"]["z"] = frame.headPosition.z;
            frameObj["headRot"]["x"] = frame.headRotation.x;
            frameObj["headRot"]["y"] = frame.headRotation.y;
            frameObj["headRot"]["z"] = frame.headRotation.z;
            frameObj["headRot"]["w"] = frame.headRotation.w;
            
            frameObj["leftHandPos"]["x"] = frame.leftHandPosition.x;
            frameObj["leftHandPos"]["y"] = frame.leftHandPosition.y;
            frameObj["leftHandPos"]["z"] = frame.leftHandPosition.z;
            frameObj["leftHandRot"]["x"] = frame.leftHandRotation.x;
            frameObj["leftHandRot"]["y"] = frame.leftHandRotation.y;
            frameObj["leftHandRot"]["z"] = frame.leftHandRotation.z;
            frameObj["leftHandRot"]["w"] = frame.leftHandRotation.w;
            
            frameObj["rightHandPos"]["x"] = frame.rightHandPosition.x;
            frameObj["rightHandPos"]["y"] = frame.rightHandPosition.y;
            frameObj["rightHandPos"]["z"] = frame.rightHandPosition.z;
            frameObj["rightHandRot"]["x"] = frame.rightHandRotation.x;
            frameObj["rightHandRot"]["y"] = frame.rightHandRotation.y;
            frameObj["rightHandRot"]["z"] = frame.rightHandRotation.z;
            frameObj["rightHandRot"]["w"] = frame.rightHandRotation.w;
            
            frameObj["bodyPos"]["x"] = frame.bodyPosition.x;
            frameObj["bodyPos"]["y"] = frame.bodyPosition.y;
            frameObj["bodyPos"]["z"] = frame.bodyPosition.z;
            frameObj["bodyRot"]["x"] = frame.bodyRotation.x;
            frameObj["bodyRot"]["y"] = frame.bodyRotation.y;
            frameObj["bodyRot"]["z"] = frame.bodyRotation.z;
            frameObj["bodyRot"]["w"] = frame.bodyRotation.w;
            
            framesArray.append(frameObj);
        }
        macroObj["frames"] = framesArray;
        macrosArray.append(macroObj);
    }
    root["macros"] = macrosArray;
    
    std::ofstream file(filePath);
    file << root.toStyledString();
    file.close();
    
    std::cout << "Macro library exported to: " << filePath << std::endl;
}

void MacroMenu::ImportMacroLibrary(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open macro library file: " << filePath << std::endl;
        return;
    }
    
    Json::Value root;
    file >> root;
    file.close();
    
    for (const auto& macroObj : root["macros"]) {
        Macro macro;
        macro.name = macroObj["name"].asString();
        macro.duration = macroObj["duration"].asFloat();
        macro.isLooping = macroObj["isLooping"].asBool();
        
        for (const auto& frameObj : macroObj["frames"]) {
            MacroFrame frame;
            frame.timestamp = frameObj["timestamp"].asFloat();
            
            frame.headPosition = glm::vec3(
                frameObj["headPos"]["x"].asFloat(),
                frameObj["headPos"]["y"].asFloat(),
                frameObj["headPos"]["z"].asFloat()
            );
            frame.headRotation = glm::quat(
                frameObj["headRot"]["w"].asFloat(),
                frameObj["headRot"]["x"].asFloat(),
                frameObj["headRot"]["y"].asFloat(),
                frameObj["headRot"]["z"].asFloat()
            );
            
            frame.leftHandPosition = glm::vec3(
                frameObj["leftHandPos"]["x"].asFloat(),
                frameObj["leftHandPos"]["y"].asFloat(),
                frameObj["leftHandPos"]["z"].asFloat()
            );
            frame.leftHandRotation = glm::quat(
                frameObj["leftHandRot"]["w"].asFloat(),
                frameObj["leftHandRot"]["x"].asFloat(),
                frameObj["leftHandRot"]["y"].asFloat(),
                frameObj["leftHandRot"]["z"].asFloat()
            );
            
            frame.rightHandPosition = glm::vec3(
                frameObj["rightHandPos"]["x"].asFloat(),
                frameObj["rightHandPos"]["y"].asFloat(),
                frameObj["rightHandPos"]["z"].asFloat()
            );
            frame.rightHandRotation = glm::quat(
                frameObj["rightHandRot"]["w"].asFloat(),
                frameObj["rightHandRot"]["x"].asFloat(),
                frameObj["rightHandRot"]["y"].asFloat(),
                frameObj["rightHandRot"]["z"].asFloat()
            );
            
            frame.bodyPosition = glm::vec3(
                frameObj["bodyPos"]["x"].asFloat(),
                frameObj["bodyPos"]["y"].asFloat(),
                frameObj["bodyPos"]["z"].asFloat()
            );
            frame.bodyRotation = glm::quat(
                frameObj["bodyRot"]["w"].asFloat(),
                frameObj["bodyRot"]["x"].asFloat(),
                frameObj["bodyRot"]["y"].asFloat(),
                frameObj["bodyRot"]["z"].asFloat()
            );
            
            macro.frames.push_back(frame);
        }
        
        macros[macro.name] = macro;
        std::cout << "Macro imported: " << macro.name << std::endl;
    }
}
