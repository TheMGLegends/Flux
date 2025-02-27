#include "Visualizer.h"

Visualizer::Visualizer() : model(nullptr), material(nullptr)
{
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

void Visualizer::SetModel(const std::filesystem::path& _modelPath)
{
	// TODO: Check if model already exists, if it doesn't, create a new one and get the reference to it
	// and its associated material and set the modelFilepath to the _modelPath otherwise get the reference
	// to the existing model and material and set the modelFilepath to the _modelPath
}

void Visualizer::Draw(ID3D11DeviceContext* deviceContext)
{
	// TODO: Set material properties

	// TODO: Draw call
}
