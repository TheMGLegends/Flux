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
		explicit Visualizer(GameObject* _gameObject);
		~Visualizer() override;

		void DrawDetails() override;

		void Serialize(nlohmann::flux_json& json) const override;
		void Deserialize(const nlohmann::flux_json& json) override;

		void SetModel(std::string_view _modelName);

		void SetMaterialTexture(std::string_view _textureName);
		Material& GetMaterial();

		void Draw(ID3D11DeviceContext& deviceContext);

	private:
		void DrawModelAndTextureSelector(bool treeOpened);

	private:
		Model* model;
		std::string modelName;
		Material material;
		std::string textureName;
	};
}

