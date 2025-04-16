#pragma once

#include "Component.h"
#include "Engine/Interfaces/IDebugWireframe.h"

#include <array>
#include <memory>
#include <SimpleMath.h>

#include "Core/Renderer/Material.h"

namespace Flux
{
	class Model;
	class Transform;

	class Camera : public Component, public IDebugWireframe, public std::enable_shared_from_this<Camera>
	{
	public:
		explicit Camera(GameObject* _gameObject);
		~Camera() override;

		void DrawDetails() override;

		void Serialize(nlohmann::flux_json& json) const override;
		void Deserialize(const nlohmann::flux_json& json) override;

		void DrawWireframe(ID3D11DeviceContext& deviceContext, DirectX::PrimitiveBatch<DirectX::VertexPositionColor>& primitiveBatch) override;

		DirectX::XMMATRIX GetViewMatrix() const;
		DirectX::XMMATRIX GetProjectionMatrix() const;

		void SetBackgroundColour(const std::array<float, 4>& _backgroundColour);
		const std::array<float, 4>& GetBackgroundColour() const;

		void SetDrawFrustum(bool _drawFrustum);
		bool ShouldDrawFrustum() const;	

		void SetSkyboxModel(const std::string& modelName);
		void SetMaterialTexture(std::string_view _textureName);

		const std::string& GetSkyboxTextureName() const;

		void SetUseSkybox(bool _useSkybox);
		bool UseSkybox() const;

		void DrawSkybox(ID3D11DeviceContext& deviceContext, const DirectX::XMMATRIX& translation, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection);

	private:
		void SetVerticalFOV(float _verticalFOV);
		void SetNearClippingPlane(float _nearClippingPlane);
		void SetFarClippingPlane(float _farClippingPlane);
		void SetAspectRatio(float screenWidth, float screenHeight);

		void SetFrustum();
		DirectX::XMMATRIX GetAdjustedProjectionMatrix(float _nearClippingPlane, float _farClippingPlane) const;

		void DrawCameraDetails(bool treeOpened);

	private:
		std::weak_ptr<Transform> transform;

		float verticalFOV; // INFO: In degrees
		float nearClippingPlane;
		float farClippingPlane;
		float aspectRatio;
		
		std::array<float, 4> backgroundColour;

		DirectX::BoundingFrustum frustum;
		std::array<DirectX::XMVECTOR, 24> frustumVertices;
		bool shouldDrawFrustum;


		// INFO: Skybox Properties

		Model* skyboxModel;
		Material skyboxMaterial;
		std::string skyboxTextureName;
		bool useSkybox;
	};
}

