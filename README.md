# Flux Engine

<p align="center">
  <a>
    <img src="Assets/Default/FluxIcon.ico" width="200" height = "200" alt="Flux Engine logo">
  </a>
</p>

## Insights

Flux is a 3D windows only game engine that is comprised of a set of core features/tools that
make game development simpler. The engine is written using C++, utilizes D3D11 to render the 
world and includes an editor to speed up development. Note this is the first iteration of the 
engine, so expect bugs and a lack of most advanced features that are available in popular game 
engines nowadays.

## Third-Party Libraries

Below is a list of third-party libraries that the Flux Engine utilizes to work as expected and
provide a better development experience for the user:

1. [ImGui](https://github.com/ocornut/imgui) - Editor UI
2. [ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo) - Editor Gizmos
3. [fmod](https://www.fmod.com/) - Audio
4. [PhysX](https://github.com/NVIDIA-Omniverse/PhysX) - Physics
5. [DirectXTK](https://github.com/microsoft/DirectXTK) - Texture Loading, Maths, Text Rendering, Debug Gizmos
6. [nlohmann json](https://github.com/nlohmann/json) - De/Serialization
7. [Assimp](https://github.com/assimp/assimp) - Model Loading
8. [SDL](https://github.com/libsdl-org/SDL) - Window Creation & Input

## What Is Offered

Utilizing the third-party libraries above, the following is offered by the engine:

### Editor
1. Scene View Panel - FPS Counter, Gizmo Selector Icons, Play/Stop and Pause Buttons
2. Scene Hierarchy Panel - Scene Name, Object List, Object Renaming, Add Game Object Button, Set Starter Scene, Search Bar
3. Details Panel - Selected Object Component Display, Object Renaming, Add Component Button
4. Contents Drawer - Specified Directory Displayed, Navigation (Back Button & Search Bar), Drag & Drop Functionality (Models & Textures -> Details Panel), Create New Scenes

### Editor Gizmos
1. Translation, Rotation and Scaling (Local & World Space) + Panning

### Audio 
1. 2D Music & SFX
2. Spatial (3D) SFX

### Physics
1. Dynamic & Static Colliders (Box & Sphere)
2. Add Forces, Gravity
3. OBB (Collision Detection & Resolution)
4. Collision/Trigger Responses (Enter & Exit)

### DirectXTK
1. Debug Gizmos (Camera View Frustum, Box & Sphere Collider Outlines)
2. Texture Loading (PNG, JPG, BMP...)

### D3D11
1. Unlit 3D Rendering
2. One Material, Texture & Model per Game Object
3. Skybox/Solid Colour Background
4. Camera Switching

### De/Serialization
1. Game Settings
2. Scenes, Components, Game Objects (Saves GO type, custom variable serialization not supported)

### Model Loading
1. OBJ, FBX (Certain versions may not work)

### Input
1. M&K, Gamepad Support
2. Key/Button/Trigger Presses (Pressed, Held, Released)
3. Scroll Wheel Values, Mouse Movement Values (Relative & Absolute Modes), Joystick Values
5. Gamepad Deadzone

## Usability

Here is a list of all the controls/features provided by the editor thus far:

1. Right Click (Scene Name) – Set starter scene in scene hierarchy panel.
2. Double Click (Game Object) – Rename object in scene hierarchy
3. Ctrl + S – Save current scene
4. Asterisk (*) Next to Scene Name – Indicates unsaved changes
5. Right Click (Contents Drawer) – Add a new scene in the current directory
6. Right Click + Drag (Scene View) – Look around using editor camera
7. Right Click + WASD (Scene View) – Fly through scene (editor mode only)
8. Right Click + Q/E (Scene View) – Move editor camera down/up
9. Scroll Wheel (Scene View Selected) – Adjust editor camera movement speed
10. Double Click (Folder in Contents Drawer) – Open folder to view contents
11. Drag & Drop (Asset) – Move asset (texture/model) into component in details panel
12. Q/W/E/R (Scene View Selected) – Change transform mode: Pan, Translate, Rotate, Scale
13. Z (Scene View + Object Selected) – Toggle transform space (World/Local)
14. Double Click (Scene in Contents Drawer) – Open selected scene
15. Details Panel (Object Selected) – Rename object via name field
16. F3 – Toggle FPS counter on/off
17. Custom Classes – To appear in Add GameObject, follow Cube/Sphere class examples

## Getting the Engine

The engine has some third-party libraries installed and stored in the vendors folder of the project already (ImGui, SDL...), other libraries
however have been downloaded as packages using the NuGet Package Manager, therefore Microsoft Visual Studio will be needed
to be able to run the engine.

1. Ensure you have Microsoft Visual Studio 2022
2. Check your visual studio installer and ensure you have "Desktop development with C++" included in your installation of visual studio
3. Download the project from GitHub and unzip it
4. Locate the .sln file and double left click it to open the project in visual studio
5. Build the Project by pressing "Local Windows Debugger" next to the green filled in play button
6. Once built, the engine should now be running.
7. The source code of the engine is availabe so you are able to browse through if you don't understand something

## Common Issues

One common issue you may have depending on your devices permissions may be related to xcopy commands, these commands copy dlls that are necessary for
the projects audio system to be able to function properly and ultimately be able to run the engine. These commands can be located by:

1. Right Clicking FluxEngine in the Solution Explorer and going to Properties > Build Events > Post-Build Events > Command Line
2. Click the drop-down triangle that appears when you click on command line and click edit
3. Remove the 'x' from both commands it should now read copy... instead of xcopy...
4. Press Ok, apply changes and now try running the project again.
