cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
set(PROJECT_NAME CheatMenu)
project(${PROJECT_NAME} CXX)

################################################################################
# Configure paths
################################################################################
set(GTA_SA_DIR F:/GTASanAndreas)

# Can ignore the below paths if you got them in system paths
set(PLUGIN_SDK_DIR $ENV{PLUGIN_SDK_DIR})
set(DIRECTX9_SDK_DIR $ENV{DIRECTX9_SDK_DIR})
################################################################################

################################################################################
# Required projects
################################################################################
add_subdirectory(deps)
add_subdirectory(tests)

################################################################################
# Source groups
################################################################################
set(src_files
    "src/Animation.cpp"
    "src/Animation.h"
    "src/CheatMenu.cpp"
    "src/CheatMenu.h"
    "src/Events.cpp"
    "src/Events.h"
    "src/Game.cpp"
    "src/Game.h"
    "src/Hook.cpp"
    "src/Hook.h"
    "src/Json.cpp"
    "src/Json.h"
    "src/Menu.cpp"
    "src/Menu.h"
    "src/MenuInfo.h"
    "src/Neon.cpp"
    "src/Neon.h"
    "src/Paint.cpp"
    "src/Paint.h"
    "src/pch.cpp"
    "src/pch.h"
    "src/Ped.cpp"
    "src/Ped.h"
    "src/Player.cpp"
    "src/Player.h"
    "src/Teleport.cpp"
    "src/Teleport.h"
    "src/Ui.cpp"
    "src/Ui.h"
    "src/Util.cpp"
    "src/Util.h"
    "src/Vehicle.cpp"
    "src/Vehicle.h"
    "src/Visual.cpp"
    "src/Visual.h"
    "src/VKeys.h"
    "src/Weapon.cpp"
    "src/Weapon.h"
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} SHARED ${src_files})

target_precompile_headers(${PROJECT_NAME} PUBLIC "src/pch.h")

string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
    $<$<CONFIG:Release>:
        MultiThreaded
    >
    $<$<CONFIG:Debug>:
        MultiThreadedDebug
    >
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY  "${GTA_SA_DIR}/$<0:>/"
    SUFFIX ".asi"
    MSVC_RUNTIME_LIBRARY ${MSVC_RUNTIME_LIBRARY_STR}
)

################################################################################
# Include directories
################################################################################
include_directories(
    "${PLUGIN_SDK_DIR}/plugin_sa"
    "${PLUGIN_SDK_DIR}/plugin_sa/game_sa"
    "${PLUGIN_SDK_DIR}/shared"
    "${PLUGIN_SDK_DIR}/shared/game"
    "${DIRECTX9_SDK_DIR}/include"
    "deps"
)

################################################################################
# Compile definitions
################################################################################
target_compile_definitions(${PROJECT_NAME} PRIVATE
    "$<$<CONFIG:Release>:"
        "_NDEBUG"
    ">"
    "$<$<CONFIG:Debug>:"
        "_DEBUG"
    ">"
    "_CRT_SECURE_NO_WARNINGS;"
    "_CRT_NON_CONFORMING_SWPRINTFS;"
    "GTASA;"
    "GTAGAME_NAME=\"San Andreas\";"
    "GTAGAME_ABBR=\"SA\";"
    "GTAGAME_ABBRLOW=\"sa\";"
    "GTAGAME_PROTAGONISTNAME=\"CJ\";"
    "GTAGAME_CITYNAME=\"San Andreas\";"
    "_LA_SUPPORT;"
    "_DX9_SDK_INSTALLED;"
    "PLUGIN_SGV_10US;"
    "_MBCS"
    "IS_PLATFORM_WIN"
)

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:
            /O2;
            /Oi;
            /Gy
        >
        $<$<CONFIG:Debug>:
            /Od
            /DEBUG:FULL
        >
        $<$<COMPILE_LANGUAGE:CXX>:/std:c++latest>
        /sdl-;
        /W3;
        ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
        ${DEFAULT_CXX_EXCEPTION_HANDLING}
        /w44005
    )
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:
            /OPT:REF;
            /LTCG;
            /OPT:ICF;
        >
        $<$<CONFIG:Debug>:
            /DEBUG:FULL;
            /SAFESEH:NO;
        >
        /SUBSYSTEM:WINDOWS
    )
endif()

################################################################################
# Pre build events
################################################################################
add_custom_command(
    TARGET ${PROJECT_NAME} 
    PRE_BUILD
    COMMAND  taskkill /f /fi "imagename eq gta_sa.exe"
)

################################################################################
# Dependencies
################################################################################
target_link_libraries(${PROJECT_NAME} PUBLIC 
optimized plugin
debug plugin_d
d3d9
d3dx9
d3d11
d3dx11
XInput9_1_0
Depend
)

target_link_directories(${PROJECT_NAME} PUBLIC
    "${PLUGIN_SDK_DIR}/output/lib/"
    "${DIRECTX9_SDK_DIR}/lib/x86/"
    "$<$<CONFIG:Release>:"
        "deps/Release/"
    ">"
    "$<$<CONFIG:Debug>:"
        "deps/Debug/"
    ">"
)



