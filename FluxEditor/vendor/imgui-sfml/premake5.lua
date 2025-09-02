---@diagnostic disable: undefined-global, undefined-field

function UseIMGUISFML()
    local sourceDir = "vendor/imgui-sfml"

    filter { "configurations:Debug or Development" }
        -- Debug Build for Windows (VS 2022)
        if not DirectoryExists("FluxEditor/vendor/imgui-sfml/install/debug") then
            prebuildcommands
            {
                'cmake -S ' .. sourceDir .. ' -B ' .. sourceDir .. '/build/debug -G "Visual Studio 17 2022" -A x64 -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDebug -DCMAKE_INSTALL_PREFIX=' .. sourceDir .. '/install/debug',
                'cmake --build ' .. sourceDir .. '/build/debug --config Debug',
                'cmake --install ' .. sourceDir .. '/build/debug --config Debug --prefix ' .. sourceDir .. '/install/debug'
            }
        end

        links { "ImGui-SFML" }
        includedirs { "FluxEditor/vendor/imgui-sfml/install/debug/include" }
        libdirs { "FluxEditor/vendor/imgui-sfml/install/debug/lib" }
    filter {}

    filter "configurations:Release"
        -- Release Build for Windows (VS 2022)
        if not DirectoryExists("FluxEditor/vendor/imgui-sfml/install/release") then
            prebuildcommands
            {
                'cmake -S ' .. sourceDir .. ' -B ' .. sourceDir .. '/build/release -G "Visual Studio 17 2022" -A x64 -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded -DCMAKE_INSTALL_PREFIX=' .. sourceDir .. '/install/release',
                'cmake --build ' .. sourceDir .. '/build/release --config Release',
                'cmake --install ' .. sourceDir .. '/build/release --config Release --prefix ' .. sourceDir .. '/install/release'
            }
        end

        links { "ImGui-SFML" }
        includedirs { "FluxEditor/vendor/imgui-sfml/install/release/include" }
        libdirs { "FluxEditor/vendor/imgui-sfml/install/release/lib" }
    filter {}
end