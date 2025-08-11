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

workspace "Flux"
    architecture "x86_64" -- Only 64-Bit Systems are Supported
    configurations { "Debug", "Release" }
    builddir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    -- TODO Include External Library Premakes

    project "Flux"
        location "Flux"
        kind "SharedLib"
        language "C++"