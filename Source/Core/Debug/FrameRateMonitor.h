#pragma once

#include <string>
#include <SimpleMath.h>
#include <SpriteFont.h>

#include "Core/Debug/Debug.h"
#include "Core/Renderer/AssetHandler.h"
#include "Core/Time/Time.h"

namespace Flux::FrameRateMonitor
{
	namespace Internal
	{
		inline DirectX::SpriteFont* spriteFont = nullptr;
		inline std::string frameRate = "0";
		inline bool isActive = true;
		inline float scale = 0.5f;
		inline float currentFrameTime = 0.0f;
	}
	
	inline void Initialise()
	{
		using namespace Internal;

		spriteFont = AssetHandler::GetFont("SimpleFont");

		if (!spriteFont)
		{
			Debug::LogError("FrameRateMonitor::Initialise() - Failed to get SpriteFont");
			return;
		}
	}

	inline void Update(float deltaTime)
	{
		using namespace Internal;

		currentFrameTime += deltaTime;

		if (currentFrameTime >= 1.0f)
		{
			frameRate = "FPS: " + std::to_string(static_cast<int>(Time::GetFrameCount()));
			currentFrameTime = 0.0f;
		}
	}

	inline void Render(DirectX::SpriteBatch* spriteBatch)
	{
		using namespace Internal;
		using namespace DirectX::SimpleMath;

		if (!isActive)
			return;

		spriteFont->DrawString(spriteBatch, frameRate.c_str(), Vector2(5.0f, 5.0f), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), scale);
	}

	/// @brief Toggles Active State (Starts as Active)
	inline void Toggle()
	{
		using namespace Internal;

		isActive = !isActive;
	}
}

