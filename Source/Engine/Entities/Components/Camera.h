#pragma once

#include "Component.h"
#include "../../Interfaces/IDebugWireframe.h"

#include <array>
#include <memory>
#include <SimpleMath.h>

class Transform;
class Material;
class Model;

class Camera : public Component, public IDebugWireframe
{
public:
	Camera();
	virtual ~Camera() override;

	virtual void Serialize(nlohmann::ordered_json& json) const override;
	virtual void Deserialize(const nlohmann::ordered_json& json) override;

	virtual void DrawWireframe(ID3D11DeviceContext* deviceContext) override;

	DirectX::XMMATRIX GetViewMatrix() const;
	DirectX::XMMATRIX GetProjectionMatrix() const;

	DirectX::SimpleMath::Vector3 Forward() const;
	DirectX::SimpleMath::Vector3 Right() const;
	DirectX::SimpleMath::Vector3 Up() const;

	inline void SetRotation(const DirectX::SimpleMath::Quaternion& _rotation) { rotation = _rotation; }
	inline const DirectX::SimpleMath::Quaternion& GetRotation() const { return rotation; }

	void DrawSkybox(ID3D11DeviceContext& deviceContext, const DirectX::XMMATRIX& translation, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection);

private:
	inline void SetVerticalFOV(float _verticalFOV) { verticalFOV = _verticalFOV; }
	inline void SetNearClippingPlane(float _nearClippingPlane) { nearClippingPlane = _nearClippingPlane; }
	inline void SetFarClippingPlane(float _farClippingPlane) { farClippingPlane = _farClippingPlane; }
	inline void SetAspectRatio(float screenWidth, float screenHeight) { aspectRatio = screenWidth / screenHeight; }
	inline void SetBackgroundColour(const std::array<float, 4>& _backgroundColour) { backgroundColour = _backgroundColour; }

private:
	std::weak_ptr<Transform> transform;

	DirectX::SimpleMath::Quaternion rotation;

	float verticalFOV; // INFO: In degrees
	float nearClippingPlane;
	float farClippingPlane;
	float aspectRatio;

	std::array<float, 4> backgroundColour;

	// INFO: Skybox Members
	Model* skyboxModel;
	Material* skyboxMaterial;
	bool useSkybox;
};

