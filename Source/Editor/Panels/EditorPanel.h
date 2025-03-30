#pragma once

namespace Flux
{
	class EditorPanel
	{
	public:
		EditorPanel() = default;
		virtual ~EditorPanel() {}

		virtual int Initialise() = 0;
		virtual void Update(float deltaTime) = 0;
	};
}

