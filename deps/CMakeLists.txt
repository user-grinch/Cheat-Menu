################################################################################
# Build Depend.lib
# All the external libraries that shouldn't need to be compiled often
################################################################################
cmake_minimum_required(VERSION 3.0)
project(Depend)

set(depend_files
    "fla/Main.h"
    "fla/Main.cpp"
    "fla/IDaccess.h"
    "fla/IDaccess.cpp"
    "fla/Library/Library.h"
    "fla/Library/Library.cpp"
    "imgui/imconfig.h"
    "imgui/imgui.cpp"
    "imgui/imgui.h"
    "imgui/imgui_demo.cpp"
    "imgui/imgui_draw.cpp"
    "imgui/imgui_impl_dx9.cpp"
    "imgui/imgui_impl_dx9.h"
    "imgui/imgui_impl_dx11.cpp"
    "imgui/imgui_impl_dx11.h"
    # "imgui/imgui_impl_vulkan.cpp"
    # "imgui/imgui_impl_vulkan.h"
    "imgui/imgui_impl_win32.cpp"
    "imgui/imgui_impl_win32.h"
    "imgui/imgui_internal.h"
    "imgui/imgui_widgets.cpp"
    "imgui/imgui_tables.cpp"
    "imgui/imstb_rectpack.h"
    "imgui/imstb_textedit.h"
    "imgui/imstb_truetype.h"
    "imgui/stb_image.h"
    "json.hpp"
    "kiero/kiero.cpp"
    "kiero/kiero.h"
    "kiero/minhook/MinHook.h"
    "kiero/minhook/buffer.c"
    "kiero/minhook/hook.c"
    "kiero/minhook/trampoline.c"
    "kiero/minhook/hde/hde32.c"
    "kiero/minhook/trampoline.c"
)

add_library(${PROJECT_NAME} STATIC ${depend_files})

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
    "_DX9_SDK_INSTALLED;"
    "PLUGIN_SGV_10US;"
    "_MBCS"
    "IS_PLATFORM_WIN"
)

include_directories(
    "$ENV{PLUGIN_SDK_DIR}/plugin_sa"
    "$ENV{PLUGIN_SDK_DIR}/plugin_sa/game_sa"
    "$ENV{PLUGIN_SDK_DIR}/shared"
    "$ENV{PLUGIN_SDK_DIR}/shared/game"
    "$ENV{DIRECTX9_SDK_DIR}/include"
    # "${VULKAN_SDK_DIR}/Include"
    # "${VULKAN_SDK_DIR}/lib32"
)

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

string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
    $<$<CONFIG:Release>:
        MultiThreaded
    >
    $<$<CONFIG:Debug>:
        MultiThreadedDebug
    >
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    MSVC_RUNTIME_LIBRARY ${MSVC_RUNTIME_LIBRARY_STR}
)

target_link_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}"/deps/kiero/minhook/lib/"
)

target_link_libraries(${PROJECT_NAME} PUBLIC 
    optimized plugin
    debug plugin_d
    d3d9
    d3dx9
    d3d11
    d3dx11
    XInput9_1_0
)