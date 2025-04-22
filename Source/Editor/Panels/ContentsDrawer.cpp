#include "ContentsDrawer.h"

#include <assimp/Importer.hpp>
#include <string/imgui_stdlib.h>

#include "Core/GlobalDefines.h"

#include "Core/Configs/FiletypeConfig.h"
#include "Core/Configs/RuntimeConfig.h"

#include "Core/Debug/Debug.h"
#include "Core/EventSystem/EventDispatcher.h"
#include "Core/Helpers/StringHelpers.h"
#include "Core/Renderer/AssetHandler.h"
#include "Core/EventSystem/Events/CreateSceneEvent.h"
#include "Core/EventSystem/Events/LoadSceneEvent.h"

#include "Engine/Scene/SceneContext.h"

namespace Flux
{
	using namespace GlobalDefines;

	ContentsDrawer::ContentsDrawer() : currentDirectory(), contents(), contentsRefreshTime(1.0f), currentRefreshTime(0.0f),
									   folderIcon(0), meshIcon(0), audioIcon(0), ddsIcon(0), sceneIcon(0), defaultFileIcon(0)
	{
	}

	ContentsDrawer::~ContentsDrawer() = default;

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

			DrawContentsContextMenu();

			// INFO: Back Button
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
			ImGui::BeginDisabled(currentDirectory == FiletypeConfig::ASSET_DIRECTORY);
			if (ImGui::Button("Back"))
			{
				currentDirectory = currentDirectory.parent_path();
				UpdateContents();
				searchString.clear();
			}
			ImGui::EndDisabled();
			
			ImGui::SameLine();

			// INFO: Search Bar
			ImGui::PushItemWidth(300.0f);
			ImGui::InputTextWithHint("##SearchBar", "Search...", &searchString, ImGuiInputTextFlags_CallbackAlways | ImGuiInputTextFlags_AutoSelectAll);
			ImGui::PopItemWidth();
			ImGui::PopStyleVar();

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
		ImVec2 iconSize = ImVec2(50.0f, 50.0f);
		float panelWidth = ImGui::GetContentRegionAvail().x;
		bool updateContents = false;

		for (size_t i = 0; i < contents.size(); i++)
		{
			const ContentsData& contentsData = contents[i];
			const std::string& filenameString = contentsData.filenameString;

			// INFO: Filter by Search String
			if (!searchString.empty() && filenameString.rfind(searchString, 0) == std::string::npos) { continue; }

			ImGui::PushID(static_cast<int>(i));

			const std::filesystem::directory_entry& entry = contentsData.entry;
			const std::filesystem::path& path = entry.path();
			const std::string& filenameStemString = contentsData.filenameStemString;

			// INFO: Choose the Asset Icon
			AssetIconData assetIconData = ChooseAssetIcon(contentsData);

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(16.0f, 4.0f));

			// INFO: Group Icon & Label
			ImGui::BeginGroup();

			// INFO: Remove Background Colour for Buttons
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
			ImGui::ImageButton("##Asset", assetIconData.textureID, iconSize);
			ImGui::PopStyleColor();

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (entry.is_directory())
				{
					currentDirectory /= path.filename();
					updateContents = true;
					searchString.clear();
				}
				else if (assetIconData.assetType == AssetType::Scene)
				{
					EventDispatcher::QueueEvent(EventType::LoadScene, std::make_shared<LoadSceneEvent>(path));
				}
			}

			// INFO: Setup Drag & Drop Payload
			if (RuntimeConfig::IsInEditorMode() && ImGui::BeginDragDropSource())
			{
				std::string payloadType = "";

				switch (assetIconData.assetType)
				{
				using enum AssetType;
				case Model:
					payloadType = "Model";
					break;
				case Texture:
					payloadType = "Texture";
					break;
				case SkyboxTexture:
					payloadType = "SkyboxTexture";
					break;
				default:
					break;
				}

				ImGui::Image(assetIconData.textureID, iconSize);
				ImGui::SetDragDropPayload(payloadType.c_str(), filenameStemString.c_str(), filenameStemString.size() + 1);

				ImGui::EndDragDropSource();
			}

			// INFO: Centre Text if shorter than icon width
			float textWidth = ImGui::CalcTextSize(filenameString.c_str()).x;

			if (float paddingWidth = ImGui::GetStyle().ItemSpacing.x; textWidth < iconSize.x + paddingWidth)
			{
				ImGui::SetCursorPos({ ImGui::GetCursorPos().x + (iconSize.x + (paddingWidth * 0.5f) - textWidth) * 0.5f, ImGui::GetCursorPos().y });
			}

			ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + iconSize.x + ImGui::GetStyle().ItemSpacing.x);
			ImGui::Text(filenameString.c_str());
			ImGui::PopTextWrapPos();

			ImGui::EndGroup();

			if (float nextItemPosition = ImGui::GetItemRectMax().x + ImGui::GetStyle().ItemSpacing.x + iconSize.x; nextItemPosition < panelWidth) 
			{ 
				ImGui::SameLine(); 
			}

			ImGui::PopStyleVar();

			ImGui::PopID();

			if (updateContents) { UpdateContents(); }
		}
	}

	void ContentsDrawer::DrawContentsContextMenu() const
	{
		if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		{
			ImGui::OpenPopup("ContentsContextMenu");
		}

		if (ImGui::BeginPopup("ContentsContextMenu"))
		{
			if (ImGui::MenuItem("New Scene"))
			{
				std::string newSceneName = std::format("NewScene{}.json", AssetHandler::GetSceneCount());
				std::filesystem::path newScenePath = currentDirectory / newSceneName;

				EventDispatcher::QueueEvent(EventType::CreateScene, std::make_shared<CreateSceneEvent>(newSceneName, newScenePath));
			}

			ImGui::EndPopup();
		}
	}

	ContentsDrawer::AssetIconData ContentsDrawer::ChooseAssetIcon(const ContentsData& contentsData) const
	{
		AssetIconData assetIconData{};

		const std::filesystem::directory_entry& entry = contentsData.entry;
		const std::string& filenameStemString = contentsData.filenameStemString;
		const std::string& extensionType = contentsData.extensionType;
		const std::filesystem::path& path = entry.path();

		// INFO: Icon Choosing
		if (entry.is_directory())
		{
			assetIconData.textureID = folderIcon; // INFO: Folders
		}
		else if (FiletypeConfig::IsSupportedTextureFormat(extensionType))
		{
			// INFO: Load Texture if not already loaded
			if (!AssetHandler::HasTexture(filenameStemString))
			{
				AssetHandler::LoadTexture(path);
			}

			assetIconData.textureID = (ImTextureID)AssetHandler::GetTexture(filenameStemString); // INFO: Textures
			assetIconData.assetType = AssetType::Texture;
		}
		else if (FiletypeConfig::IsSupportedModelFormat(extensionType))
		{
			// INFO: Load Model if not already loaded
			if (!AssetHandler::HasModel(filenameStemString))
			{
				Assimp::Importer importer;
				AssetHandler::LoadModel(path, importer);
			}

			assetIconData.textureID = meshIcon; // INFO: Meshes
			assetIconData.assetType = AssetType::Model;
		}
		else if (FiletypeConfig::IsSupportedAudioFormat(extensionType))
		{
			// INFO: Load Audio if not already loaded
			if (!AssetHandler::HasAudioPath(filenameStemString))
			{
				AssetHandler::StoreAudioPath(path);
			}

			assetIconData.textureID = audioIcon; // INFO: Audio
		}
		else if (extensionType == FiletypeConfig::DDS)
		{
			// INFO: Load DDS Texture if not already loaded
			if (!AssetHandler::HasTexture(filenameStemString, true))
			{
				AssetHandler::LoadTexture(path, true);
			}

			assetIconData.textureID = ddsIcon; // INFO: DDS Textures
			assetIconData.assetType = AssetType::SkyboxTexture;
		}
		else if (extensionType == ".json")
		{
			assetIconData.textureID = sceneIcon; // INFO: Scenes
			assetIconData.assetType = AssetType::Scene;
		}
		else
		{
			assetIconData.textureID = defaultFileIcon; // INFO: Generic Files
		}

		return assetIconData;
	}
}
