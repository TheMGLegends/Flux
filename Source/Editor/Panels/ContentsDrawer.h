#pragma once

#include "EditorPanel.h"

#include <filesystem>
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
		std::vector<std::filesystem::directory_entry> contents;

		float contentsRefreshTime;
		float currentRefreshTime;
	};
}

