---@diagnostic disable: undefined-global, undefined-field

function UseSFML()
    local sourceDir = "vendor/sfml"

    defines { "SFML_STATIC" }
    includedirs { "Flux/vendor/sfml/include" }

    -- All Config SFML External Links
    links
    {
        "opengl32.lib",
        "winmm.lib",
        "gdi32.lib",
        "ws2_32.lib"
    }

    filter { "configurations:Debug or Development" }
        -- Debug Build for Windows (VS 2022)
        if not DirectoryExists("Flux/vendor/sfml/install/debug") then
            prebuildcommands
            {
                'cmake -S ' .. sourceDir .. ' -B ' .. sourceDir .. '/build/debug -G "Visual Studio 17 2022" -A x64 -DBUILD_SHARED_LIBS=OFF -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDebug -DCMAKE_INSTALL_PREFIX=' .. sourceDir .. '/install/debug',
                'cmake --build ' .. sourceDir .. '/build/debug --config Debug --parallel',
                'cmake --install ' .. sourceDir .. '/build/debug --config Debug --prefix ' .. sourceDir .. '/install/debug'
            }
        end

        links
        {
            "freetyped.lib",
            "flacd.lib",
            "vorbisencd.lib",
            "vorbisfiled.lib",
            "vorbisd.lib",
            "oggd.lib",

            "sfml-audio-s-d.lib",
            "sfml-graphics-s-d.lib",
            "sfml-network-s-d.lib",
            "sfml-system-s-d.lib",
            "sfml-window-s-d.lib"
        }
        libdirs { "Flux/vendor/sfml/install/debug/lib" }
    filter {}

    filter "configurations:Release"
        -- Release Build for Windows (VS 2022)
        if not DirectoryExists("Flux/vendor/sfml/install/release") then
            prebuildcommands
            {
                'cmake -S ' .. sourceDir .. ' -B ' .. sourceDir .. '/build/release -G "Visual Studio 17 2022" -A x64 -DBUILD_SHARED_LIBS=OFF -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded -DCMAKE_INSTALL_PREFIX=' .. sourceDir .. '/install/release',
                'cmake --build ' .. sourceDir .. '/build/release --config Release --parallel',
                'cmake --install ' .. sourceDir .. '/build/release --config Release --prefix ' .. sourceDir .. '/install/release'
            }
        end

        links
        {
            "freetype.lib",
            "flac.lib",
            "vorbisenc.lib",
            "vorbisfile.lib",
            "vorbis.lib",
            "ogg.lib",

            "sfml-audio-s.lib",
            "sfml-graphics-s.lib",
            "sfml-network-s.lib",
            "sfml-system-s.lib",
            "sfml-window-s.lib"
        }
        libdirs { "Flux/vendor/sfml/install/release/lib" }
    filter {}

    filter "system:windows"
        linkoptions { "/IGNORE:4006" } -- Ignore Duplicate Link Warnings (System Picks Only One Anyway)
    filter {}
end