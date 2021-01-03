################################################################################
# Build Vendor.lib
# All the external libraries that shouldn't need to be compiled often
################################################################################
cmake_minimum_required(VERSION 3.0)
project(Vendor)

set(vendor_files
    "imgui/imconfig.h"
    "imgui/imgui.cpp"
    "imgui/imgui.h"
    "imgui/imgui_demo.cpp"
    "imgui/imgui_draw.cpp"
    "imgui/imgui_impl_dx9.cpp"
    "imgui/imgui_impl_dx9.h"
    "imgui/imgui_impl_dx11.cpp"
    "imgui/imgui_impl_dx11.h"
    "imgui/imgui_impl_win32.cpp"
    "imgui/imgui_impl_win32.h"
    "imgui/imgui_internal.h"
    "imgui/imgui_widgets.cpp"
    "imgui/imstb_rectpack.h"
    "imgui/imstb_textedit.h"
    "imgui/imstb_truetype.h"
    "imgui/stb_image.h"
    "json.hpp"
    "kiero/kiero.cpp"
    "kiero/kiero.h"
    "kiero/minhook/include/MinHook.h"
    "moon/pool_object_extender.h"
    "moon/vehicle_renderer.h"
    "moon/vehicle_renderer.cpp"
)

add_library(${PROJECT_NAME} STATIC ${vendor_files})

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

include_directories(
    "$ENV{PLUGIN_SDK_DIR}/plugin_sa"
    "$ENV{PLUGIN_SDK_DIR}/plugin_sa/game_sa"
    "$ENV{PLUGIN_SDK_DIR}/shared"
    "$ENV{PLUGIN_SDK_DIR}/shared/game"
    "$ENV{DIRECTX9_SDK_DIR}/include"
)

target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Release>:
        /O2;
        /Oi;
        /Gy
    >
    $<$<CONFIG:Debug>:
        /Od
    >
    /std:c++latest;
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
    ${CMAKE_SOURCE_DIR}"/src/vendor/kiero/minhook/lib/"
)

target_link_libraries(${PROJECT_NAME} PUBLIC 
    optimized plugin
    debug plugin_d
    d3d9
    d3dx9
    d3d11
    d3dx11
    XInput9_1_0
    optimized libMinHook-x86-v140-mt
    optimized libMinHook-x86-v140-md
    debug libMinHook-x86-v140-mtd
    debug libMinHook-x86-v140-mdd
)