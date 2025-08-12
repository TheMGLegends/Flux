-- Build using CMakeLists for Windows (VS 2022)

if os.host() == "windows" and _OPTIONS["config"] == "Debug" then
    -- Debug Build
    os.execute('cmake -S . -B build/debug -G "Visual Studio 17 2022" -A x64 -DSPDLOG_USE_STD_FORMAT=ON -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDebug -DCMAKE_INSTALL_PREFIX=install/debug')
    os.execute('cmake --build build/debug --config Debug')
    os.execute('cmake --install build/debug --config Debug --prefix install/debug')

elseif os.host() == "windows" and _OPTIONS["config"] == "Release" then
    -- Release Build
    os.execute('cmake -S . -B build/release -G "Visual Studio 17 2022" -A x64 -DSPDLOG_USE_STD_FORMAT=ON -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded -DCMAKE_INSTALL_PREFIX=install/release')
    os.execute('cmake --build build/release --config Release')
    os.execute('cmake --install build/release --config Release --prefix install/release')
end

function UseSPDLOG()
    defines { "SPDLOG_COMPILED_LIB", "SPDLOG_USE_STD_FORMAT" }
    includedirs { "Flux/vendor/spdlog/include" }

    filter "configurations:Debug"
        links { "spdlogd" }
        libdirs { "Flux/vendor/spdlog/install/debug/lib" }
    filter {}
    
    filter "configurations:Release"
        links { "spdlog" }
        libdirs { "Flux/vendor/spdlog/install/release/lib" }
    filter {}
end

-- TODO Make external project so it shows up in the solution