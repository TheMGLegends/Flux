#include "Transform.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <PxRigidDynamic.h>

#include "Core/Configs/EditorConfig.h"

#include "Engine/Entities/GameObjects/GameObject.h"

using namespace DirectX::SimpleMath;

namespace Flux
{
	Transform::Transform(GameObject* _gameObject) : Component(_gameObject), position(Vector3::Zero), rotation(Quaternion::Identity), scale(Vector3::One)
	{
		name = "Transform";
		componentType = ComponentType::Transform;
		isRemoveable = false;
	}

	Transform::~Transform() = default;

	void Transform::DrawDetails()
	{
		ImGui::Dummy(ImVec2(18.0f, 10.0f));

		ImGui::SameLine();
		if (ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow))
		{
			// INFO: Translation Row
			if (DisplayVector3Field("Position", position))
			{
				EditorConfig::SetSceneNeedsSaving(true);
			}

			// INFO: Rotation Row
			Vector3 eulerRotation = rotation.ToEuler();
			eulerRotation.x = DirectX::XMConvertToDegrees(eulerRotation.x);
			eulerRotation.y = DirectX::XMConvertToDegrees(eulerRotation.y);
			eulerRotation.z = DirectX::XMConvertToDegrees(eulerRotation.z);

			if (DisplayVector3Field("Rotation", eulerRotation, 1.0f, "%.1f", -89.0f, 89.0f))
			{
				rotation = Quaternion::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(eulerRotation.y),
															  DirectX::XMConvertToRadians(eulerRotation.x),
															  DirectX::XMConvertToRadians(eulerRotation.z));
				EditorConfig::SetSceneNeedsSaving(true);
			}

			// INFO: Scale Row
			if (DisplayVector3Field("Scale", scale))
			{
				EditorConfig::SetSceneNeedsSaving(true);
			}

			ImGui::TreePop();
		}

		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
	}

	void Transform::Serialize(nlohmann::flux_json& json) const
	{
		// INFO: Serialize Parent Class
		Component::Serialize(json);

		auto& jsonBack = json["Components"].back();
		jsonBack["Position"] = { position.x, position.y, position.z };

		Vector3 eulerRotation = rotation.ToEuler();
		jsonBack["Rotation"] = { DirectX::XMConvertToDegrees(eulerRotation.x),
								 DirectX::XMConvertToDegrees(eulerRotation.y),
								 DirectX::XMConvertToDegrees(eulerRotation.z) }; // INFO: Rotation Values saved in degrees for readability
		jsonBack["Scale"] = { scale.x, scale.y, scale.z };
	}

	void Transform::Deserialize(const nlohmann::flux_json& json)
	{
		// INFO: Deserialize Parent Class
		Component::Deserialize(json);

		// INFO: Deserialize Transform Data
		if (json.contains("Position"))
		{
			position = Vector3(json["Position"][0].get<float>(), json["Position"][1].get<float>(), json["Position"][2].get<float>());
		}

		if (json.contains("Rotation"))
		{
			rotation = Quaternion::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(json["Rotation"][1].get<float>()),
														  DirectX::XMConvertToRadians(json["Rotation"][0].get<float>()),
														  DirectX::XMConvertToRadians(json["Rotation"][2].get<float>()));
		}

		if (json.contains("Scale"))
		{
			scale = Vector3(json["Scale"][0].get<float>(), json["Scale"][1].get<float>(), json["Scale"][2].get<float>());
		}
	}

	DirectX::XMMATRIX Transform::GetWorldMatrix() const
	{
		DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslationFromVector(position);
		DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationQuaternion(rotation);
		DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScalingFromVector(scale);

		return scaleMatrix * rotationMatrix * translationMatrix;
	}

	DirectX::XMMATRIX Transform::GetWorldMatrix(const Vector3& offsetScale) const
	{
		DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslationFromVector(position);

		Vector3 adjustedScale = scale * offsetScale;
		DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScalingFromVector(adjustedScale);
		DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationQuaternion(rotation);

		return scaleMatrix * rotationMatrix * translationMatrix;
	}

	DirectX::XMMATRIX Transform::GetWorldMatrix(float customUniformScale) const
	{
		DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslationFromVector(position);

		auto customScale = Vector3(customUniformScale, customUniformScale, customUniformScale);
		DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScalingFromVector(customScale);
		DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationQuaternion(rotation);

		return scaleMatrix * rotationMatrix * translationMatrix;
	}

	Vector3 Transform::Forward() const
	{
		return Vector3::Transform(Vector3::Forward, rotation);
	}

	Vector3 Transform::Right() const
	{
		return Vector3::Transform(Vector3::Right, rotation);
	}

	Vector3 Transform::Up() const
	{
		return Vector3::Transform(Vector3::Up, rotation);
	}

	void Transform::SetPosition(const Vector3& _position)
	{
		position = _position;
	}

	const Vector3& Transform::GetPosition() const
	{
		return position;
	}

	void Transform::SetRotation(const Quaternion& _rotation)
	{
		rotation = _rotation;
	}

	const Quaternion& Transform::GetRotation() const
	{
		return rotation;
	}

	void Transform::SetScale(const Vector3& _scale)
	{
		scale = _scale;
	}

	const Vector3& Transform::GetScale() const
	{
		return scale;
	}

	void Transform::Rotate(const Vector3& eulerRotation)
	{
		Quaternion rotationQuaternion = Quaternion::CreateFromYawPitchRoll(eulerRotation.y, eulerRotation.x, eulerRotation.z);

		// INFO: Combine the current rotation with the new rotation
		rotation *= rotationQuaternion;
	}

	void Transform::Translate(const Vector3& translation)
	{
		position += translation;
	}
}
