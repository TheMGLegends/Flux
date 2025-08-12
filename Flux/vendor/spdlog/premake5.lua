---@diagnostic disable: undefined-global, undefined-field

function UseSPDLOG()
    local sourceDir = "vendor/spdlog"

    defines { "SPDLOG_COMPILED_LIB", "SPDLOG_USE_STD_FORMAT" }
    includedirs { "Flux/vendor/spdlog/include" }

    filter "configurations:Debug"
        -- Debug Build for Windows (VS 2022)
        prebuildcommands
        {
            'cmake -S ' .. sourceDir .. ' -B ' .. sourceDir .. '/build/debug -G "Visual Studio 17 2022" -A x64 -DSPDLOG_USE_STD_FORMAT=ON -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDebug -DCMAKE_INSTALL_PREFIX=' .. sourceDir .. '/install/debug',
            'cmake --build ' .. sourceDir .. '/build/debug --config Debug',
            'cmake --install ' .. sourceDir .. '/build/debug --config Debug --prefix ' .. sourceDir .. '/install/debug'
        }

        links { "spdlogd" }
        libdirs { "Flux/vendor/spdlog/install/debug/lib" }
    filter {}
    
    filter "configurations:Release"
        -- Release Build for Windows (VS 2022)
        prebuildcommands
        {
            'cmake -S ' .. sourceDir .. ' -B ' .. sourceDir .. '/build/release -G "Visual Studio 17 2022" -A x64 -DSPDLOG_USE_STD_FORMAT=ON -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded -DCMAKE_INSTALL_PREFIX=' .. sourceDir .. '/install/release',
            'cmake --build ' .. sourceDir .. '/build/release --config Release',
            'cmake --install ' .. sourceDir .. '/build/release --config Release --prefix ' .. sourceDir .. '/install/release'
        }

        links { "spdlog" }
        libdirs { "Flux/vendor/spdlog/install/release/lib" }
    filter {}
end