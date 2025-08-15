---@diagnostic disable: undefined-global, undefined-field

function UseYAML()
    local sourceDir = "vendor/yaml-cpp"

    defines { "YAML_CPP_STATIC_DEFINE" }
    includedirs { "Flux/vendor/yaml-cpp/include" }

    filter "configurations:Debug"
        -- Debug Build for Windows (VS 2022)
        if not DirectoryExists("Flux/vendor/yaml-cpp/install/debug") then
            prebuildcommands
            {
                'cmake -S ' .. sourceDir .. ' -B ' .. sourceDir .. '/build/debug -G "Visual Studio 17 2022" -A x64 -DYAML_CPP_INSTALL=ON -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDebug -DCMAKE_INSTALL_PREFIX=' .. sourceDir .. '/install/debug',
                'cmake --build ' .. sourceDir .. '/build/debug --config Debug',
                'cmake --install ' .. sourceDir .. '/build/debug --config Debug --prefix ' .. sourceDir .. '/install/debug'
            }
        end

        links { "yaml-cppd" }
        libdirs { "Flux/vendor/yaml-cpp/install/debug/lib" }
    filter {}
    
    filter "configurations:Release"
        -- Release Build for Windows (VS 2022)
        if not DirectoryExists("Flux/vendor/yaml-cpp/install/release") then
            prebuildcommands
            {
                'cmake -S ' .. sourceDir .. ' -B ' .. sourceDir .. '/build/release -G "Visual Studio 17 2022" -A x64 -DYAML_CPP_INSTALL=ON -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded -DCMAKE_INSTALL_PREFIX=' .. sourceDir .. '/install/release',
                'cmake --build ' .. sourceDir .. '/build/release --config Release',
                'cmake --install ' .. sourceDir .. '/build/release --config Release --prefix ' .. sourceDir .. '/install/release'
            }
        end

        links { "yaml-cpp" }
        libdirs { "Flux/vendor/yaml-cpp/install/release/lib" }
    filter {}
end