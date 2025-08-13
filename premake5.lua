---@diagnostic disable: undefined-global, lowercase-global

-- Implementation Supports: Windows

--[[Useful Commands List:
cppdialect
defines
externalproject
files
filter
includedirs
kind
language
libdirs
links
location
objdir
optimize
project
startproject
staticruntime
targetdir
warnings
workspace
pchheader
pchsource
systemversion
postbuildcommands
runtime
--]]

workspace "Flux"
    architecture "x86_64" -- Only 64-Bit Systems are Supported
    configurations { "Debug", "Release" }
    builddir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    -- TODO Include External Library Premakes (They reference externalprojects with functions for setup into main project)
    include "Flux/vendor/entt"      -- Entity-Component-System Library
    include "Flux/vendor/spdlog"    -- Logging Library
    include "Flux/vendor/yaml-cpp"  -- YAML Parser Library

    -- Startup Project
    filter "action:vs*"
        startproject "Sandbox"
        buildoptions { "/utf-8" }
    filter {}

    -- Main Project
    project "Flux"
        location "Flux"
        kind "SharedLib"
        language "C++"

        -- Build Directories
        targetdir ("build/" .. builddir .. "/%{prj.name}")
        objdir ("build-int/" .. builddir .. "/%{prj.name}")

        -- Precompiled Header
        pchheader "FluxPCH.h"
        pchsource "Flux/src/FluxPCH.cpp"

        -- Project Source Files
        files { "%{prj.name}/src/**.h", "%{prj.name}/src/**.cpp" }

        -- TODO Run External Library Premake Functions that setup includedirs, libdirs, links, etc.
        UseENTT()   -- Entity-Component-System Library
        UseSPDLOG() -- Logging Library
        UseYAML()   -- YAML Parser Library

        -- Flux Engine Include Directory
        includedirs { "%{prj.name}/src" }

        -- Post Build Commands (Copy DLL into Running Project Build Directory)
        local sourceDir = "%{cfg.buildtarget.relpath}"
        local destinationDir = "../build/" .. builddir .. "/Sandbox"
        postbuildcommands { "{COPYDIR} " .. sourceDir .. " " .. destinationDir }

        -- Windows Specific Settings
        filter "system:windows"
            cppdialect "C++20"
            staticruntime "On"
            systemversion "latest"

            defines { "FLUX_PLATFORM_WINDOWS", "FLUX_BUILD_DLL" }
        filter {}

        -- Debug Configuration Settings
        filter "configurations:Debug"
            defines { "FLUX_DEBUG" }
            symbols "On"
            runtime "Debug"
        filter {}

        -- Release Configuration Settings
        filter "configurations:Release"
            defines { "FLUX_RELEASE" }
            optimize "On"
            runtime "Release"
        filter {}

    -- Sandbox Project
    project "Sandbox"
        location "Sandbox"
        kind "ConsoleApp"
        language "C++"

        -- Build Directories
        targetdir ("build/" .. builddir .. "/%{prj.name}")
        objdir ("build-int/" .. builddir .. "/%{prj.name}")

        -- Project Source Files
        files { "%{prj.name}/src/**.h", "%{prj.name}/src/**.cpp" }

        -- Sandbox Include Directories
        includedirs
        {
            "Flux/vendor/entt/single_include",  -- Entity-Component-System Library
            "Flux/vendor/spdlog/include",       -- Logging Library
            "Flux/vendor/yaml-cpp/include",     -- YAML Parser Library

            "Flux/src"
        }

        -- Link to Flux Engine
        links { "Flux" }

        -- Windows Specific Settings
        filter "system:windows"
            cppdialect "C++20"
            staticruntime "On"
            systemversion "latest"

            defines { "FLUX_PLATFORM_WINDOWS" }
        filter {}

        -- Debug Configuration Settings
        filter "configurations:Debug"
            defines { "FLUX_DEBUG" }
            symbols "On"
            runtime "Debug"
        filter {}

        -- Release Configuration Settings
        filter "configurations:Release"
            defines { "FLUX_RELEASE" }
            optimize "On"
            runtime "Release"
        filter {}