#include "FrameRateMonitor.h"

#include <format>
#include <SimpleMath.h>
#include <SpriteBatch.h>
#include <SpriteFont.h>

#include "Core/Configs/EditorConfig.h"

#include "Core/Debug/Debug.h"
#include "Core/Renderer/AssetHandler.h"
#include "Core/Time/Time.h"

namespace Flux::FrameRateMonitor
{
	namespace Internal
	{
		DirectX::SpriteFont* spriteFont = nullptr;
		std::string frameRate = "0";
		bool isActive = true;
		float scale = 0.5f;
		float currentFrameTime = 0.0f;
	}

	void Initialise()
	{
		using namespace Internal;

		spriteFont = AssetHandler::GetFont("SimpleFont");

		if (!spriteFont)
		{
			Debug::LogError("FrameRateMonitor::Initialise() - Failed to get SpriteFont");
			return;
		}
	}

	void Update(float deltaTime)
	{
		using namespace Internal;

		currentFrameTime += deltaTime;

		if (currentFrameTime >= 1.0f)
		{
			frameRate = std::format("FPS: {}", static_cast<int>(Time::GetFrameCount()));
			currentFrameTime = 0.0f;
		}
	}

	void Render(DirectX::SpriteBatch* spriteBatch)
	{
		using namespace Internal;
		using namespace DirectX::SimpleMath;

		if (!isActive) { return; }

		spriteFont->DrawString(spriteBatch, frameRate.c_str(), Vector2(0.0f, EditorConfig::GetSceneViewHeight() - 24.0f),
							   DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), scale);
	}

	void Toggle()
	{
		using namespace Internal;
		isActive = !isActive;
	}

	void SetIsActive(bool _isActive)
	{
		using namespace Internal;
		isActive = _isActive;
	}

	bool IsActive()
	{
		using namespace Internal;
		return isActive;
	}

}
