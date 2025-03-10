#pragma once

#include "Component.h"

#include <d3d11.h>
#include <string>

namespace Flux
{
	class Material;
	class Model;

	class Visualizer : public Component
	{
	public:
		Visualizer(GameObject* _gameObject);
		virtual ~Visualizer() override;

		virtual void Serialize(nlohmann::ordered_json& json) const override;
		virtual void Deserialize(const nlohmann::ordered_json& json) override;

		void SetModel(const std::string& _modelName);
		inline Material& GetMaterial() const { return *material; }

		void Draw(ID3D11DeviceContext& deviceContext);

	private:
		Model* model;
		std::string modelName;
		Material* material;
	};
}

