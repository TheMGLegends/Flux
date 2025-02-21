#pragma once

#include "Component.h"

#include <d3d11.h>
#include <filesystem>

class Material;
class Model;

class VisualizerComponent : public Component
{
public:
	VisualizerComponent();
	virtual ~VisualizerComponent() override;

	virtual void Serialize(nlohmann::ordered_json& json) const override;
	virtual void Deserialize(const nlohmann::ordered_json& json) override;

	void SetModel(const std::filesystem::path& _modelPath);

	void Draw(ID3D11DeviceContext* deviceContext);

private:
	Model* model;
	std::filesystem::path modelFilepath;
	Material* material;
};

