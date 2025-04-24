This documentation contains instructions on how you can access and use different parts of the engine,
unlike the ReadMe which contains keybinds, this documentation will contain more in-depth code specific
knowledge. 

One thing to keep in mind is that namespaces have been used to encompass the classes in
the engine, all of them have been wrapped around with the "Flux" namespace and further namespaces in
certain scenarios like "Flux::Debug" have also been used so if you are unable to call certain functions 
even after including their respective headers, do check the headers themselves to see the type of 
namespace being used.

# Debug Logging System
To gain the ability to send debug messages (Logs, Warnings & Errors) to the console, you will need to
include the Debug header like so:

```C++
#include "Core/Debug/Debug.h"
```

Then you can use the following functions in the Debug header to debug different levels of severity
depending on your needs:

```C++
void Log(const std::string& message);
void LogWarning(const std::string& message);
void LogError(const std::string& message);
```

# Math Helpers
In case you need some maths helpers you can include the following header, note that ideally you should
use maths functions provided by standard libraries or DirectXTK instead as they are likely to be more
optimized and extensive in terms of functionality they provide:

```C++
#include "Core/Helpers/MathHelpers.h"
```

The header contains the following functions:

```C++
float Clamp(float value, float min, float max);
float Max(float x, float y, float z);
```

# Input System
To utilize the capabilities of the input system you need to include the following header:

```C++
#include "Core/Input/Input.h"
```

Included in this header is a range of functionalities from getting the pressed state (Pressed, Held,
Released) of different types of devices (Keyboard Keys, Mouse Buttons, Gamepad Buttons & Triggers),
to things like scroll wheel values, mouse positions (Relative & Absolute) and joystick axes:

```C++
static bool GetKey(SDL_Scancode key);
static bool GetKeyDown(SDL_Scancode key);
static bool GetKeyUp(SDL_Scancode key);

static bool GetMouseButton(SDL_MouseButtonFlags button);
static bool GetMouseButtonDown(SDL_MouseButtonFlags button);
static bool GetMouseButtonUp(SDL_MouseButtonFlags button);

static const DirectX::SimpleMath::Vector2& GetMousePosition();
static void SetMousePosition(const DirectX::SimpleMath::Vector2& position);

/// @param isRelative: true = relative, false = absolute
static void SetMouseMode(bool _isRelative);
static bool IsMouseRelative();

/// @brief Get the vertical scroll of the mouse wheel
/// @param verticalScroll: The variable that will be populated with the vertical scroll value
/// @return true if the scroll wheel is being scrolled (not 0)
static bool GetMouseVerticalScroll(float& verticalScroll);

static bool GetGamepadButton(SDL_GamepadButton button);
static bool GetGamepadButtonDown(SDL_GamepadButton button);
static bool GetGamepadButtonUp(SDL_GamepadButton button);

/// @param trigger: Only works with SDL_GAMEPAD_AXIS_LEFT_TRIGGER and SDL_GAMEPAD_AXIS_RIGHT_TRIGGER
/// @param axisState: Optional parameter to get the axis state of the trigger between 0 and 1
static bool GetTrigger(SDL_GamepadAxis trigger, float* axisState = nullptr);

/// @param trigger: Only works with SDL_GAMEPAD_AXIS_LEFT_TRIGGER and SDL_GAMEPAD_AXIS_RIGHT_TRIGGER
/// @param axisState: Optional parameter to get the axis state of the trigger between 0 and 1
static bool GetTriggerDown(SDL_GamepadAxis trigger, float* axisState = nullptr);

/// @param trigger: Only works with SDL_GAMEPAD_AXIS_LEFT_TRIGGER and SDL_GAMEPAD_AXIS_RIGHT_TRIGGER
/// @param axisState: Optional parameter to get the axis state of the trigger between 0 and 1
static bool GetTriggerUp(SDL_GamepadAxis trigger, float* axisState = nullptr);

static DirectX::SimpleMath::Vector2 GetJoystickAxes(GamepadJoystick joystick, bool inverseY = true);
```

# Time System
In most cases, most of your logic should be included in functions that already have access to a deltaTime
parameter that is passed in by the engine (Apart from things like Start functions etc.), however in case
you do want to get access to things like deltaTime in your own custom functions, you will need to include
the following header:

```C++
#include "Core/Time/Time.h"
```

Here you will be able to get things like the DeltaTime, ElapsedTime and current frame count:

```C++
static float DeltaTime();
static float ElapsedTime();
static unsigned int GetFrameCount();
```

# Audio System
Unfortunately, the audio system isn't provided in the form of a component that can be attached to game objects
visually as I didn't have enough time to implement this, however nevertheless, it is still fairly simple to use
purely via code. 

To gain access to the capabilities of the Audio system you will need to include the following header:

```C++
#include "Engine/Audio/Audio.h"
```

Here you will be given access to the following functions:

```C++
/// @brief Plays a 2D sound
/// @param volume 0.0f to 1.0f (0.0f is silent, 1.0f is full volume)
/// @param isLooping If false the sound will play once, if true the sound will loop
static void PlaySound2D(const std::string& audioName, float volume = 1.0f, bool isLooping = false);

/// @brief Plays a 3D sound
/// @param position The position of the sound in 3D space
/// @param volume 0.0f to 1.0f (0.0f is silent, 1.0f is full volume)
/// @param radius The radius of the sound in 3D space
/// @param isLooping If false the sound will play once, if true the sound will loop
static void PlaySound3D(const std::string& audioName, const FMOD_VECTOR& position, float volume = 1.0f, float radius = 100.0f, bool isLooping = false);

/// @brief Plays 2D music
/// @param volume 0.0f to 1.0f (0.0f is silent, 1.0f is full volume)
/// @param isLooping If false the music will play once, if true the music will loop
static void PlayMusic(const std::string& audioName, float volume = 1.0f, bool isLooping = true);

/// @brief Primarily used for looping music tracks
static void StopSound(std::string_view audioName);

static void StopAllSounds();
```

The biggest thing to note is the string passed in representing the audioName, this audio name matches the name of 
the audio file that you added to the assets directory, so if you had an audio file "CharacterDeathSound.mp3", the
string you would pass in to play the audio would be "CharacterDeathSound". Keep in mind, renaming the audio file
means you will need to update any parts of your code where you are using the old name of the audio file otherwise
the audio won't play.

# Component System
There is a couple of components that the engine supports, here is a list of the supported components:

1. Transform
2. Camera
3. Visualizer (Similar to unities Mesh & Mesh Renderer Components)
4. PhysicsBody (Similar to unities Rigidbody Component)
5. BoxCollider
6. SphereCollider

Each component has it's own specific capabilities and functionality that can be changed both via code and via the
details panel in the editor. To gain access to any of these components you should use the following include path:

```C++
#include "Engine/Entities/Components/[DesiredComponent].h"
```

In the case of colliders:

```C++
#include "Engine/Entities/Components/Colliders/[DesiredCollider].h"
```

# Game Object System
This will be the system you will utilize the most as a user of the engine, hence the documentation for this will be
broken down into different parts depending on what you are looking for.

## Creating Custom Game Objects
To create your own custom game objects that you can add to the scene, you will need to firstly inherit from GameObject
like so:

```C++
#include "Engine/Entities/GameObjects/GameObject.h"

namespace Flux
{
	class MyGameObject : public GameObject
	{
		REFLECT(MyGameObject);

	public:
		MyGameObject() {}
		~MyGameObject() override {}
	};
}
```

Ensure you override the destructor so the objects contents can be properly cleaned up during destruction. The REFLECT
macro is what allows your own custom game object to be visible in the menu that appears when you select the "Add Game
Object" button in the hierarchy view, this way your custom object with it's own custom behaviour will be spawned into
the scene.

To implement your own custom logic you can choose from the following list of functions found in the GameObject class
and overriding them in your class:

```C++
virtual void Start() {}
virtual void Update(float deltaTime) {}
virtual void LateUpdate(float deltaTime) {}
virtual void FixedUpdate(float fixedDeltaTime) {}

virtual void OnCollisionEnter(std::shared_ptr<Collider> other) {}
virtual void OnCollisionExit(std::shared_ptr<Collider> other) {}

virtual void OnTriggerEnter(std::shared_ptr<Collider> other) {}
virtual void OnTriggerExit(std::shared_ptr<Collider> other) {}

virtual void OnDisable() {}
virtual void OnEnable() {}
virtual void OnDestroy() {}
```

## Other Game Object Functions
Alongside the functions you can override to implement your own custom logic, there are also some other functions you
can use for different scenarios.

To mark an object for destruction you can call the Destroy function, note that any last minute logic before the object
is destroyed can be carried out in the OnDestroy method, which in turn gets called when Destroy is called.

You can use the following function to check if a game object contains a specific component:

```C++
template<class T>
bool HasComponent();
```

You can call the function from a game object itself like so:

```C++
if (HasComponent<PhysicsBody>())
{
  // INFO: Logic to run if current game object contains a PhysicsBody component
}
```

You can also call the function from outside of the game object, as long as you have a reference to it like so:

```C++
if (gameObject->HasComponent<Camera>())
{
  // INFO: Logic to run if the referenced game object contains a Camera component
}
```

The same logic in terms of calling them internally and externally applies to the other functions provided as well.

You can get a component granted that the object actually contains it, if it doesn't contain it, it will return a default
empty weak_ptr:

```C++
template<class T>
std::weak_ptr<T> GetComponent();

void OnCollisionEnter(std::shared_ptr<Collider> other) override
{
	if (std::shared_ptr<Visualizer> visualizer = other->GetGameObject()->GetComponent<Visualizer>().lock())
	{
		// INFO: The other object we are colliding with has a valid visualizer component
		// now we can run our custom logic on it
	}
}
```

You can add components programmatically as well as opposed to just doing so via the editor "Add Component" key by calling
the following:

```C++
template<class T, typename... Args>
std::weak_ptr<T> AddComponent(Args&&... args);

if (std::shared_ptr<PhysicsBody> physicsBody = AddComponent<PhysicsBody>(this).lock())
{
	// INFO: PhysicsBody was added successfully
}
```

AddComponent returns a weak_ptr to the added component, locking it or calling .expired() on it allows us to check whether
it has been added successfully, the parameter passed in "this" refers to the owning game object that the component is a
part of.

Lastly you can also remove components programmatically by calling the following:

```C++
template<class T>
bool RemoveComponent(std::weak_ptr<T> component);

// INFO: There is a box collider on this object
std::weak_ptr<Collider> boxCollider = AddComponent<BoxCollider>(this);

if (RemoveComponent(boxCollider))
{
	// INFO: Successfully removed the box collider
}
```

RemoveComponent takes a weak_ptr parameter pointing to the component you want to remove, if the component is found on the
game object it will be queued for deletion at the end of the current frame and the function will return true to signify
the component has been correctly removed.

# Scene Context
The SceneContext is a global way to get access to the scene and is primarly used when you want to spawn objects into your
scene programmatically whilst the game is running e.g. (Shooting bullets from a gun).

Firstly include the SceneContext header:

```C++
#include "Engine/Scene/SceneContext.h"
```

Then you can call the following function to spawn the specified game object at a specific location and with a specific
orientation:

```C++
template<class T, typename... Args>
static T* SpawnGameObject(const DirectX::SimpleMath::Vector3& spawnLocation, const DirectX::SimpleMath::Quaternion& spawnRotation, Args&&... args);

SceneContext::SpawnGameObject<Cube>(Vector3::Zero, Quaternion::Identity);
```

Similarly to adding components, after you've specified location and rotation you can pass in any number of arguments
that the game objects constructor that you are trying to spawn in takes.

The SceneContext also provides a programmatical way for you to switch levels during play mode:

```C++
static void LoadScene(const std::string& sceneName);
```

The sceneName provided refers to the name of the json file you want to load in, which should be located somewhere in
the assets directory, otherwise it won't be recognized and therefore won't be loaded.

# Other Functionalities
You can set the project to be in standalone, which basically means that when the application is run the editor won't appear,
it will purely just be the game that you've made running in play mode.

You can achieve this by going in the "main.cpp" file and changing the parameter "false" to "true" to signify your wish to
start the application in standalone mode.

```C++
int main()
{
	Flux::Application app{ true };
}
```

Note that when built via standalone you will need to specify the starter scene you want to be loaded as there is no other
way to do so since the editor will no longer display or be available. How to set the starter scene is already provided in
the ReadMe or you can also look into the "GameConfig.json" file to change it manually to your desired starter scene.
