cmake_minimum_required (VERSION 3.8)

project(ImGuizmo VERSION 1.83)

# Add all source files in the ImGuizmo directory automatically.
file(GLOB IMGUIZMO_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

# Add all the source files to the library.
add_library(ImGuizmo STATIC ${IMGUIZMO_SOURCES})
add_library(ImGuizmo::ImGuizmo ALIAS ImGuizmo)

# Link to ImGui (Saved inside the imgui-sfml binding system)
target_link_libraries(ImGuizmo PUBLIC ImGui-SFML)

target_include_directories(
   ImGuizmo
   PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
      $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
