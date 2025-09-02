cmake_minimum_required (VERSION 3.15)

project(ImGuizmo VERSION 1.83)

# Add all source files in the ImGuizmo directory automatically.
file(GLOB IMGUIZMO_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

# Add all the source files to the library.
add_library(ImGuizmo STATIC ${IMGUIZMO_SOURCES})
add_library(ImGuizmo::ImGuizmo ALIAS ImGuizmo)

# Link to SFML Graphics
list(APPEND CMAKE_PREFIX_PATH
    "${CMAKE_SOURCE_DIR}/../../../Flux/vendor/sfml/install/debug/lib/cmake/SFML"
    "${CMAKE_SOURCE_DIR}/../../../Flux/vendor/sfml/install/release/lib/cmake/SFML"
)

set(SFML_STATIC_LIBRARIES ON CACHE BOOL "Use SFML static libraries")
find_package(SFML 3.0.1 COMPONENTS Graphics REQUIRED)

# Link to ImGui (Saved inside the imgui-sfml binding system)
list(APPEND CMAKE_PREFIX_PATH
    "${CMAKE_SOURCE_DIR}/../imgui-sfml/install/debug/lib/cmake/ImGui-SFML"
    "${CMAKE_SOURCE_DIR}/../imgui-sfml/install/release/lib/cmake/ImGui-SFML"
)

find_package(ImGui-SFML REQUIRED)
target_link_libraries(ImGuizmo PUBLIC ImGui-SFML::ImGui-SFML)

target_include_directories(
   ImGuizmo
   PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
      $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

install(TARGETS ImGuizmo
    EXPORT ImGuizmoTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}   # for executables (not used for static lib)
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}   # for shared libs
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}   # for static libs
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}  # optional: install headers
)