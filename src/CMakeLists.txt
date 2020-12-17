################################################################################
# Source groups
################################################################################
set(no_group_source_files
    "Animation.cpp"
    "Animation.h"
    "CheatMenu.cpp"
    "CheatMenu.h"
    "CustomWidgets.cpp"
    "CustomWidgets.h"
    "Events.cpp"
    "Events.h"
    "external/imgui/imgui_impl_dx11.cpp"
    "external/imgui/imgui_impl_dx11.h"
    "external/imgui/stb_image.h"
    "external/json.hpp"
    "external/kiero/kiero.cpp"
    "external/kiero/kiero.h"
    "external/kiero/minhook/include/MinHook.h"
    "external/moon/pool_object_extender.h"
    "external/moon/texture_manager.cpp"
    "external/moon/texture_manager.h"
    "external/moon/vehicle_renderer.cpp"
    "external/moon/vehicle_renderer.h"
    "Game.cpp"
    "Game.h"
    "Hook.cpp"
    "Hook.h"
    "Json.cpp"
    "Json.h"
    "Menu.cpp"
    "Menu.h"
    "NeonAPI.cpp"
    "NeonAPI.h"
    "Paint.cpp"
    "Paint.h"
    "pch.cpp"
    "pch.h"
    "Ped.cpp"
    "Ped.h"
    "Player.cpp"
    "Player.h"
    "Teleport.cpp"
    "Teleport.h"
    "Ui.cpp"
    "Ui.h"
    "Util.cpp"
    "Util.h"
    "Vehicle.cpp"
    "Vehicle.h"
    "Visual.cpp"
    "Visual.h"
    "VKeys.h"
    "Weapon.cpp"
    "Weapon.h"
)
source_group("" FILES ${no_group_source_files})

set(external
    "external/imgui/imconfig.h"
    "external/imgui/imgui.cpp"
    "external/imgui/imgui.h"
    "external/imgui/imgui_demo.cpp"
    "external/imgui/imgui_draw.cpp"
    "external/imgui/imgui_impl_dx9.cpp"
    "external/imgui/imgui_impl_dx9.h"
    "external/imgui/imgui_impl_win32.cpp"
    "external/imgui/imgui_impl_win32.h"
    "external/imgui/imgui_internal.h"
    "external/imgui/imgui_widgets.cpp"
    "external/imgui/imstb_rectpack.h"
    "external/imgui/imstb_textedit.h"
    "external/imgui/imstb_truetype.h"
)
source_group("external" FILES ${external})

set(ALL_FILES
    ${no_group_source_files}
    ${external}
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} SHARED ${ALL_FILES})

add_precompiled_header(${PROJECT_NAME} "pch.h" "pch.cpp")

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
    "${DX9_DIR}/include"
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
if(MSVC)
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
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(external/imgui/imgui_impl_dx11.cpp ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(external/kiero/kiero.cpp ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(external/moon/texture_manager.cpp ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(external/moon/vehicle_renderer.cpp ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(external/imgui/imgui.cpp ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(external/imgui/imgui_demo.cpp ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(external/imgui/imgui_draw.cpp ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(external/imgui/imgui_impl_dx9.cpp ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(external/imgui/imgui_impl_win32.cpp ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(external/imgui/imgui_widgets.cpp ${FILE_CL_OPTIONS})
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:
            /SAFESEH:NO;
            /OPT:REF;
            /LTCG;
            /OPT:ICF
        >
        $<$<CONFIG:Debug>:
            /DEBUG:FULL;
        >
        /SUBSYSTEM:WINDOWS
    )
endif()

################################################################################
# Pre build events
################################################################################
add_custom_command_if(
    TARGET ${PROJECT_NAME}
    PRE_BUILD
    COMMANDS
    COMMAND $<CONFIG:Release> taskkill /f /fi "imagename eq gta_sa.exe"
    COMMAND  $<CONFIG:Debug> taskkill /f /fi "imagename eq gta_sa.exe"
)

################################################################################
# Dependencies
################################################################################
target_link_libraries(${PROJECT_NAME} PUBLIC 
optimized plugin
optimized libMinHook-x86-v140-mt
optimized libMinHook-x86-v140-md
debug plugin_d
debug libMinHook-x86-v140-mtd
debug libMinHook-x86-v140-mdd
d3d9
d3dx9
d3d11
d3dx11
XInput9_1_0
)

target_link_directories(${PROJECT_NAME} PUBLIC
    "${PLUGIN_SDK_DIR}/output/lib/"
    "$ENV{DIRECTX9_SDK_DIR}/lib/x86/"
    "external/kiero/minhook/lib/"
)

