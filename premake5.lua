---@diagnostic disable: undefined-global, lowercase-global, undefined-field

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

function DirectoryExists(path)
    return os.rename(path, path) and os.isdir(path)
end

workspace "Flux"
    architecture "x86_64" -- Only 64-Bit Systems are Supported
    configurations { "Debug", "Release" }
    builddir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    -- Include External Library Premakes
    include "Flux/vendor/box2d"      -- 2D Physics Library
    include "Flux/vendor/entt"      -- Entity-Component-System Library
    include "Flux/vendor/sfml"      -- Simple and Fast Multimedia Library
    include "Flux/vendor/spdlog"    -- Logging Library
    include "Flux/vendor/yaml-cpp"  -- YAML Parser Library

    -- Startup Project
    filter "action:vs*"
        startproject "FluxEditor"
        buildoptions { "/utf-8" }
    filter {}

    -- Game Engine Project
    project "Flux"
        location "Flux"
        kind "StaticLib"
        language "C++"
        cppdialect "C++20"
        staticruntime "On"

        -- Build Directories
        targetdir ("build/" .. builddir .. "/%{prj.name}")
        objdir ("build-int/" .. builddir .. "/%{prj.name}")

        -- Precompiled Header
        pchheader "FluxPCH.h"
        pchsource "Flux/src/FluxPCH.cpp"

        -- Project Source Files
        files { "%{prj.name}/src/**.h", "%{prj.name}/src/**.cpp" }

        -- Run External Library Premake Functions that setup includedirs, libdirs, links, etc.
        UseBOX2D()   -- 2D Physics Library
        UseENTT()   -- Entity-Component-System Library
        UseSFML()   -- Simple and Fast Multimedia Library
        UseSPDLOG() -- Logging Library
        UseYAML()   -- YAML Parser Library

        -- Include Directories
        includedirs { "%{prj.name}/src" }

        -- Windows Specific Settings
        filter "system:windows"
            systemversion "latest"
            defines { "FLUX_PLATFORM_WINDOWS" }
        filter {}

        -- Debug Configuration Settings
        filter "configurations:Debug"
            defines { "FLUX_DEBUG", "FLUX_ASSERTS_ENABLED" }
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
        cppdialect "C++20"
        staticruntime "On"

        -- Build Directories
        targetdir ("build/" .. builddir .. "/%{prj.name}")
        objdir ("build-int/" .. builddir .. "/%{prj.name}")

        -- Project Source Files
        files { "%{prj.name}/src/**.h", "%{prj.name}/src/**.cpp" }

        -- Include Directories [TODO: Do we need to include all these external libraries?]
        includedirs
        {
            "Flux/vendor/box2d/include",        -- 2D Physics Library
            "Flux/vendor/entt/single_include",  -- Entity-Component-System Library
            "Flux/vendor/sfml/include",         -- Simple and Fast Multimedia Library
            "Flux/vendor/spdlog/include",       -- Logging Library
            "Flux/vendor/yaml-cpp/include",     -- YAML Parser Library

            "Flux/src"
        }

        -- Copy Flux Engine Resources into Project
        prebuildcommands
        {
            "{MKDIR} " .. "../Sandbox/resources",
            "{COPYDIR} " .. "%[%{wks.location}Flux/resources] " .. "../Sandbox/resources"
        }

        -- Link to Flux Engine
        links { "Flux" }

        -- Global Defines
        defines
        {
            "SFML_STATIC",
            "SPDLOG_COMPILED_LIB", "SPDLOG_USE_STD_FORMAT",
            "YAML_CPP_STATIC_DEFINE"
        }

        -- Windows Specific Settings
        filter "system:windows"
            systemversion "latest"
            defines { "FLUX_PLATFORM_WINDOWS" }
        filter {}

        -- Debug Configuration Settings
        filter "configurations:Debug"
            defines
            {
                "FLUX_DEBUG",
                "FLUX_ASSERTS_ENABLED"
            }
            symbols "On"
            runtime "Debug"
        filter {}

        -- Release Configuration Settings
        filter "configurations:Release"
            defines { "FLUX_RELEASE" }
            optimize "On"
            runtime "Release"
        filter {}

    -- Game Engine Editor Project
    project "FluxEditor"
        location "FluxEditor"
        kind "ConsoleApp"
        language "C++"
        cppdialect "C++20"
        staticruntime "On"

        -- Build Directories
        targetdir ("build/" .. builddir .. "/%{prj.name}")
        objdir ("build-int/" .. builddir .. "/%{prj.name}")

        -- Project Source Files
        files { "%{prj.name}/src/**.h", "%{prj.name}/src/**.cpp" }

        -- Include Directories [TODO: Do we need to include all these external libraries?]
        includedirs
        {
            "Flux/vendor/box2d/include",        -- 2D Physics Library
            "Flux/vendor/entt/single_include",  -- Entity-Component-System Library
            "Flux/vendor/sfml/include",         -- Simple and Fast Multimedia Library
            "Flux/vendor/spdlog/include",       -- Logging Library
            "Flux/vendor/yaml-cpp/include",     -- YAML Parser Library

            "Flux/src",
            "Sandbox/src"
        }

        -- Copy Flux Engine Resources into Project
        prebuildcommands
        {
            "{MKDIR} " .. "../FluxEditor/resources",
            "{COPYDIR} " .. "%[%{wks.location}Flux/resources] " .. "../FluxEditor/resources"
        }

        -- Links
        links
        {
            "Flux", -- Flux Engine
            "Sandbox" -- Sandbox [TODO: Temporarily links to this project, change later to any user made project]
        }

        -- Global Defines
        defines
        {
            "SFML_STATIC",
            "SPDLOG_COMPILED_LIB", "SPDLOG_USE_STD_FORMAT",
            "YAML_CPP_STATIC_DEFINE"
        }

        -- Windows Specific Settings
        filter "system:windows"
            systemversion "latest"
            defines { "FLUX_PLATFORM_WINDOWS" }
        filter {}

        -- Debug Configuration Settings
        filter "configurations:Debug"
            defines
            {
                "FLUX_DEBUG",
                "FLUX_ASSERTS_ENABLED"
            }
            symbols "On"
            runtime "Debug"
        filter {}

        -- Release Configuration Settings
        filter "configurations:Release"
            defines { "FLUX_RELEASE" }
            optimize "On"
            runtime "Release"
        filter {}