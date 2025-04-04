#pragma once

#include "Component.h"
#include "Engine/Interfaces/IDebugWireframe.h"

#include <array>
#include <memory>
#include <SimpleMath.h>

#include "Core/Renderer/Material.h"

namespace Flux
{
	class Transform;
	class Model;

	class Camera : public Component, public IDebugWireframe
	{
	public:
		Camera(GameObject* _gameObject);
		virtual ~Camera() override;

		virtual void DrawDetails() override;

		virtual void Serialize(nlohmann::ordered_json& json) const override;
		virtual void Deserialize(const nlohmann::ordered_json& json) override;

		virtual void DrawWireframe(ID3D11DeviceContext& deviceContext, DirectX::PrimitiveBatch<DirectX::VertexPositionColor>& primitiveBatch) override;

		DirectX::XMMATRIX GetViewMatrix() const;
		DirectX::XMMATRIX GetProjectionMatrix() const;

		inline const std::array<float, 4>& GetBackgroundColour() const { return backgroundColour; }

		inline void SetDrawFrustum(bool _drawFrustum) { shouldDrawFrustum = _drawFrustum; }
		inline bool ShouldDrawFrustum() const { return shouldDrawFrustum; }

		void SetSkyboxModel(const std::string& modelName);
		void SetMaterialTexture(const std::string& _textureName);

		inline void SetUseSkybox(bool _useSkybox) { useSkybox = _useSkybox; }
		inline bool UseSkybox() const { return useSkybox; }

		void DrawSkybox(ID3D11DeviceContext& deviceContext, const DirectX::XMMATRIX& translation, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection);

	private:
		inline void SetVerticalFOV(float _verticalFOV) { verticalFOV = _verticalFOV; }
		inline void SetNearClippingPlane(float _nearClippingPlane) { nearClippingPlane = _nearClippingPlane; }
		inline void SetFarClippingPlane(float _farClippingPlane) { farClippingPlane = _farClippingPlane; }
		inline void SetAspectRatio(float screenWidth, float screenHeight) { aspectRatio = screenWidth / screenHeight; }
		inline void SetBackgroundColour(const std::array<float, 4>& _backgroundColour) { backgroundColour = _backgroundColour; }

		void SetFrustum();
		DirectX::XMMATRIX GetAdjustedProjectionMatrix(float _nearClippingPlane, float _farClippingPlane) const;

	private:
		std::weak_ptr<Transform> transform;

		float verticalFOV; // INFO: In degrees
		float nearClippingPlane;
		float farClippingPlane;
		float aspectRatio;
		
		std::array<float, 4> backgroundColour;

		DirectX::BoundingFrustum frustum;
		DirectX::XMVECTOR frustumVertices[24];
		bool shouldDrawFrustum;


		// INFO: Skybox Properties

		Model* skyboxModel;
		Material skyboxMaterial;
		std::string skyboxTextureName;
		bool useSkybox;
	};
}

