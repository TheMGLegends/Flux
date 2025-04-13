#include "SphereCollider.h"

#include <DirectXColors.h>
#include <imgui.h>
#include <imgui_internal.h>

#include "Core/Configs/EditorConfig.h"

#include "Core/Helpers/MathHelpers.h"

#include "Engine/Physics/Physics.h"
#include "Engine/Scene/SceneContext.h"
#include "Engine/Entities/GameObjects/GameObject.h"
#include "Engine/Entities/Components/PhysicsBody.h"
#include "Engine/Entities/Components/Transform.h"

using namespace DirectX::SimpleMath;

namespace Flux
{
	SphereCollider::SphereCollider(GameObject* _gameObject) : Collider(_gameObject), radius(1.0f)
	{
		name = "SphereCollider";
		componentType = ComponentType::SphereCollider;

		// INFO: Create Sphere Collider Shape
		SetColliderShape();
	}

	SphereCollider::~SphereCollider()
	{
		if (rigidActor)
		{
			rigidActor->release();
			rigidActor = nullptr;
		}
	}

	void SphereCollider::DrawDetails()
	{
		ImGui::PushID(this);

		bool treeOpened = ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen);

		// INFO: Active Checkbox
		ImGui::SameLine();
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
					Debug::LogWarning("SphereCollider::DrawDetails() - Removed associated PhysicsBody component");
				}

				gameObject->RemoveComponent(weak_from_this());
				EditorConfig::sceneNeedsSaving = true;
			}
		}

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

			// INFO: Radius InputField
			ImGui::Text("Radius");
			ImGui::SameLine();
			ImGui::SetCursorPosX(136.0f);
			ImGui::SetNextItemWidth(50.0f);
			if (ImGui::InputFloat("##Radius", &radius, 0.0f, 0.0f, "%.1f"))
			{
				UpdateScale();
				EditorConfig::sceneNeedsSaving = true;
			}

			ImGui::TreePop();
		}
		ImGui::PopID();

		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
	}

	void SphereCollider::Serialize(nlohmann::flux_json& json) const
	{
		// INFO: Serialize Parent Class
		Collider::Serialize(json);

		json["Components"].back()["Radius"] = radius;
	}

	void SphereCollider::Deserialize(const nlohmann::flux_json& json)
	{
		// INFO: Deserialize Parent Class
		Collider::Deserialize(json);

		// INFO: Deserialize SphereCollider Data
		SetRadius(json["Radius"].get<float>());
	}

	void SphereCollider::DrawWireframe(ID3D11DeviceContext& deviceContext, DirectX::PrimitiveBatch<DirectX::VertexPositionColor>& primitiveBatch)
	{
		DirectX::XMVECTOR centre = DirectX::XMLoadFloat3(&GetGameObject()->transform.lock()->GetPosition());

		DirectX::XMVECTOR xRange = DirectX::XMVectorSet(radius, 0, 0, 0);
		DirectX::XMVECTOR yRange = DirectX::XMVectorSet(0, radius, 0, 0);
		DirectX::XMVECTOR zRange = DirectX::XMVectorSet(0, 0, radius, 0);

		bool isTrigger = IsTrigger();

		DrawRing(deviceContext, primitiveBatch, centre, xRange, yRange, isTrigger);
		DrawRing(deviceContext, primitiveBatch, centre, xRange, zRange, isTrigger);
		DrawRing(deviceContext, primitiveBatch, centre, yRange, zRange, isTrigger);
	}

	void SphereCollider::SetColliderShape()
	{
		// INFO: Clear Collider Shape if it already exists
		if (colliderShape)
		{
			colliderShape->release();
			colliderShape = nullptr;
		}

		// INFO: Create Sphere Collider Shape
		colliderShape = Physics::GetPhysics().createShape(physx::PxSphereGeometry(radius), Physics::GetDefaultPhysicsMaterial(), true);

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
				Debug::LogError("SphereCollider::SphereCollider() - Failed to attach shape to Rigid Actor");
			}

			SceneContext::GetScene().GetPhysicsScene().addActor(*rigidActor);
		}
	}

	void SphereCollider::UpdateScale()
	{
		if (colliderShape)
		{
			// INFO: Get reference to actor that the shape is attached to
			physx::PxRigidActor* rigidActor = colliderShape->getActor();

			// INFO: Remove the shape from the actor to allow for geometry changes
			rigidActor->detachShape(*colliderShape);

			physx::PxSphereGeometry sphereGeometry{};
			if (colliderShape->getSphereGeometry(sphereGeometry))
			{
				Vector3 scale = GetGameObject()->transform.lock()->GetScale();
				sphereGeometry.radius = radius * MathHelpers::Max(scale.x, scale.y, scale.z);
				colliderShape->setGeometry(sphereGeometry);
			}

			// INFO: Re-attach the shape to the actor
			rigidActor->attachShape(*colliderShape);
		}
	}

	void SphereCollider::SetRadius(float _radius)
	{
		radius = _radius;
		UpdateScale();
	}

	void SphereCollider::DrawRing(ID3D11DeviceContext& deviceContext, DirectX::PrimitiveBatch<DirectX::VertexPositionColor>& primitiveBatch,
		const DirectX::XMVECTOR& centre, const DirectX::XMVECTOR& majorAxis, const DirectX::XMVECTOR& minorAxis, bool isTrigger)
	{
		static const int ringSegments = 32;

		DirectX::VertexPositionColor vertices[ringSegments + 1]{};

		float segmentAngle = DirectX::XM_2PI / static_cast<float>(ringSegments);
		DirectX::XMVECTOR cosSegment = DirectX::XMVectorReplicate(cosf(segmentAngle));
		DirectX::XMVECTOR sinSegment = DirectX::XMVectorReplicate(sinf(segmentAngle));
		DirectX::XMVECTOR incrementalSin = DirectX::XMVectorZero();

		static const DirectX::XMVECTOR initialCos = { 1.0f, 1.0f, 1.0f, 1.0f };

		DirectX::XMVECTOR incrementalCos = initialCos;

		for (size_t i = 0; i < ringSegments; i++)
		{
			DirectX::XMVECTOR position{};

			position = DirectX::XMVectorMultiplyAdd(majorAxis, incrementalCos, centre);
			position = DirectX::XMVectorMultiplyAdd(minorAxis, incrementalSin, position);

			XMStoreFloat3(&vertices[i].position, position);
			XMStoreFloat4(&vertices[i].color, isTrigger ? DirectX::Colors::Yellow : DirectX::Colors::LawnGreen);

			DirectX::XMVECTOR newCos = DirectX::XMVectorSubtract(DirectX::XMVectorMultiply(incrementalCos, cosSegment), DirectX::XMVectorMultiply(incrementalSin, sinSegment));
			DirectX::XMVECTOR newSin = DirectX::XMVectorAdd(DirectX::XMVectorMultiply(incrementalCos, sinSegment), DirectX::XMVectorMultiply(incrementalSin, cosSegment));

			incrementalCos = newCos;
			incrementalSin = newSin;
		}

		vertices[ringSegments] = vertices[0];

		primitiveBatch.Draw(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP, vertices, static_cast<size_t>(ringSegments) + 1);
	}
}
