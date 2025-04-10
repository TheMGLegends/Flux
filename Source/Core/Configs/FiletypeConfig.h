#pragma once

#include <filesystem>
#include <unordered_set>
#include <string>

namespace Flux::FiletypeConfig
{
	extern const std::filesystem::path ASSET_DIRECTORY;
	extern const char* const SPRITEFONT;
	extern const char* const DDS;
	extern const char* const SCENE;

	extern const std::unordered_set<std::string> SUPPORTED_TEXTURE_FORMATS;
	extern const std::unordered_set<std::string> SUPPORTED_MODEL_FORMATS;
	extern const std::unordered_set<std::string> SUPPORTED_AUDIO_FORMATS;

	bool IsSupportedTextureFormat(const std::string& extension);
	bool IsSupportedModelFormat(const std::string& extension);
	bool IsSupportedAudioFormat(const std::string& extension);
}

