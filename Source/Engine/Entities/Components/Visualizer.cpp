#include "Visualizer.h"

#include "Core/Configs/DirectXConfig.h"
#include "Core/Debug/Debug.h"
#include "Core/Renderer/AssetHandler.h"
#include "Core/Renderer/Material.h"
#include "Core/Renderer/Model.h"

using namespace Flux;
using namespace Flux::DirectXConfig;

Visualizer::Visualizer(GameObject* _gameObject) : Component(_gameObject), model(nullptr), textureName("DefaultTexture")
{
	name = "Visualizer";
	componentType = ComponentType::Visualizer;

	// INFO: Load Default Model & Material 
	SetModel("Cube");
	material = AssetHandler::GetMaterial(ShaderType::Unlit);
}

Visualizer::~Visualizer()
{
}

void Visualizer::Serialize(nlohmann::ordered_json& json) const
{
	// INFO: Serialize Parent Class
	Component::Serialize(json);

	auto& jsonBack = json["Components"].back();
	jsonBack["ModelName"] = modelName;
	jsonBack["TextureName"] = textureName;
}

void Visualizer::Deserialize(const nlohmann::ordered_json& json)
{
	// INFO: Deserialize Parent Class
	Component::Deserialize(json);

	// INFO: Deserialize Visualizer Data
	SetModel(json["ModelName"].get<std::string>());
	SetMaterialTexture(json["TextureName"].get<std::string>());
}

void Visualizer::SetModel(const std::string& _modelName)
{
	modelName = _modelName;

	model = AssetHandler::GetModel(modelName);

	if (!model)
	{
		Debug::LogError("Visualizer::SetModel() - Failed to load Visualizer Model: " + modelName);
		modelName = "";
	}
}

void Visualizer::SetMaterialTexture(const std::string& _textureName)
{
	textureName = _textureName;

	material.SetTexture(textureName);
}

void Visualizer::Draw(ID3D11DeviceContext& deviceContext)
{
	if (!model)
	{
		Debug::LogError("Visualizer::Draw() - Model is nullptr");
		return;
	}

	material.Bind(deviceContext);
	model->Draw(deviceContext);
}
