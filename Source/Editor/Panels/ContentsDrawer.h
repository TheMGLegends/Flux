#pragma once

#include "EditorPanel.h"

#include <imgui.h>
#include <filesystem>
#include <string>
#include <vector>

namespace Flux
{
	class ContentsDrawer : public EditorPanel
	{
	public:
		ContentsDrawer();
		virtual ~ContentsDrawer() override;

		virtual int Initialise() override;
		virtual void Update(float deltaTime) override;

		void UpdateContents();
		void DrawContents();

	private:
		struct ContentsData
		{
			std::filesystem::directory_entry entry;

			std::string filenameString;
			std::string filenameStemString;
			std::string extensionType;
		};

		enum class AssetType
		{
			None = 0,

			Scene,
			Model,
			Texture,
			SkyboxTexture
		};

	private:
		std::filesystem::path currentDirectory;
		std::vector<ContentsData> contents;

		float contentsRefreshTime;
		float currentRefreshTime;

		ImTextureID folderIcon;
		ImTextureID meshIcon;
		ImTextureID audioIcon;
		ImTextureID ddsIcon;
		ImTextureID sceneIcon;
		ImTextureID defaultFileIcon;

		std::string searchString;
	};
}

