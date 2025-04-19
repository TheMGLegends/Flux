#include "PhysicsBody.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <magic_enum.hpp>
#include <extensions/PxRigidBodyExt.h>

#include "Core/Configs/EditorConfig.h"

#include "Core/Debug/Debug.h"

#include "Engine/Physics/Physics.h"
#include "Engine/Scene/SceneContext.h"
#include "Engine/Entities/Components/Transform.h"
#include "Engine/Entities/Components/Colliders/Collider.h"
#include "Engine/Entities/GameObjects/GameObject.h"

using namespace DirectX::SimpleMath;

namespace Flux
{
	PhysicsBody::PhysicsBody(GameObject* _gameObject) : Component(_gameObject), mass(1.0f), drag(0.0f), angularDrag(0.05f), 
														useGravity(true), isKinematic(false)
	{
		name = "PhysicsBody";
		componentType = ComponentType::PhysicsBody;

		// INFO: No constraints by default
		for (size_t i = 0; i < static_cast<size_t>(ConstraintAxis::Count); ++i)
		{
			positionConstraints[i] = false;
			rotationConstraints[i] = false;
		}
	}

	PhysicsBody::~PhysicsBody() = default;

	void PhysicsBody::PostConstruction()
	{
		GameObject* gameObject = GetGameObject();

		if (gameObject)
		{
			// INFO: Add Default BoxCollider if no Collider is found
			if (!gameObject->HasComponent<Collider>())
			{
				Debug::LogWarning("PhysicsBody::PhysicsBody() - No collider found on GameObject, adding a default BoxCollider");
				attachedCollider = gameObject->AddComponent<BoxCollider>(gameObject);
			}
			else
			{
				attachedCollider = gameObject->GetComponent<Collider>();

				// INFO: If collider was already present, it defaulted to static so we need to set it to dynamic
				if (!attachedCollider.expired())
				{
					std::shared_ptr<Collider> collider = attachedCollider.lock();

					// INFO: Reset the Rigid Actor to be dynamic
					collider->SetRigidActor();

					// INFO: Reset the Collider Shape to be attached to the new Rigid Actor
					collider->SetColliderShape();
				}
			}
		}
	}

	void PhysicsBody::PostDestruction()
	{
		// INFO: Set the attached collider to be static if it is valid
		if (std::shared_ptr<Collider> collider = attachedCollider.lock())
		{
			// INFO: Reset the rigid actor to be static
			collider->SetRigidActor();

			// INFO: Reset the collider shape to be attached to the new rigid actor
			collider->SetColliderShape();
		}
	}

	void PhysicsBody::Start()
	{
		// INFO: Initialise rigid actor with values from the PhysicsBody ready for simulation
		SetIsActive(isActive);
		SetMass(mass);
		SetDrag(drag);
		SetAngularDrag(angularDrag);
		SetUseGravity(useGravity);
		SetIsKinematic(isKinematic);

		for (size_t i = 0; i < static_cast<size_t>(ConstraintAxis::Count); ++i)
		{
			SetPositionConstraint(positionConstraints[i], static_cast<ConstraintAxis>(i));
			SetRotationConstraint(rotationConstraints[i], static_cast<ConstraintAxis>(i));
		}
	}

	void PhysicsBody::DrawDetails()
	{
		ImGui::PushID(this);

		// INFO: Active Checkbox
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
		if (ImGui::Checkbox("##ComponentActive", &isActive))
		{
			EditorConfig::SetSceneNeedsSaving(true);
		}

		ImGui::SameLine();
		bool treeOpened = ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow);

		// INFO: Remove Component Button
		ImVec2 buttonSize = ImVec2(65.0f, 0.0f);
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - (buttonSize.x + 10.0f));
		if (ImGui::Button("Remove", buttonSize))
		{
			GameObject* gameObject = GetGameObject();
			if (gameObject) 
			{ 
				gameObject->RemoveComponent(weak_from_this());
				EditorConfig::SetSceneNeedsSaving(true);
			}
		}
		ImGui::PopStyleVar(2);

		if (treeOpened)
		{
			// INFO: Mass InputField
			ImGui::Text("Mass");
			ImGui::SameLine();
			ImGui::SetCursorPosX(136.0f);
			ImGui::SetNextItemWidth(50.0f);
			if (ImGui::InputFloat("##Radius", &mass, 0.0f, 0.0f, "%.1f"))
			{
				EditorConfig::SetSceneNeedsSaving(true);
			}

			// INFO: Drag InputField
			ImGui::Text("Drag");
			ImGui::SameLine();
			ImGui::SetCursorPosX(136.0f);
			ImGui::SetNextItemWidth(50.0f);
			if (ImGui::InputFloat("##Drag", &drag, 0.0f, 0.0f, "%.2f"))
			{
				EditorConfig::SetSceneNeedsSaving(true);
			}

			// INFO: Angular Drag InputField
			ImGui::Text("Angular Drag");
			ImGui::SameLine();
			ImGui::SetCursorPosX(136.0f);
			ImGui::SetNextItemWidth(50.0f);
			if (ImGui::InputFloat("##AngularDrag", &angularDrag, 0.0f, 0.0f, "%.2f"))
			{
				EditorConfig::SetSceneNeedsSaving(true);
			}

			// INFO: Use Gravity Checkbox
			ImGui::Text("Use Gravity");
			ImGui::SameLine();
			ImGui::SetCursorPosX(136.0f);
			if (ImGui::Checkbox("##UseGravity", &useGravity))
			{
				EditorConfig::SetSceneNeedsSaving(true);
			}

			// INFO: Is Kinematic Checkbox
			ImGui::Text("Is Kinematic");
			ImGui::SameLine();
			ImGui::SetCursorPosX(136.0f);
			if (ImGui::Checkbox("##IsKinematic", &isKinematic))
			{
				EditorConfig::SetSceneNeedsSaving(true);
			}

			// INFO: Constraints
			ImGui::Text("Constraints");
			if (DisplayArray3Field("Freeze Position", &positionConstraints[0]))
			{
				EditorConfig::SetSceneNeedsSaving(true);
			}

			if (DisplayArray3Field("Freeze Rotation", &rotationConstraints[0]))
			{
				EditorConfig::SetSceneNeedsSaving(true);
			}

			ImGui::TreePop();
		}
		ImGui::PopID();

		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
	}

	void PhysicsBody::Serialize(nlohmann::flux_json& json) const
	{
		// INFO: Serialize Parent Class
		Component::Serialize(json);

		auto& jsonBack = json["Components"].back();
		jsonBack["Mass"] = mass;
		jsonBack["Drag"] = drag;
		jsonBack["AngularDrag"] = angularDrag;
		jsonBack["UseGravity"] = useGravity;
		jsonBack["IsKinematic"] = isKinematic;
		jsonBack["PositionConstraints"] = { positionConstraints[0], positionConstraints[1], positionConstraints[2] };
		jsonBack["RotationConstraints"] = { rotationConstraints[0], rotationConstraints[1], rotationConstraints[2] };
	}

	void PhysicsBody::Deserialize(const nlohmann::flux_json& json)
	{
		// INFO: Deserialize Parent Class
		Component::Deserialize(json);

		// INFO: Deserialize PhysicsBody Data
		mass = json["Mass"].get<float>();
		drag = json["Drag"].get<float>();
		angularDrag = json["AngularDrag"].get<float>();
		useGravity = json["UseGravity"].get<bool>();
		isKinematic = json["IsKinematic"].get<bool>();

		auto& positionConstraintsJson = json["PositionConstraints"];
		for (size_t i = 0; i < positionConstraintsJson.size(); ++i)
		{
			positionConstraints[i] = positionConstraintsJson[i].get<bool>();
		}

		auto& rotationConstraintsJson = json["RotationConstraints"];
		for (size_t i = 0; i < rotationConstraintsJson.size(); ++i)
		{
			rotationConstraints[i] = rotationConstraintsJson[i].get<bool>();
		}
	}

	void PhysicsBody::SetIsActive(bool _isActive)
	{
		Component::SetIsActive(_isActive);

		// INFO: Reset the Rigid Actor of the attached collider to be the new type based on active state
		if (std::shared_ptr<Collider> collider = attachedCollider.lock())
		{
			collider->SetRigidActor();
			collider->SetColliderShape(); // INFO: Reset the Collider Shape to be attached to the new Rigid Actor
		}
	}

	void PhysicsBody::AddForce(const Vector3& force, physx::PxForceMode::Enum forceMode)
	{
		physx::PxRigidDynamic* rigidDynamic = VerifyRigidActor();

		// INFO: Check to see if the rigid body is not kinematic
		if (rigidDynamic && !rigidDynamic->getRigidBodyFlags().isSet(physx::PxRigidBodyFlag::eKINEMATIC))
		{
			physx::PxVec3 physxForce(force.x, force.y, force.z);
			rigidDynamic->addForce(physxForce, forceMode);
		}
	}

	void PhysicsBody::AddTorque(const DirectX::SimpleMath::Vector3& torque, physx::PxForceMode::Enum forceMode)
	{
		physx::PxRigidDynamic* rigidDynamic = VerifyRigidActor();

		// INFO: Check to see if the rigid body is not kinematic
		if (rigidDynamic && !rigidDynamic->getRigidBodyFlags().isSet(physx::PxRigidBodyFlag::eKINEMATIC))
		{
			physx::PxVec3 physxTorque(torque.x, torque.y, torque.z);
			rigidDynamic->addTorque(physxTorque, forceMode);
		}
	}

	void PhysicsBody::SetMass(float _mass)
	{
		if (mass != _mass) { mass = _mass; }

		physx::PxRigidDynamic* rigidDynamic = VerifyRigidActor();

		if (rigidDynamic)
		{
			physx::PxRigidBodyExt::updateMassAndInertia(*rigidDynamic, mass);
		}
	}

	float PhysicsBody::GetMass() const
	{
		return mass;
	}

	void PhysicsBody::SetDrag(float _drag)
	{
		if (drag != _drag) { drag = _drag; }

		physx::PxRigidDynamic* rigidDynamic = VerifyRigidActor();

		if (rigidDynamic)
		{
			rigidDynamic->setLinearDamping(drag);
		}
	}

	float PhysicsBody::GetDrag() const
	{
		return drag;
	}

	void PhysicsBody::SetAngularDrag(float _angularDrag)
	{
		if (angularDrag != _angularDrag) { angularDrag = _angularDrag; }

		physx::PxRigidDynamic* rigidDynamic = VerifyRigidActor();

		if (rigidDynamic)
		{
			rigidDynamic->setAngularDamping(angularDrag);
		}
	}

	float PhysicsBody::GetAngularDrag() const
	{
		return angularDrag;
	}

	void PhysicsBody::SetUseGravity(bool _useGravity)
	{
		if (useGravity != _useGravity) { useGravity = _useGravity; }

		physx::PxRigidDynamic* rigidDynamic = VerifyRigidActor();

		if (rigidDynamic)
		{
			rigidDynamic->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, !useGravity);
		}
	}

	bool PhysicsBody::UsesGravity() const
	{
		return useGravity;
	}

	void PhysicsBody::SetIsKinematic(bool _isKinematic)
	{
		if (isKinematic != _isKinematic) { isKinematic = _isKinematic; }

		if (std::shared_ptr<Collider> collider = attachedCollider.lock())
		{
			collider->SetIsKinematic(isKinematic);
		}
	}

	bool PhysicsBody::IsKinematic() const
	{
		return isKinematic;
	}

	void PhysicsBody::SetPositionConstraint(bool isConstrained, ConstraintAxis axis)
	{
		if (axis == ConstraintAxis::Count)
		{
			Debug::LogError("PhysicsBody::SetPositionConstraint() - Invalid Constraint Axis: " + std::string{ magic_enum::enum_name(axis) });
			return;
		}

		if (positionConstraints[static_cast<size_t>(axis)] != isConstrained)
		{
			positionConstraints[static_cast<size_t>(axis)] = isConstrained;
		}

		physx::PxRigidDynamic* rigidDynamic = VerifyRigidActor();

		if (rigidDynamic)
		{
			rigidDynamic->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X, positionConstraints[0]); // INFO: X-Axis
			rigidDynamic->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, positionConstraints[1]); // INFO: Y-Axis
			rigidDynamic->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, positionConstraints[2]); // INFO: Z-Axis
		}
	}

	bool PhysicsBody::IsPositionConstrained(ConstraintAxis axis) const
	{
		return positionConstraints[static_cast<size_t>(axis)];
	}

	void PhysicsBody::SetRotationConstraint(bool isConstrained, ConstraintAxis axis)
	{
		if (axis == ConstraintAxis::Count)
		{
			Debug::LogError("PhysicsBody::SetRotationConstraint() - Invalid Constraint Axis: " + std::string{ magic_enum::enum_name(axis) });
			return;
		}

		if (rotationConstraints[static_cast<size_t>(axis)] != isConstrained)
		{
			rotationConstraints[static_cast<size_t>(axis)] = isConstrained;
		}

		physx::PxRigidDynamic* rigidDynamic = VerifyRigidActor();

		if (rigidDynamic)
		{
			rigidDynamic->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, rotationConstraints[0]); // INFO: X-Axis
			rigidDynamic->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, rotationConstraints[1]); // INFO: Y-Axis
			rigidDynamic->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, rotationConstraints[2]); // INFO: Z-Axis
		}
	}

	bool PhysicsBody::IsRotationConstrained(ConstraintAxis axis) const
	{
		return rotationConstraints[static_cast<size_t>(axis)];
	}

	physx::PxRigidDynamic* PhysicsBody::VerifyRigidActor()
	{
		if (std::shared_ptr<Collider> collider = attachedCollider.lock())
		{
			RigidActorType actorType = collider->GetRigidActorType();

			if (actorType == RigidActorType::Dynamic)
			{
				return static_cast<physx::PxRigidDynamic*>(collider->GetRigidActor());
			}
			else if (actorType == RigidActorType::Static)
			{
				return nullptr;
			}
		}

		Debug::LogError("PhysicsBody::VerifyRigidActor() - No collider found");
		return nullptr;
	}

	bool PhysicsBody::DisplayArray3Field(const char* label, bool* array) const
	{
		bool changed = false;

		ImGui::SetCursorPosX(50.0f);
		if (ImGui::BeginTable(label, 2))
		{
			ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 125.0f); // INFO: Label Column
			ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 300.0f); // INFO: Value Column

			ImGui::TableNextColumn();
			ImGui::AlignTextToFramePadding();
			ImGui::Text(label);

			ImGui::TableNextColumn();
			ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());

			// INFO: X Value
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.75f, 0.0f, 0.0f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.75f, 0.0f, 0.0f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.75f, 0.0f, 0.0f, 1.0f));

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
			ImGui::Button("X");
			ImGui::PopStyleColor(3);
			ImGui::SameLine();
			if (ImGui::Checkbox("##X", &array[0]))
			{
				changed = true;
			}
			ImGui::PopStyleVar();
			ImGui::PopItemWidth();
			ImGui::SameLine();

			// INFO: Y Value
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.75f, 0.0f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.75f, 0.0f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.75f, 0.0f, 1.0f));

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
			ImGui::Button("Y");
			ImGui::PopStyleColor(3);
			ImGui::SameLine();
			if (ImGui::Checkbox("##Y", &array[1]))
			{
				changed = true;
			}
			ImGui::PopStyleVar();
			ImGui::PopItemWidth();
			ImGui::SameLine();

			// INFO: Z Value
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.75f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.0f, 0.75f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.0f, 0.75f, 1.0f));

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
			ImGui::Button("Z");
			ImGui::PopStyleColor(3);
			ImGui::SameLine();
			if (ImGui::Checkbox("##Z", &array[2]))
			{
				changed = true;
			}
			ImGui::PopStyleVar();
			ImGui::PopItemWidth();

			ImGui::EndTable();
		}

		return changed;
	}
}
