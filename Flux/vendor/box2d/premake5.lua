---@diagnostic disable: undefined-global, undefined-field

function UseBOX2D()
    local sourceDir = "vendor/box2d"

    includedirs { "Flux/vendor/box2d/include" }

    filter { "configurations:Debug or Development" }
        -- Debug Build for Windows (VS 2022)
        if not DirectoryExists("Flux/vendor/box2d/install/debug") then
            prebuildcommands
            {
                'cmake -S ' .. sourceDir .. ' -B ' .. sourceDir .. '/build/debug -G "Visual Studio 17 2022" -A x64 -DBOX2D_SAMPLES=OFF -DBOX2D_VALIDATE=OFF -DBOX2D_UNIT_TESTS=OFF -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDebug -DCMAKE_INSTALL_PREFIX=' .. sourceDir .. '/install/debug',
                'cmake --build ' .. sourceDir .. '/build/debug --config Debug',
                'cmake --install ' .. sourceDir .. '/build/debug --config Debug --prefix ' .. sourceDir .. '/install/debug'
            }
        end

        links { "box2dd" }
        libdirs { "Flux/vendor/box2d/install/debug/lib" }
    filter {}

    filter "configurations:Release"
        -- Release Build for Windows (VS 2022)
        if not DirectoryExists("Flux/vendor/box2d/install/release") then
            prebuildcommands
            {
                'cmake -S ' .. sourceDir .. ' -B ' .. sourceDir .. '/build/release -G "Visual Studio 17 2022" -A x64 -DBOX2D_SAMPLES=OFF -DBOX2D_VALIDATE=OFF -DBOX2D_UNIT_TESTS=OFF -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded -DCMAKE_INSTALL_PREFIX=' .. sourceDir .. '/install/release',
                'cmake --build ' .. sourceDir .. '/build/release --config Release',
                'cmake --install ' .. sourceDir .. '/build/release --config Release --prefix ' .. sourceDir .. '/install/release'
            }
        end

        links { "box2d" }
        libdirs { "Flux/vendor/box2d/install/release/lib" }
    filter {}
end