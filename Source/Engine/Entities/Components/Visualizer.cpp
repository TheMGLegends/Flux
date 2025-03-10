#include "Visualizer.h"

#include "Core/Configs/DirectXConfig.h"
#include "Core/Debug/Debug.h"
#include "Core/Renderer/AssetHandler.h"
#include "Core/Renderer/Material.h"
#include "Core/Renderer/Model.h"

using namespace Flux;
using namespace Flux::DirectXConfig;

Visualizer::Visualizer(GameObject* _gameObject) : Component(_gameObject), model(nullptr), material(nullptr)
{
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

	// TODO: Serialize VisualizerComponent
}

void Visualizer::Deserialize(const nlohmann::ordered_json& json)
{
	// INFO: Deserialize Parent Class
	Component::Deserialize(json);

	// INFO: Deserialize VisualizerComponent
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

void Visualizer::Draw(ID3D11DeviceContext& deviceContext)
{
	if (!model || !material)
	{
		Debug::LogError("Visualizer::Draw() - Model or Material is nullptr");
		return;
	}

	material->Bind(deviceContext);
	model->Draw(deviceContext);
}
