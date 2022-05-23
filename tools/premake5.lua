
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
    targetdir "../build/bin"

    libdirs {
        PSDK_DIR .. "/output/lib"
    }

    links { 
        "d3d9",
        "d3d11",
        "Pdh",
        "urlmon"
    }

project "depend"
    kind "StaticLib"

    files { 
        "../depend/**.h", 
        "../depend/**.hpp", 
        "../depend/**.c", 
        "../depend/**.cpp" 
    }

    filter "configurations:Debug"
        defines { "DEBUG", "IS_PLATFORM_WIN" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG", "IS_PLATFORM_WIN" }
        optimize "On"

project "CheatMenuIII"
    kind "SharedLib"
    targetextension ".asi"
    
    files { 
        "../src/cheatmenu.h", 
        "../src/cheatmenu.cpp", 
        "../src/pch.h", 
        "../src/pch.cpp", 
        "../src/d3dhook.h", 
        "../src/d3dhook.cpp", 
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
        "../src/dllmain.cpp",
        "../src/locale.h",
        "../src/locale.cpp",
        "../src/rpc.h",
        "../src/rpc.cpp"
    }
    includedirs {
        PSDK_DIR .. "/plugin_III/",
        PSDK_DIR .. "/plugin_III/game_III/",
        PSDK_DIR .. "/shared/",
        PSDK_DIR .. "/shared/game/"
    }
    libdirs (PSDK_DIR .. "/output/lib")
    
    defines { 
        "IS_PLATFORM_WIN" ,
        "_CRT_SECURE_NO_WARNINGS",
        "_CRT_NON_CONFORMING_SWPRINTFS",
        "GTA3",
        "_DX9_SDK_INSTALLED",
        "PLUGIN_SGV_10US"
    }

    linkoptions { 
        "/FORCE:MULTIPLE"
    }

    pchheader "pch.h"
    pchsource "../src/pch.cpp"

    filter "configurations:Debug"
        symbols "On"
        links { 
            "depend",
            "plugin_III_d.lib" 
        }

    filter "configurations:Release"
        optimize "On"
        links { 
            "depend",
            "plugin_III.lib" 
        }

project "CheatMenuVC"
    kind "SharedLib"
    targetextension ".asi"
    
    files { 
        "../src/cheatmenu.h", 
        "../src/cheatmenu.cpp", 
        "../src/pch.h", 
        "../src/pch.cpp", 
        "../src/d3dhook.h", 
        "../src/d3dhook.cpp", 
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
        "../src/dllmain.cpp",
        "../src/locale.h",
        "../src/locale.cpp",
        "../src/rpc.h",
        "../src/rpc.cpp"
    }
    includedirs {
        PSDK_DIR .. "/plugin_vc/",
        PSDK_DIR .. "/plugin_vc/game_vc/",
        PSDK_DIR .. "/shared/",
        PSDK_DIR .. "/shared/game/"
    }
    libdirs (PSDK_DIR .. "/output/lib")
    
    defines { 
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
            "depend",
            "plugin_vc_d.lib" 
        }

    filter "configurations:Release"
        optimize "On"
        links { 
            "depend",
            "plugin_vc.lib" 
        }

project "CheatMenuSA"
    kind "SharedLib"
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
    libdirs {
        PSDK_DIR .. "/output/lib",
        "../depend/lib"
    }
    
    defines { 
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
            "depend",
            "plugin_d.lib",
        }

    filter "configurations:Release"
        optimize "On"
        links { 
            "depend",
            "plugin.lib",
        }
        