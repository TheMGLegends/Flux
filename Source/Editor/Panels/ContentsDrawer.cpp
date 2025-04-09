#include "ContentsDrawer.h"

#include <imgui.h>

#include "Core/GlobalDefines.h"
#include "Core/Configs/FiletypeConfig.h"
#include "Core/Configs/RuntimeConfig.h"
#include "Core/Debug/Debug.h"
#include "Core/Helpers/StringHelpers.h"
#include "Core/Renderer/AssetHandler.h"

using namespace Flux;
using namespace Flux::GlobalDefines;

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

		DrawContents();

		ImGui::End();
	}
}

void ContentsDrawer::UpdateContents()
{
	contents.clear();

	for (const auto& entry : std::filesystem::directory_iterator(currentDirectory))
	{
		contents.push_back(entry);
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

	ImTextureID textureID = 0;
	ImVec2 iconSize = ImVec2(50.0f, 50.0f);
	float panelWidth = ImGui::GetContentRegionAvail().x;

	for (size_t i = 0; i < contents.size(); i++)
	{
		std::filesystem::directory_entry& entry = contents[i];

		ImGui::PushID(static_cast<int>(i));

		const std::filesystem::path& path = entry.path();
		std::filesystem::path relativePath = std::filesystem::relative(path, FiletypeConfig::ASSET_DIRECTORY);
		std::string filenameString = relativePath.filename().string();
		std::string extensionType = StringHelpers::ToLower(relativePath.extension().string());

		// INFO: Icon Choosing
		if (entry.is_directory())
		{
			textureID = (ImTextureID)AssetHandler::GetTexture("FolderIcon"); // INFO: Folders
		}
		else if (FiletypeConfig::IsSupportedTextureFormat(extensionType))
		{
			textureID = (ImTextureID)AssetHandler::GetTexture(relativePath.stem().string()); // INFO: Textures
		}
		else if (FiletypeConfig::IsSupportedModelFormat(extensionType))
		{
			textureID = (ImTextureID)AssetHandler::GetTexture("MeshIcon"); // INFO: Meshes
		}
		else if (FiletypeConfig::IsSupportedAudioFormat(extensionType))
		{
			textureID = (ImTextureID)AssetHandler::GetTexture("AudioIcon"); // INFO: Audio
		}
		else if (extensionType == FiletypeConfig::DDS)
		{
			textureID = (ImTextureID)AssetHandler::GetTexture("DdsIcon"); // INFO: DDS Textures
		}
		else if (extensionType == ".json")
		{
			textureID = (ImTextureID)AssetHandler::GetTexture("SceneIcon"); // INFO: Scenes
		}
		else
		{
			textureID = (ImTextureID)AssetHandler::GetTexture("DefaultFileIcon"); // INFO: Generic Files
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
