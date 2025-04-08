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

	for (size_t i = 0; i < contents.size(); i++)
	{
		std::filesystem::directory_entry& entry = contents[i];

		const std::filesystem::path& path = entry.path();
		std::filesystem::path relativePath = std::filesystem::relative(path, FiletypeConfig::ASSET_DIRECTORY);
		std::string filenameString = relativePath.filename().string();

		if (entry.is_directory())
		{
			if (ImGui::Button(filenameString.c_str()))
			{
				currentDirectory /= path.filename();
				UpdateContents();
			}
		}
		else
		{
			std::string extensionType = StringHelpers::ToLower(relativePath.extension().string());
			ImTextureID textureID = 0;

			if (FiletypeConfig::IsSupportedTextureFormat(extensionType))
			{
				textureID = (ImTextureID)AssetHandler::GetTexture(relativePath.stem().string());
			}
			else if (extensionType == FiletypeConfig::DDS)
			{
				textureID = (ImTextureID)AssetHandler::GetTexture("DdsIcon");
			}
			else if (extensionType == ".obj")
			{
				textureID = (ImTextureID)AssetHandler::GetTexture("ObjIcon");
			}
			else if (extensionType == ".json")
			{
				textureID = (ImTextureID)AssetHandler::GetTexture("JsonIcon");
			}

			if (textureID == 0)
			{
				textureID = (ImTextureID)AssetHandler::GetTexture("DefaultFileIcon");
			}

			ImGui::Image(textureID, ImVec2(50.0f, 50.0f));
			ImGui::Text(filenameString.c_str());
		}
	}
}
