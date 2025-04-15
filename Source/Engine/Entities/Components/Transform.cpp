#include "Transform.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <PxRigidDynamic.h>

#include "PhysicsBody.h"
#include "Colliders/Collider.h"

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

	Transform::~Transform()
	{
	}

	void Transform::DrawDetails()
	{
		if (ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			// INFO: Translation Row
			if (DisplayVector3Field("Position", position))
			{
				SetPositionEditor(position);
				EditorConfig::sceneNeedsSaving = true;
			}

			// INFO: Rotation Row
			Vector3 eulerRotation = rotation.ToEuler();
			eulerRotation.x = DirectX::XMConvertToDegrees(eulerRotation.x);
			eulerRotation.y = DirectX::XMConvertToDegrees(eulerRotation.y);
			eulerRotation.z = DirectX::XMConvertToDegrees(eulerRotation.z);

			if (DisplayVector3Field("Rotation", eulerRotation, 1.0f, "%.1f"))
			{
				SetRotationEditor(Quaternion::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(eulerRotation.y),
					DirectX::XMConvertToRadians(eulerRotation.x),
					DirectX::XMConvertToRadians(eulerRotation.z)));
				EditorConfig::sceneNeedsSaving = true;
			}

			// INFO: Scale Row
			if (DisplayVector3Field("Scale", scale))
			{
				SetScale(scale);
				EditorConfig::sceneNeedsSaving = true;
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
		position = Vector3(json["Position"][0].get<float>(), json["Position"][1].get<float>(), json["Position"][2].get<float>());
		rotation = Quaternion::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(json["Rotation"][1].get<float>()),
													  DirectX::XMConvertToRadians(json["Rotation"][0].get<float>()),
													  DirectX::XMConvertToRadians(json["Rotation"][2].get<float>()));
		scale = Vector3(json["Scale"][0].get<float>(), json["Scale"][1].get<float>(), json["Scale"][2].get<float>());
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

	void Transform::SetScale(const DirectX::SimpleMath::Vector3& _scale)
	{
		scale = _scale;

		// INFO: Make collider bounds match the transform scale + offset scale of the collider
		if (GetGameObject()->HasComponent<Collider>())
		{
			if (std::shared_ptr<Collider> collider = GetGameObject()->GetComponent<Collider>().lock())
			{
				collider->UpdateScale();
			}
		}
	}

	void Transform::Rotate(const Vector3& eulerRotation)
	{
		Quaternion rotationQuaternion = Quaternion::CreateFromYawPitchRoll(eulerRotation.y, eulerRotation.x, eulerRotation.z);

		// INFO: Combine the current rotation with the new rotation
		rotation *= rotationQuaternion;
	}

	void Transform::SetPositionEditor(const DirectX::SimpleMath::Vector3& _position)
	{
		position = _position;

		// INFO: If game object has a physics body we need to setGlobalPose
		if (GetGameObject()->HasComponent<PhysicsBody>())
		{
			physx::PxRigidDynamic* rigidDynamic = GetGameObject()->GetComponent<PhysicsBody>().lock()->VerifyRigidActor();

			if (rigidDynamic)
			{
				rigidDynamic->setGlobalPose(physx::PxTransform(physx::PxVec3(position.x, position.y, position.z),
					physx::PxQuat(rotation.x, rotation.y, rotation.z, rotation.w)));
			}
		}
	}

	void Transform::SetRotationEditor(const DirectX::SimpleMath::Quaternion& _rotation)
	{
		rotation = _rotation;

		// INFO: If game object has a physics body we need to setGlobalPose
		if (GetGameObject()->HasComponent<PhysicsBody>())
		{
			physx::PxRigidDynamic* rigidDynamic = GetGameObject()->GetComponent<PhysicsBody>().lock()->VerifyRigidActor();
			if (rigidDynamic)
			{
				rigidDynamic->setGlobalPose(physx::PxTransform(physx::PxVec3(position.x, position.y, position.z),
					physx::PxQuat(rotation.x, rotation.y, rotation.z, rotation.w)));
			}
		}
	}
}
