#include "PhysicsBody.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <magic_enum.hpp>
#include <extensions/PxRigidBodyExt.h>

#include "Core/Debug/Debug.h"

#include "Engine/Physics/Physics.h"
#include "Engine/Scene/SceneContext.h"
#include "Engine/Entities/Components/Transform.h"
#include "Engine/Entities/Components/Colliders/Collider.h"
#include "Engine/Entities/GameObjects/GameObject.h"

using namespace DirectX::SimpleMath;

namespace Flux
{
	PhysicsBody::PhysicsBody(GameObject* _gameObject) : Component(_gameObject), mass(1.0f), drag(0.0f), angularDrag(0.05f), useGravity(true)
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

	PhysicsBody::~PhysicsBody()
	{
	}

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

			// INFO: Set RigidActor Properties
			SetMass(mass);
			SetDrag(drag);
			SetAngularDrag(angularDrag);
		}
	}

	void PhysicsBody::DrawDetails()
	{
		ImGui::PushID(this);

		bool treeOpened = ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen);

		// INFO: Active Checkbox
		ImGui::SameLine();
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
		ImGui::Checkbox("##ComponentActive", &isActive);
		ImGui::PopStyleVar();

		// INFO: Remove Component Button
		ImVec2 buttonSize = ImVec2(65.0f, 0.0f);
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - (buttonSize.x + 10.0f));
		if (ImGui::Button("Remove", buttonSize))
		{
			GameObject* gameObject = GetGameObject();
			if (gameObject) { gameObject->RemoveComponent(weak_from_this()); }
		}

		if (treeOpened)
		{
			// INFO: Mass InputField
			ImGui::Text("Mass");
			ImGui::SameLine();
			ImGui::SetCursorPosX(136.0f);
			ImGui::SetNextItemWidth(50.0f);
			if (ImGui::InputFloat("##Radius", &mass, 0.0f, 0.0f, "%.1f"))
			{
				SetMass(mass);
			}

			// INFO: Drag InputField
			ImGui::Text("Drag");
			ImGui::SameLine();
			ImGui::SetCursorPosX(136.0f);
			ImGui::SetNextItemWidth(50.0f);
			if (ImGui::InputFloat("##Drag", &drag, 0.0f, 0.0f, "%.2f"))
			{
				SetDrag(drag);
			}

			// INFO: Angular Drag InputField
			ImGui::Text("Angular Drag");
			ImGui::SameLine();
			ImGui::SetCursorPosX(136.0f);
			ImGui::SetNextItemWidth(50.0f);
			if (ImGui::InputFloat("##AngularDrag", &angularDrag, 0.0f, 0.0f, "%.2f"))
			{
				SetAngularDrag(angularDrag);
			}

			// INFO: Use Gravity Checkbox
			ImGui::Text("Use Gravity");
			ImGui::SameLine();
			ImGui::SetCursorPosX(136.0f);
			if (ImGui::Checkbox("##UseGravity", &useGravity))
			{
				SetUseGravity(useGravity);
			}

			// INFO: Constraints
			ImGui::Text("Constraints");
			if (DisplayArray3Field("Freeze Position", &positionConstraints[0]))
			{
				SetPositionConstraint(positionConstraints[0], ConstraintAxis::X);
				SetPositionConstraint(positionConstraints[1], ConstraintAxis::Y);
				SetPositionConstraint(positionConstraints[2], ConstraintAxis::Z);
			}

			if (DisplayArray3Field("Freeze Rotation", &rotationConstraints[0]))
			{
				SetRotationConstraint(rotationConstraints[0], ConstraintAxis::X);
				SetRotationConstraint(rotationConstraints[1], ConstraintAxis::Y);
				SetRotationConstraint(rotationConstraints[2], ConstraintAxis::Z);
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
		jsonBack["PositionConstraints"] = { positionConstraints[0], positionConstraints[1], positionConstraints[2] };
		jsonBack["RotationConstraints"] = { rotationConstraints[0], rotationConstraints[1], rotationConstraints[2] };
	}

	void PhysicsBody::Deserialize(const nlohmann::flux_json& json)
	{
		// INFO: Deserialize Parent Class
		Component::Deserialize(json);

		// INFO: Deserialize PhysicsBody Data
		SetMass(json["Mass"].get<float>());
		SetDrag(json["Drag"].get<float>());
		SetAngularDrag(json["AngularDrag"].get<float>());
		SetUseGravity(json["UseGravity"].get<bool>());

		auto& positionConstraintsJson = json["PositionConstraints"];
		for (size_t i = 0; i < positionConstraintsJson.size(); ++i)
		{
			SetPositionConstraint(positionConstraintsJson[i].get<bool>(), static_cast<ConstraintAxis>(i));
		}

		auto& rotationConstraintsJson = json["RotationConstraints"];
		for (size_t i = 0; i < rotationConstraintsJson.size(); ++i)
		{
			SetRotationConstraint(rotationConstraintsJson[i].get<bool>(), static_cast<ConstraintAxis>(i));
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

		if (rigidDynamic)
		{
			// INFO: Check to see if the rigid body is not kinematic
			if (!rigidDynamic->getRigidBodyFlags().isSet(physx::PxRigidBodyFlag::eKINEMATIC))
			{
				physx::PxVec3 physxForce(force.x, force.y, force.z);
				rigidDynamic->addForce(physxForce, forceMode);
			}
		}
	}

	void PhysicsBody::SetMass(float _mass)
	{
		mass = _mass;

		physx::PxRigidDynamic* rigidDynamic = VerifyRigidActor();

		if (rigidDynamic)
		{
			physx::PxRigidBodyExt::updateMassAndInertia(*rigidDynamic, mass);
		}
	}

	void PhysicsBody::SetDrag(float _drag)
	{
		drag = _drag;

		physx::PxRigidDynamic* rigidDynamic = VerifyRigidActor();

		if (rigidDynamic)
		{
			rigidDynamic->setLinearDamping(drag);
		}
	}

	void PhysicsBody::SetAngularDrag(float _angularDrag)
	{
		angularDrag = _angularDrag;

		physx::PxRigidDynamic* rigidDynamic = VerifyRigidActor();

		if (rigidDynamic)
		{
			rigidDynamic->setAngularDamping(angularDrag);
		}
	}

	void PhysicsBody::SetUseGravity(bool _useGravity)
	{
		useGravity = _useGravity;

		physx::PxRigidDynamic* rigidDynamic = VerifyRigidActor();

		if (rigidDynamic)
		{
			rigidDynamic->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, !useGravity);
		}
	}

	void PhysicsBody::SetPositionConstraint(bool isConstrained, ConstraintAxis axis)
	{
		if (axis == ConstraintAxis::Count)
		{
			Debug::LogError("PhysicsBody::SetPositionConstraint() - Invalid Constraint Axis: " + std::string{ magic_enum::enum_name(axis) });
			return;
		}

		positionConstraints[static_cast<size_t>(axis)] = isConstrained;

		physx::PxRigidDynamic* rigidDynamic = VerifyRigidActor();

		if (rigidDynamic)
		{
			rigidDynamic->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X, positionConstraints[0]); // INFO: X-Axis
			rigidDynamic->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, positionConstraints[1]); // INFO: Y-Axis
			rigidDynamic->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, positionConstraints[2]); // INFO: Z-Axis
		}
	}

	void PhysicsBody::SetRotationConstraint(bool isConstrained, ConstraintAxis axis)
	{
		if (axis == ConstraintAxis::Count)
		{
			Debug::LogError("PhysicsBody::SetRotationConstraint() - Invalid Constraint Axis: " + std::string{ magic_enum::enum_name(axis) });
			return;
		}

		rotationConstraints[static_cast<size_t>(axis)] = isConstrained;

		physx::PxRigidDynamic* rigidDynamic = VerifyRigidActor();

		if (rigidDynamic)
		{
			rigidDynamic->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, rotationConstraints[0]); // INFO: X-Axis
			rigidDynamic->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, rotationConstraints[1]); // INFO: Y-Axis
			rigidDynamic->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, rotationConstraints[2]); // INFO: Z-Axis
		}
	}

	physx::PxRigidDynamic* PhysicsBody::VerifyRigidActor()
	{
		if (std::shared_ptr<Collider> collider = attachedCollider.lock())
		{
			if (collider->GetRigidActorType() == RigidActorType::Dynamic)
			{
				return static_cast<physx::PxRigidDynamic*>(collider->GetRigidActor());
			}
		}

		Debug::LogError("PhysicsBody::VerifyRigidActor() - No Collider found or Collider is not dynamic");
		return nullptr;
	}

	bool PhysicsBody::DisplayArray3Field(const char* label, bool* array)
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
