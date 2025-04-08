#pragma once

#include <filesystem>
#include <unordered_set>
#include <string>

namespace Flux::FiletypeConfig
{
	inline const std::filesystem::path ASSET_DIRECTORY = "Assets";
	inline const char* const SPRITEFONT = ".spritefont";
	inline const char* const DDS = ".dds";

	inline const std::unordered_set<std::string> SUPPORTED_TEXTURE_FORMATS = { ".png", ".jpg", ".jpeg", ".bmp" };
	inline const std::unordered_set<std::string> SUPPORTED_MODEL_FORMATS = { ".fbx", ".obj", ".x" };
	inline const std::unordered_set<std::string> SUPPORTED_AUDIO_FORMATS = { ".wav", ".mp3", ".ogg", ".flac", ".aiff" };

	inline bool IsSupportedTextureFormat(const std::string& extension)
	{
		return SUPPORTED_TEXTURE_FORMATS.find(extension) != SUPPORTED_TEXTURE_FORMATS.end();
	}

	inline bool IsSupportedModelFormat(const std::string& extension)
	{
		return SUPPORTED_MODEL_FORMATS.find(extension) != SUPPORTED_MODEL_FORMATS.end();
	}

	inline bool IsSupportedAudioFormat(const std::string& extension)
	{
		return SUPPORTED_AUDIO_FORMATS.find(extension) != SUPPORTED_AUDIO_FORMATS.end();
	}
}

