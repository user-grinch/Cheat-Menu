
----------------------------
-- Project Generator
----------------------------
-- Environment vars
PSDK_DIR = os.getenv("PLUGIN_SDK_DIR")
DX9SDK_DIR = os.getenv("DIRECTX9_SDK_DIR")
GTASA_DIR = "C:/Work/GTASanAndreas"
GTAVC_DIR = "F:/GTA Vice City"

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

project "CheatMenuVC"
    kind "SharedLib"
    targetdir (GTAVC_DIR)
    targetextension ".asi"
    
    files { 
        "../src/CheatMenu.h", 
        "../src/CheatMenu.cpp", 
        "../src/pch.h", 
        "../src/pch.cpp", 
        "../src/Hook.h", 
        "../src/Hook.cpp", 
        "../src/Updater.h", 
        "../src/Updater.cpp", 
        "../src/Json.h", 
        "../src/Json.cpp",
        "../src/Animation.h", 
        "../src/Animation.cpp",
        "../src/Teleport.h", 
        "../src/Teleport.cpp",
        "../src/Player.h", 
        "../src/Player.cpp",
        "../src/Ped.h", 
        "../src/Ped.cpp",
        "../src/Ui.h", 
        "../src/Ui.cpp", 
        "../src/Vehicle.h", 
        "../src/Vehicle.cpp", 
        "../src/Util.h", 
        "../src/Util.cpp", 
        "../src/Menu.h", 
        "../src/Menu.cpp",
        "../src/Weapon.h", 
        "../src/Weapon.cpp",
        "../src/Game.h", 
        "../src/Game.cpp",
        "../src/ResourceStore.h", 
        "../src/ResourceStore.cpp"
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
        