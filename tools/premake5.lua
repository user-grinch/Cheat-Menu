
----------------------------
-- Premake Project Generator
----------------------------

-- Environment vars
----------------------------
-- Should get picked up automatically if you installed them properly
PSDK_DIR = os.getenv("PLUGIN_SDK_DIR")
DX9SDK_DIR = os.getenv("DIRECTX9_SDK_DIR")

if (DX9SDK_DIR == nil) then
    error("DIRECTX9_SDK_DIR environment variable not set")
end

if (PSDK_DIR == nil) then
    error("PLUGIN_SDK_DIR environment variable not set")
end

-- Build directories
----------------------------
GTASA_DIR = "F:/GTASanAndreas"
GTAVC_DIR = "E:/GTA Vice City"
GTA3_DIR = "E:/GTA3"

----------------------------

workspace "CheatMenu"
    configurations { "Debug", "Release" }
    architecture "x86"
    platforms "Win32"
    language "C++"
    cppdialect "C++20"
    characterset "MBCS"
    staticruntime "On"
    location "../build"

project "Depend"
    kind "StaticLib"
    targetdir "../build/bin"

    files { 
        "../depned/**.h", 
        "../depend/**.hpp", 
        "../depend/**.c", 
        "../depend/**.cpp" 
    }
    libdirs (PSDK_DIR .. "/output/lib")

    filter "configurations:Debug"
        defines { "DEBUG", "IS_PLATFORM_WIN" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG", "IS_PLATFORM_WIN" }
        optimize "On"

project "CheatMenuIII"
    kind "SharedLib"
    targetdir (GTA3_DIR)
    targetextension ".asi"
    
    files { 
        "../src/cheatmenu.h", 
        "../src/cheatmenu.cpp", 
        "../src/pch.h", 
        "../src/pch.cpp", 
        "../src/hook.h", 
        "../src/hook.cpp", 
        "../src/updater.h", 
        "../src/updater.cpp", 
        "../src/json.h", 
        "../src/json.cpp",
        "../src/ui.h", 
        "../src/ui.cpp",
        "../src/util.h", 
        "../src/util.cpp",
        "../src/menu.h", 
        "../src/menu.cpp",
        "../src/player.h", 
        "../src/player.cpp",
        "../src/animation.h", 
        "../src/animation.cpp",
        "../src/teleport.h", 
        "../src/teleport.cpp",
        "../src/ped.h", 
        "../src/ped.cpp",
        "../src/resourcestore.h", 
        "../src/resourcestore.cpp",
        "../src/fontmgr.h", 
        "../src/fontmgr.cpp",
        "../src/hotkeys.h", 
        "../src/hotkeys.cpp",
        "../src/vehicle.h", 
        "../src/vehicle.cpp",
        "../src/weapon.h", 
        "../src/weapon.cpp",
        "../src/game.h", 
        "../src/game.cpp",
        "../src/timecyc.h", 
        "../src/visual.h", 
        "../src/visual.cpp",
        "../src/filehandler.h", 
        "../src/filehandler.cpp",
        "../src/dllmain.cpp"
    }
    includedirs {
        PSDK_DIR .. "/plugin_III/",
        PSDK_DIR .. "/plugin_III/game_III/",
        PSDK_DIR .. "/shared/",
        PSDK_DIR .. "/shared/game/"
    }
    libdirs (PSDK_DIR .. "/output/lib")
    
    defines { 
        "NDEBUG", 
        "IS_PLATFORM_WIN" ,
        "_CRT_SECURE_NO_WARNINGS",
        "_CRT_NON_CONFORMING_SWPRINTFS",
        "GTA3",
        "_DX9_SDK_INSTALLED",
        "PLUGIN_SGV_10US"
    }

    pchheader "pch.h"
    pchsource "../src/pch.cpp"

    filter "configurations:Debug"
        symbols "On"
        links { 
            "Depend",
            "d3d9",
            "d3d11",
            "XInput9_1_0",
            "Pdh",
            "urlmon",
            "plugin_III_d.lib" 
        }

    filter "configurations:Release"
        optimize "On"
        links { 
            "Depend",
            "d3d9",
            "d3d11",
            "XInput9_1_0",
            "Pdh",
            "urlmon",
            "plugin_III.lib" 
        }

project "CheatMenuVC"
    kind "SharedLib"
    targetdir (GTAVC_DIR)
    targetextension ".asi"
    
    files { 
        "../src/cheatmenu.h", 
        "../src/cheatmenu.cpp", 
        "../src/pch.h", 
        "../src/pch.cpp", 
        "../src/hook.h", 
        "../src/hook.cpp", 
        "../src/updater.h", 
        "../src/updater.cpp", 
        "../src/json.h", 
        "../src/json.cpp",
        "../src/animation.h", 
        "../src/animation.cpp",
        "../src/teleport.h", 
        "../src/teleport.cpp",
        "../src/player.h", 
        "../src/player.cpp",
        "../src/ped.h", 
        "../src/ped.cpp",
        "../src/ui.h", 
        "../src/ui.cpp", 
        "../src/vehicle.h", 
        "../src/vehicle.cpp", 
        "../src/util.h", 
        "../src/util.cpp", 
        "../src/menu.h", 
        "../src/menu.cpp",
        "../src/weapon.h", 
        "../src/weapon.cpp",
        "../src/game.h", 
        "../src/game.cpp",
        "../src/visual.h", 
        "../src/visual.cpp",
        "../src/resourcestore.h", 
        "../src/resourcestore.cpp",
        "../src/fontmgr.h", 
        "../src/fontmgr.cpp",
        "../src/filehandler.h", 
        "../src/filehandler.cpp",
        "../src/hotkeys.h", 
        "../src/hotkeys.cpp",
        "../src/dllmain.cpp"
    }
    includedirs {
        PSDK_DIR .. "/plugin_vc/",
        PSDK_DIR .. "/plugin_vc/game_vc/",
        PSDK_DIR .. "/shared/",
        PSDK_DIR .. "/shared/game/"
    }
    libdirs (PSDK_DIR .. "/output/lib")
    
    defines { 
        "NDEBUG", 
        "IS_PLATFORM_WIN" ,
        "_CRT_SECURE_NO_WARNINGS",
        "_CRT_NON_CONFORMING_SWPRINTFS",
        "GTAVC",
        "_DX9_SDK_INSTALLED",
        "PLUGIN_SGV_10US"
    }

    pchheader "pch.h"
    pchsource "../src/pch.cpp"

    filter "configurations:Debug"
        symbols "On"
        links { 
            "Depend",
            "d3d9",
            "d3d11",
            "XInput9_1_0",
            "Pdh",
            "urlmon",
            "plugin_vc_d.lib" 
        }

    filter "configurations:Release"
        optimize "On"
        links { 
            "Depend",
            "d3d9",
            "d3d11",
            "XInput9_1_0",
            "Pdh",
            "urlmon",
            "plugin_vc.lib" 
        }

project "CheatMenuSA"
    kind "SharedLib"
    targetdir (GTASA_DIR)
    targetextension ".asi"
    
    files { 
        "../src/**.h", 
        "../src/**.hpp", 
        "../src/**.cpp" 
    }
    includedirs {
        PSDK_DIR .. "/plugin_sa/",
        PSDK_DIR .. "/plugin_sa/game_sa/",
        PSDK_DIR .. "/shared/",
        PSDK_DIR .. "/shared/game/"
    }
    libdirs (PSDK_DIR .. "/output/lib")
    
    defines { 
        "NDEBUG", 
        "IS_PLATFORM_WIN" ,
        "_CRT_SECURE_NO_WARNINGS",
        "_CRT_NON_CONFORMING_SWPRINTFS",
        "GTASA",
        "_DX9_SDK_INSTALLED",
        "PLUGIN_SGV_10US"
    }

    pchheader "pch.h"
    pchsource "../src/pch.cpp"

    filter "configurations:Debug"
        symbols "On"
        links { 
            "Depend",
            "d3d9",
            "d3d11",
            "XInput9_1_0",
            "Pdh",
            "urlmon",
            "plugin_d.lib" 
        }

    filter "configurations:Release"
        optimize "On"
        links { 
            "Depend",
            "d3d9",
            "d3d11",
            "XInput9_1_0",
            "Pdh",
            "urlmon",
            "plugin.lib" 
        }
        