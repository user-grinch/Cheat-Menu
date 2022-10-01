
----------------------------
-- Premake Project Generator
----------------------------

-- Environment vars
----------------------------
-- Should get picked up automatically if you installed them properly
PSDK_DIR = os.getenv("PLUGIN_SDK_DIR")

if (PSDK_DIR == nil) then
    error("PLUGIN_SDK_DIR environment variable not set")
end

----------------------------

function createProject(projectID)
    upperID = string.upper(projectID)
    pathExt = ""
    if (projectID ~= "sa") then
        pathExt = "_" .. projectID 
    end

    project ("CheatMenu" .. upperID)
        kind "SharedLib"
        targetextension ".asi"
        
        includedirs {
            PSDK_DIR .. "/plugin_" .. projectID .. "/",
            PSDK_DIR .. "/plugin_" .. projectID .. "/game_" .. projectID .. "/",
            PSDK_DIR .. "/shared/",
            PSDK_DIR .. "/shared/game/"
        }

        libdirs {
            PSDK_DIR .. "/output/lib",
            "../lib/",
        }
        
        files { 
            "../src/**.h", 
            "../src/**.hpp", 
            "../src/**.c", 
            "../src/**.cpp" 
        }

        if (upperID ~= "SA") then
            removefiles { 
                "../src/**_sa.c", 
                "../src/**_sa.hpp",
                "../src/**_sa.cpp"
            }
        end
        
        if upperID == "III" then 
            upperID = "3" 

            linkoptions { 
                "/FORCE:MULTIPLE"
            }
        end
        defines { 
            "GTA" .. upperID,
        }

        pchheader "pch.h"
        pchsource "../src/pch.cpp"

        filter "configurations:Debug"
            symbols "On"
            links { 
                "depend",
                "plugin" .. pathExt .. "_d.lib",
            }

        filter "configurations:Release"
            optimize "On"
            links { 
                "depend",
                "plugin" .. pathExt.. ".lib",
            }
end

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

    links { 
        "d3d9",
        "d3d11",
        "Pdh",
        "urlmon",
        "Xinput9_1_0"
    }

    defines { 
        "IS_PLATFORM_WIN" ,
        "_CRT_SECURE_NO_WARNINGS",
        "_CRT_NON_CONFORMING_SWPRINTFS",
        "_DX9_SDK_INSTALLED",
        "PLUGIN_SGV_10US",
        "_GTA_"
    }

    includedirs {
        "../include/",
        "../src/",
    }

project "depend"
    kind "StaticLib"

    files { 
        "../include/**.h", 
        "../include/**.hpp", 
        "../include/**.c", 
        "../include/**.cpp" 
    }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"

createProject("III")
createProject("sa")
createProject("vc")
    