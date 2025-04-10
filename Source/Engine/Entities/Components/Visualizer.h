#pragma once

#include "Component.h"

#include <memory>
#include <string>

#include "Core/Renderer/Material.h"

struct ID3D11DeviceContext;

namespace Flux
{
	class Model;

	class Visualizer : public Component, public std::enable_shared_from_this<Visualizer>
	{
	public:
		Visualizer(GameObject* _gameObject);
		virtual ~Visualizer() override;

		virtual void DrawDetails() override;

		virtual void Serialize(nlohmann::flux_json& json) const override;
		virtual void Deserialize(const nlohmann::flux_json& json) override;

		void SetModel(const std::string& _modelName);

		void SetMaterialTexture(const std::string& _textureName);
		inline Material& GetMaterial() { return material; }

		void Draw(ID3D11DeviceContext& deviceContext);

	private:
		Model* model;
		std::string modelName;
		Material material;
		std::string textureName;
	};
}

