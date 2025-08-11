-- Implementation Supports: Windows

-- Useful commands to integrate:
-- cppdialect
-- defines
-- externalproject
-- files
-- filter
-- includedirs
-- kind
-- language
-- libdirs
-- links
-- location
-- objdir
-- optimize
-- project
-- startproject
-- staticruntime
-- targetdir
-- warnings
-- workspace
-- pchheader
-- pchsource
-- systemversion
-- postbuildcommands
-- runtime

newoption {
    trigger = "config",
    description = "Select the build configuration",
    default = "Debug",
    category = "Build Options",
    allowed = { 
        {"Debug", "Debug Build"}, 
        {"Release", "Release Build"}
    }
}

workspace "Flux"
    architecture "x86_64" -- Only 64-Bit Systems are Supported
    configurations { "Debug", "Release" }
    builddir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    -- TODO Include External Library Premakes (They reference externalprojects with functions for setup into main project)
    include "Flux/vendor/spdlog"

    -- Startup Project
    filter "action:vs*"
        startproject "Sandbox"
        buildoptions { "/utf-8" }

    -- Main Project
    project "Flux"
        location "Flux"
        kind "SharedLib"
        language "C++"

        -- Build Directories
        targetdir ("build/" .. builddir .. "/%{prj.name}")
        objdir ("build-int/" .. builddir .. "/%{prj.name}")

        -- TODO Precompiled Header

        -- Project Source Files
        files { "%{prj.name}/src/**.h", "%{prj.name}/src/**.cpp" }

        -- TODO Run External Library Premake Functions that setup includedirs, libdirs, links, etc.
        UseSPDLOG()

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

        -- Debug Configuration Settings
        filter "configurations:Debug"
            defines { "FLUX_DEBUG" }
            symbols "On"
            runtime "Debug"

        -- Release Configuration Settings
        filter "configurations:Release"
            defines { "FLUX_RELEASE" }
            optimize "On"
            runtime "Release"
    
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

        -- TODO Run External Library Premake Functions that setup includedirs, libdirs, links, etc.

        -- Sandbox Include Directory
        includedirs { "%{prj.name}/src" }

        -- Link to Flux Engine
        links { "Flux" }

        -- Post Build Commands (Copy Executable into Running Project Build Directory)
        local sourceDir = "%{cfg.buildtarget.relpath}"
        local destinationDir = "../build/" .. builddir .. "/Sandbox"
        postbuildcommands { "{COPYDIR} " .. sourceDir .. " " .. destinationDir }

        -- Windows Specific Settings
        filter "system:windows"
            cppdialect "C++20"
            staticruntime "On"
            systemversion "latest"

            defines { "FLUX_PLATFORM_WINDOWS" }

        -- Debug Configuration Settings
        filter "configurations:Debug"
            defines { "FLUX_DEBUG" }
            symbols "On"
            runtime "Debug"

        -- Release Configuration Settings
        filter "configurations:Release"
            defines { "FLUX_RELEASE" }
            optimize "On"
            runtime "Release"