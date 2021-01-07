################################################################################
# Build Test.asi
# A testing plugin for the menu, recompiling the whole menu is tedious  
################################################################################

cmake_minimum_required(VERSION 3.0)
project(Test)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} SHARED "Test.cpp")

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
)

################################################################################
# Compile and link options
################################################################################
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
    /std:c++latest;
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
)

target_link_directories(${PROJECT_NAME} PUBLIC
"$ENV{PLUGIN_SDK_DIR}/output/lib/"
"$<$<CONFIG:Release>:"
    "vendor/Release/"
">"
"$<$<CONFIG:Debug>:"
    "vendor/Debug/"
">")

target_link_libraries(${PROJECT_NAME} PUBLIC 
    optimized plugin
    debug plugin_d
)