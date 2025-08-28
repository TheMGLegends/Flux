#pragma once

#include "Flux/Core.h"

#include <memory>

#include "Flux/Events/Event.h"
#include "Flux/Math/Vector2.h"

namespace sf { class RenderWindow; }

namespace Flux
{
	struct WindowProperties
	{
		std::string title;
		unsigned int width;
		unsigned int height;
		Vector2I position;

		WindowProperties(const std::string& title = "Flux Engine",
						 unsigned int width = 1280,
						 unsigned int height = 720,
						 const Vector2I& position = Vector2I())
			: title(title), width(width), height(height), position(position) {}
	};

	class Window : public EventEmitter
	{
	public:
		Window(const WindowProperties& properties = WindowProperties());
		virtual ~Window() override;

		void Update();

		const WindowProperties& GetProperties() const { return data.properties; }
		const std::string& GetTitle() const { return data.properties.title; }
		unsigned int GetWidth() const { return data.properties.width; }
		unsigned int GetHeight() const { return data.properties.height; }

		void SetVSyncEnabled(bool enabled);
		bool IsVSyncEnabled() const { return data.vsyncEnabled; }

		void SetFramerateLimit(unsigned int limit);
		unsigned int GetFramerateLimit() const { return data.framerateLimit; }

	private:
		struct WindowData
		{
			WindowProperties properties;
			bool vsyncEnabled;
			unsigned int framerateLimit;

			WindowData(const WindowProperties& properties = WindowProperties())
				: properties(properties), vsyncEnabled(true), framerateLimit(60) {
			}
		} data;

		struct RepeatedKeyData
		{
			int keyCode;
			int repeatCount;

			RepeatedKeyData(int keyCode = -1, int repeatCount = 0)
				: keyCode(keyCode), repeatCount(repeatCount) {}
		} repeatedKeyData;

		std::unique_ptr<sf::RenderWindow> window;
	};
}

// INFO: Specialization for Logging Window Properties
template<>
struct std::formatter<Flux::WindowProperties>
{
	template <typename ParseContext>
	constexpr ParseContext::iterator parse(ParseContext& ctx)
	{
		return ctx.begin();
	}

	template <typename FormatContext>
	FormatContext::iterator format(const Flux::WindowProperties& properties, FormatContext& ctx) const
	{
		return std::format_to(ctx.out(), "{0}, ({1}, {2})", properties.title, properties.width, properties.height);
	}
};