#include "FiletypeConfig.h"

namespace Flux::FiletypeConfig
{
	const std::filesystem::path ASSET_DIRECTORY = "Assets";

	const char* const SPRITEFONT = ".spritefont";
	const char* const DDS = ".dds";
	const char* const SCENE = ".json";

	const std::unordered_set<std::string> SUPPORTED_TEXTURE_FORMATS = { ".png", ".jpg", ".jpeg", ".bmp" };
	const std::unordered_set<std::string> SUPPORTED_MODEL_FORMATS = { ".fbx", ".obj", ".x" };
	const std::unordered_set<std::string> SUPPORTED_AUDIO_FORMATS = { ".wav", ".mp3", ".ogg", ".flac", ".aiff" };

	bool IsSupportedTextureFormat(const std::string& extension)
	{
		return SUPPORTED_TEXTURE_FORMATS.contains(extension);
	}

	bool IsSupportedModelFormat(const std::string& extension)
	{
		return SUPPORTED_MODEL_FORMATS.contains(extension);
	}

	bool IsSupportedAudioFormat(const std::string& extension)
	{
		return SUPPORTED_AUDIO_FORMATS.contains(extension);
	}
}
