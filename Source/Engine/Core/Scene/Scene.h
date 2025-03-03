#pragma once

#include "../../Interfaces/ISerializable.h"

#include <memory>
#include <vector>

class GameObject;
class Camera;

class Scene : public ISerializable
{
public:
	Scene();
	~Scene();

	virtual void Serialize(nlohmann::ordered_json& json) const override;
	virtual void Deserialize(const nlohmann::ordered_json& json) override;

	void Start();
	void Update(float deltaTime);
	void LateUpdate(float deltaTime);
	void FixedUpdate(float fixedDeltaTime);

private:
	std::vector<std::unique_ptr<GameObject>> gameObjects;
	std::weak_ptr<Camera> camera;
};

