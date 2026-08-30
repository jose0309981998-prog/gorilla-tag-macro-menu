cmake_minimum_required(VERSION 3.10)
project(GorillaTagMacroMenu)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add GLM include directory (header-only library)
find_package(glm REQUIRED)

# Find jsoncpp library
find_package(jsoncpp REQUIRED)

# Source files
set(SOURCES
    MacroMenu.cpp
    MacroMenuDLL.cpp
)

set(HEADERS
    MacroMenu.h
)

# Create DLL
add_library(GorillaTagMacroMenu SHARED ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(GorillaTagMacroMenu 
    PUBLIC
        glm::glm
        jsoncpp_lib
)

# Include directories
target_include_directories(GorillaTagMacroMenu 
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Platform-specific settings
if(MSVC)
    # Visual Studio specific settings
    target_compile_options(GorillaTagMacroMenu PRIVATE /W4)
    set_target_properties(GorillaTagMacroMenu PROPERTIES
        VS_PLATFORM_TOOLSET "v142"
    )
endif()

# Output directory
set_target_properties(GorillaTagMacroMenu PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
