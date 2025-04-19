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
				// INFO: Also remove any associated PhysicsBody
				if (gameObject->HasComponent<PhysicsBody>())
				{
					gameObject->RemoveComponent(gameObject->GetComponent<PhysicsBody>());
					Debug::LogWarning("BoxCollider::DrawDetails() - Removed associated PhysicsBody component");
				}

				gameObject->RemoveComponent(weak_from_this()); 
				EditorConfig::SetSceneNeedsSaving(true);
			}
		}
		ImGui::PopStyleVar(2);

		if (treeOpened)
		{
			// INFO: Is Trigger Checkbox
			ImGui::Text("Is Trigger");
			ImGui::SameLine();
			ImGui::SetCursorPosX(136.0f);
			if (ImGui::Checkbox("##IsTrigger", &isTrigger))
			{
				SetIsTrigger(isTrigger);
				EditorConfig::SetSceneNeedsSaving(true);
			}

			if (DisplayVector3Field("Size", size, 0.1f, "%.1f"))
			{
				UpdateScale();
				EditorConfig::SetSceneNeedsSaving(true);
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
		static const std::array<DirectX::XMVECTOR, 8> vertices =
		{
			DirectX::XMVECTOR{ -1, -1, -1, 0 },
			DirectX::XMVECTOR{ 1, -1, -1, 0 },
			DirectX::XMVECTOR{ 1, -1, 1, 0 },
			DirectX::XMVECTOR{ -1, -1, 1, 0 },
			DirectX::XMVECTOR{ -1, 1, -1, 0 },
			DirectX::XMVECTOR{ 1, 1, -1, 0 },
			DirectX::XMVECTOR{ 1, 1, 1, 0 },
			DirectX::XMVECTOR{ -1, 1, 1, 0 }
		};

		static const std::array<unsigned short, 24> indices =
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
			std::array<DirectX::VertexPositionColor, 8> worldVertices{};

			bool isTrigger = IsTrigger();

			for (size_t i = 0; i < 8; i++)
			{
				DirectX::XMStoreFloat3(&worldVertices[i].position, DirectX::XMVector3Transform(vertices[i], world));
				DirectX::XMStoreFloat4(&worldVertices[i].color, isTrigger ? DirectX::Colors::Yellow : DirectX::Colors::LawnGreen);
			}

			primitiveBatch.DrawIndexed(D3D11_PRIMITIVE_TOPOLOGY_LINELIST, &indices[0], 24, &worldVertices[0], 8);
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

		// INFO: Ensure Rigid Actor is valid
		if (rigidActor)
		{
			if (!rigidActor->attachShape(*colliderShape))
			{
				Debug::LogError("BoxCollider::BoxCollider() - Failed to attach shape to Rigid Actor");
			}

			SceneContext::GetScene().GetPhysicsScene().addActor(*rigidActor);

			// INFO: Reset active and trigger states
			SetIsActive(isActive);
			SetIsTrigger(isTrigger);
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

			if (physx::PxBoxGeometry boxGeometry{}; colliderShape->getBoxGeometry(boxGeometry))
			{
				Vector3 adjustedSize = size * GetGameObject()->transform.lock()->GetScale();
				boxGeometry.halfExtents = { adjustedSize.x, adjustedSize.y, adjustedSize.z };
				colliderShape->setGeometry(boxGeometry);
			}

			// INFO: Re-attach the shape to the actor
			rigidActor->attachShape(*colliderShape);
		}
	}

	void BoxCollider::SetSize(const Vector3& _size)
	{
		if (size != _size) { size = _size; }
		UpdateScale();
	}

	void BoxCollider::SetSize(float xHalfExtent, float yHalfExtent, float zHalfExtent)
	{
		SetSize(Vector3(xHalfExtent, yHalfExtent, zHalfExtent));
	}

	const Vector3& BoxCollider::GetSize() const
	{
		return size;
	}
}
