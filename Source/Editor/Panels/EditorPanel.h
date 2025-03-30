#pragma once

namespace Flux
{
	class EditorPanel
	{
	public:
		EditorPanel() = default;
		virtual ~EditorPanel() {}

		virtual void Initialise() {}
		virtual void Update(float deltaTime) = 0;
	};
}

