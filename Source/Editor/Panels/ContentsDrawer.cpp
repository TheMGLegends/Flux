#include "ContentsDrawer.h"

#include "Core/GlobalDefines.h"

#include "Core/Configs/FiletypeConfig.h"
#include "Core/Configs/RuntimeConfig.h"

#include "Core/Debug/Debug.h"
#include "Core/EventSystem/EventDispatcher.h"
#include "Core/Helpers/StringHelpers.h"
#include "Core/Renderer/AssetHandler.h"
#include "Core/EventSystem/Events/LoadSceneEvent.h"

#include "Engine/Scene/SceneContext.h"

namespace Flux
{
	using namespace GlobalDefines;

	ContentsDrawer::ContentsDrawer() : contentsRefreshTime(1.0f), currentRefreshTime(0.0f), currentDirectory()
	{
	}

	ContentsDrawer::~ContentsDrawer()
	{
	}

	int ContentsDrawer::Initialise()
	{
		currentDirectory = FiletypeConfig::ASSET_DIRECTORY;

		if (!std::filesystem::exists(currentDirectory))
		{
			Debug::LogError("ContentsDrawer::Initialise() - Asset directory does not exist: " + currentDirectory.string());
			return FLUX_FAILURE;
		}

		UpdateContents();

		if (contents.empty())
		{
			Debug::LogWarning("ContentsDrawer::Initialise() - No contents found in the asset directory: " + currentDirectory.string());
		}

		// INFO: Cache Folder Icons
		folderIcon = (ImTextureID)AssetHandler::GetTexture("FolderIcon");
		meshIcon = (ImTextureID)AssetHandler::GetTexture("MeshIcon");
		audioIcon = (ImTextureID)AssetHandler::GetTexture("AudioIcon");
		ddsIcon = (ImTextureID)AssetHandler::GetTexture("DdsIcon");
		sceneIcon = (ImTextureID)AssetHandler::GetTexture("SceneIcon");
		defaultFileIcon = (ImTextureID)AssetHandler::GetTexture("DefaultFileIcon");

		return FLUX_SUCCESS;
	}

	void ContentsDrawer::Update(float deltaTime)
	{
		if (ImGui::Begin("Contents Drawer"))
		{
			currentRefreshTime += deltaTime;

			if (currentRefreshTime >= contentsRefreshTime)
			{
				currentRefreshTime = 0.0f;
				UpdateContents();
			}

			ImVec2 windowPos = ImGui::GetWindowPos();
			ImVec2 windowSize = ImGui::GetWindowSize();

			// INFO: Play Mode Overlay
			if (RuntimeConfig::IsInPlayMode() || RuntimeConfig::IsPaused())
			{
				ImGui::GetForegroundDrawList()->AddRectFilled(windowPos, ImVec2(windowPos.x + windowSize.x, windowPos.y + windowSize.y), IM_COL32(0, 116, 188, 50));
			}

			if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
			{
				ImGui::OpenPopup("ContentsContextMenu");
			}

			if (ImGui::BeginPopup("ContentsContextMenu"))
			{
				if (ImGui::MenuItem("New Scene"))
				{
					std::filesystem::path newScenePath = currentDirectory / "NewScene.json";
					SceneContext::GetScene().CreateDefaultScene(newScenePath);
				}

				ImGui::EndPopup();
			}

			DrawContents();

			ImGui::End();
		}
	}

	void ContentsDrawer::UpdateContents()
	{
		contents.clear();

		for (const auto& entry : std::filesystem::directory_iterator(currentDirectory))
		{
			std::filesystem::path relativePath = std::filesystem::relative(entry.path(), FiletypeConfig::ASSET_DIRECTORY);

			ContentsData contentsData;
			contentsData.entry = entry;
			contentsData.filenameString = relativePath.filename().string();
			contentsData.filenameStemString = relativePath.stem().string();
			contentsData.extensionType = StringHelpers::ToLower(relativePath.extension().string());

			contents.push_back(contentsData);
		}
	}

	void ContentsDrawer::DrawContents()
	{
		if (currentDirectory != FiletypeConfig::ASSET_DIRECTORY)
		{
			if (ImGui::Button("Back"))
			{
				currentDirectory = currentDirectory.parent_path();
				UpdateContents();
			}
		}

		AssetType assetType = AssetType::None;
		ImTextureID textureID = 0;
		ImVec2 iconSize = ImVec2(50.0f, 50.0f);
		float panelWidth = ImGui::GetContentRegionAvail().x;

		for (size_t i = 0; i < contents.size(); i++)
		{
			ImGui::PushID(static_cast<int>(i));

			ContentsData& contentsData = contents[i];
			const std::filesystem::directory_entry& entry = contentsData.entry;
			const std::string& filenameString = contentsData.filenameString;
			const std::string& filenameStemString = contentsData.filenameStemString;
			const std::string& extensionType = contentsData.extensionType;
			const std::filesystem::path& path = entry.path();

			// INFO: Icon Choosing
			if (entry.is_directory())
			{
				textureID = folderIcon; // INFO: Folders
			}
			else if (FiletypeConfig::IsSupportedTextureFormat(extensionType))
			{
				textureID = (ImTextureID)AssetHandler::GetTexture(filenameStemString); // INFO: Textures
				assetType = AssetType::Texture;
			}
			else if (FiletypeConfig::IsSupportedModelFormat(extensionType))
			{
				textureID = meshIcon; // INFO: Meshes
				assetType = AssetType::Model;
			}
			else if (FiletypeConfig::IsSupportedAudioFormat(extensionType))
			{
				textureID = audioIcon; // INFO: Audio
			}
			else if (extensionType == FiletypeConfig::DDS)
			{
				textureID = ddsIcon; // INFO: DDS Textures
				assetType = AssetType::SkyboxTexture;
			}
			else if (extensionType == ".json")
			{
				textureID = sceneIcon; // INFO: Scenes
				assetType = AssetType::Scene;
			}
			else
			{
				textureID = defaultFileIcon; // INFO: Generic Files
			}

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(16.0f, 4.0f));

			// INFO: Group Icon & Label
			ImGui::BeginGroup();

			// INFO: Remove Background Colour for Buttons
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
			ImGui::ImageButton("##Asset", textureID, iconSize);
			ImGui::PopStyleColor();

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (entry.is_directory())
				{
					currentDirectory /= path.filename();
					UpdateContents();
				}
				else if (assetType == AssetType::Scene)
				{
					EventDispatcher::QueueEvent(EventType::LoadScene, std::make_shared<LoadSceneEvent>(path));
				}
			}

			// INFO: Setup Drag & Drop Payload
			if (ImGui::BeginDragDropSource())
			{
				std::string payloadType = "";

				switch (assetType)
				{
				case AssetType::Model:
					payloadType = "Model";
					break;
				case AssetType::Texture:
					payloadType = "Texture";
					break;
				case AssetType::SkyboxTexture:
					payloadType = "SkyboxTexture";
					break;
				default:
					break;
				}

				ImGui::Image(textureID, iconSize);
				ImGui::SetDragDropPayload(payloadType.c_str(), filenameStemString.c_str(), filenameStemString.size() + 1);

				ImGui::EndDragDropSource();
			}

			// INFO: Centre Text if shorter than icon width
			float textWidth = ImGui::CalcTextSize(filenameString.c_str()).x;
			float paddingWidth = ImGui::GetStyle().ItemSpacing.x;

			if (textWidth < iconSize.x + paddingWidth)
			{
				ImGui::SetCursorPos({ ImGui::GetCursorPos().x + (iconSize.x + (paddingWidth * 0.5f) - textWidth) * 0.5f, ImGui::GetCursorPos().y });
			}

			ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + iconSize.x + ImGui::GetStyle().ItemSpacing.x);
			ImGui::Text(filenameString.c_str());
			ImGui::PopTextWrapPos();

			ImGui::EndGroup();

			float nextItemPosition = ImGui::GetItemRectMax().x + ImGui::GetStyle().ItemSpacing.x + iconSize.x;
			if (nextItemPosition < panelWidth) { ImGui::SameLine(); }

			ImGui::PopStyleVar();

			ImGui::PopID();
		}
	}
}
