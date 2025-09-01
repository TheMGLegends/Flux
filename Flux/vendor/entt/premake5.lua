---@diagnostic disable: undefined-global, undefined-field

function UseENTT()
    local sourceDir = "vendor/entt"

    includedirs { "Flux/vendor/entt/single_include" }

    filter { "configurations:Debug or Development" }
        -- Debug Build for Windows (VS 2022)
        if not DirectoryExists("Flux/vendor/entt/install/debug") then
            prebuildcommands
            {
                'cmake -S ' .. sourceDir .. ' -B ' .. sourceDir .. '/build/debug -G "Visual Studio 17 2022" -A x64 -DENTT_INSTALL=ON -DENTT_INCLUDE_NATVIS=ON -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDebug -DCMAKE_INSTALL_PREFIX=' .. sourceDir .. '/install/debug',
                'cmake --build ' .. sourceDir .. '/build/debug --config Debug',
                'cmake --install ' .. sourceDir .. '/build/debug --config Debug --prefix ' .. sourceDir .. '/install/debug'
            }
        end
    filter {}

    filter "configurations:Release"
        -- Release Build for Windows (VS 2022)
        if not DirectoryExists("Flux/vendor/entt/install/release") then
            prebuildcommands
            {
                'cmake -S ' .. sourceDir .. ' -B ' .. sourceDir .. '/build/release -G "Visual Studio 17 2022" -A x64 -DENTT_INSTALL=ON -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded -DCMAKE_INSTALL_PREFIX=' .. sourceDir .. '/install/release',
                'cmake --build ' .. sourceDir .. '/build/release --config Release',
                'cmake --install ' .. sourceDir .. '/build/release --config Release --prefix ' .. sourceDir .. '/install/release'
            }
        end
    filter {}
end