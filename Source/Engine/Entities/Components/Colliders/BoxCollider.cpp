#include "BoxCollider.h"

#include <DirectXColors.h>
#include <imgui.h>
#include <imgui_internal.h>

#include "Core/Configs/EditorConfig.h"

#include "Engine/Physics/Physics.h"
#include "Engine/Scene/SceneContext.h"
#include "Engine/Entities/GameObjects/GameObject.h"
#include "Engine/Entities/Components/PhysicsBody.h"
#include "Engine/Entities/Components/Transform.h"

using namespace DirectX::SimpleMath;

namespace Flux
{
	BoxCollider::BoxCollider(GameObject* _gameObject) : Collider(_gameObject), size(Vector3::One)
	{
		name = "BoxCollider";
		componentType = ComponentType::BoxCollider;

		// INFO: Create Box Collider Shape
		SetColliderShape();
	}

	BoxCollider::~BoxCollider()
	{
		if (rigidActor)
		{
			rigidActor->release();
			rigidActor = nullptr;
		}
	}

	void BoxCollider::Start()
	{
		Collider::Start();

		SetSize(size);
	}

	void BoxCollider::DrawDetails()
	{
		ImGui::PushID(this);

		bool treeOpened = ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen);

		// INFO: Active Checkbox
		ImGui::SameLine();
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
		if (ImGui::Checkbox("##ComponentActive", &isActive))
		{
			EditorConfig::sceneNeedsSaving = true;
		}
		ImGui::PopStyleVar();

		// INFO: Remove Component Button
		ImVec2 buttonSize = ImVec2(65.0f, 0.0f);
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - (buttonSize.x + 10.0f));
		if (ImGui::Button("Remove", buttonSize))
		{
			GameObject* gameObject = GetGameObject();
			if (gameObject) 
			{
				// INFO: Also remove any associated PhysicsBody
				if (gameObject->HasComponent<PhysicsBody>())
				{
					gameObject->RemoveComponent(gameObject->GetComponent<PhysicsBody>());
					Debug::LogWarning("BoxCollider::DrawDetails() - Removed associated PhysicsBody component");
				}

				gameObject->RemoveComponent(weak_from_this()); 
				EditorConfig::sceneNeedsSaving = true;
			}
		}
		ImGui::PopStyleVar();

		if (treeOpened)
		{
			// INFO: Is Trigger Checkbox
			ImGui::Text("Is Trigger");
			ImGui::SameLine();
			ImGui::SetCursorPosX(136.0f);
			if (ImGui::Checkbox("##IsTrigger", &isTrigger))
			{
				SetIsTrigger(isTrigger);
				EditorConfig::sceneNeedsSaving = true;
			}

			if (DisplayVector3Field("Size", size, 0.1f, "%.1f"))
			{
				UpdateScale();
				EditorConfig::sceneNeedsSaving = true;
			}

			ImGui::TreePop();
		}
		ImGui::PopID();

		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
	}

	void BoxCollider::Serialize(nlohmann::flux_json& json) const
	{
		// INFO: Serialize Parent Class
		Collider::Serialize(json);

		json["Components"].back()["Size"] = { size.x, size.y, size.z };
	}

	void BoxCollider::Deserialize(const nlohmann::flux_json& json)
	{
		// INFO: Deserialize Parent Class
		Collider::Deserialize(json);

		// INFO: Deserialize BoxCollider Data
		size = Vector3(json["Size"][0].get<float>(), json["Size"][1].get<float>(), json["Size"][2].get<float>());
	}

	void BoxCollider::DrawWireframe(ID3D11DeviceContext& deviceContext, DirectX::PrimitiveBatch<DirectX::VertexPositionColor>& primitiveBatch)
	{
		static const DirectX::XMVECTOR vertices[8] =
		{
			{ -1, -1, -1, 0 },
			{ 1, -1, -1, 0 },
			{ 1, -1, 1, 0 },
			{ -1, -1, 1, 0 },
			{ -1, 1, -1, 0 },
			{ 1, 1, -1, 0 },
			{ 1, 1, 1, 0 },
			{ -1, 1, 1, 0 }
		};

		static const unsigned short indices[24] =
		{
			0, 1,
			1, 2,
			2, 3,
			3, 0,
			4, 5,
			5, 6,
			6, 7,
			7, 4,
			0, 4,
			1, 5,
			2, 6,
			3, 7
		};

		std::shared_ptr<Transform> owningTransform = GetGameObject()->transform.lock();

		if (owningTransform)
		{
			DirectX::XMMATRIX world = owningTransform->GetWorldMatrix(size);

			// INFO: Translate the vertices to world space
			DirectX::VertexPositionColor worldVertices[8]{};

			bool isTrigger = IsTrigger();

			for (size_t i = 0; i < 8; i++)
			{
				DirectX::XMStoreFloat3(&worldVertices[i].position, DirectX::XMVector3Transform(vertices[i], world));
				DirectX::XMStoreFloat4(&worldVertices[i].color, isTrigger ? DirectX::Colors::Yellow : DirectX::Colors::LawnGreen);
			}

			primitiveBatch.DrawIndexed(D3D11_PRIMITIVE_TOPOLOGY_LINELIST, indices, 24, worldVertices, 8);
		}
	}

	void BoxCollider::SetColliderShape()
	{
		// INFO: Clear Collider Shape if it already exists
		if (colliderShape)
		{
			colliderShape->release();
			colliderShape = nullptr;
		}

		// INFO: Create Box Collider Shape (Scale Size with Transform Scale)
		Vector3 adjustedSize = GetGameObject()->transform.lock()->GetScale() * size;
		colliderShape = Physics::GetPhysics().createShape(physx::PxBoxGeometry(adjustedSize.x, adjustedSize.y, adjustedSize.z), Physics::GetDefaultPhysicsMaterial(), true);

		// INFO: Default Collisions with everything
		physx::PxFilterData filterData{};
		filterData.word0 = 1;
		filterData.word1 = 1;
		colliderShape->setSimulationFilterData(filterData);

		// INFO: Reset trigger state
		SetIsTrigger(isTrigger);

		// INFO: Ensure Rigid Actor is valid
		if (rigidActor)
		{
			if (!rigidActor->attachShape(*colliderShape))
			{
				Debug::LogError("BoxCollider::BoxCollider() - Failed to attach shape to Rigid Actor");
			}

			SceneContext::GetScene().GetPhysicsScene().addActor(*rigidActor);
		}
	}

	void BoxCollider::UpdateScale()
	{
		if (colliderShape)
		{
			// INFO: Get reference to actor that the shape is attached to
			physx::PxRigidActor* rigidActor = colliderShape->getActor();

			// INFO: Remove the shape from the actor to allow for geometry changes
			rigidActor->detachShape(*colliderShape);

			physx::PxBoxGeometry boxGeometry{};
			if (colliderShape->getBoxGeometry(boxGeometry))
			{
				Vector3 adjustedSize = size * GetGameObject()->transform.lock()->GetScale();
				boxGeometry.halfExtents = { adjustedSize.x, adjustedSize.y, adjustedSize.z };
				colliderShape->setGeometry(boxGeometry);
			}

			// INFO: Re-attach the shape to the actor
			rigidActor->attachShape(*colliderShape);
		}
	}

	void BoxCollider::SetSize(const DirectX::SimpleMath::Vector3& _size)
	{
		if (size != _size) { size = _size; }
		UpdateScale();
	}
}
