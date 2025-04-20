#pragma once

#include <string>

namespace DirectX
{
	inline namespace DX11
	{
		class SpriteBatch;
		class SpriteFont;
	}
}

namespace Flux::FrameRateMonitor
{
	namespace Internal
	{
		extern DirectX::SpriteFont* spriteFont;
		extern std::string frameRate;
		extern bool isActive;
		extern float scale;
		extern float currentFrameTime;
	}
	
	void Initialise();
	void Update(float deltaTime);
	void Render(DirectX::SpriteBatch* spriteBatch);

	/// @brief Toggles Active State (Starts as Active)
	void Toggle();
	bool IsActive();
}

