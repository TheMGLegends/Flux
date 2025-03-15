#pragma once

namespace Flux
{
	enum class ComponentType
	{
		None = 0,

		Transform,
		Camera,
		PhysicsBody,
		Visualizer,
		BoxCollider,
		SphereCollider
	};
}