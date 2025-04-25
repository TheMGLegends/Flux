#include "Audio.h"

#include <filesystem>
#include <fmod_errors.h>

#include "Core/GlobalDefines.h"

#include "Core/Debug/Debug.h"
#include "Core/Renderer/AssetHandler.h"

namespace Flux
{
	using namespace GlobalDefines;

	FMOD::System* Audio::system = nullptr;
	std::vector<AudioData> Audio::audios;
	AudioData Audio::editorAudio;

	int Audio::Initialise()
	{
		FMOD_RESULT result = FMOD_OK;

		result = FMOD::System_Create(&system);

		if (result != FMOD_OK)
		{
			Debug::LogError("Audio::Initialise() - Failed to create FMOD system. FMOD Error: " + std::string(FMOD_ErrorString(result)));
			return FLUX_FAILURE;
		}

		result = system->init(512, FMOD_INIT_NORMAL, nullptr);

		if (result != FMOD_OK)
		{
			Debug::LogError("Audio::Initialise() - Failed to initialise FMOD system. FMOD Error: " + std::string(FMOD_ErrorString(result)));
			return FLUX_FAILURE;
		}

		return FLUX_SUCCESS;
	}

	void Audio::Update()
	{
		system->update();

		// INFO: Check channels for sounds that have finished playing
		std::erase_if(audios, [](const AudioData& audio)
			{
				if (bool isPlaying = audio.IsPlaying(); !isPlaying)
				{
					audio.channel->stop();
					audio.sound->release();
					return true;
				}

				return false;
			});
	}

	void Audio::Release()
	{
		// INFO: Release all sounds
		for (auto& audio : audios)
		{
			audio.channel->stop();
			audio.sound->release();
		}

		audios.clear();
		system->release();
	}

	void Audio::PlaySound2D(const std::string& audioName, float volume, bool isLooping)
	{
		// INFO: Verify audio exists
		const std::filesystem::path& audioPath = AssetHandler::GetAudioPath(audioName);

		if (audioPath.empty())
		{
			Debug::LogError("Audio::PlaySound2D() - Failed to play 2D sound, see above errors. Audio Name: " + audioName);
			return;
		}

		FMOD_RESULT result{};

		// INFO: Load Sound
		FMOD::Sound* sound = nullptr;
		result = system->createSound(audioPath.string().c_str(), FMOD_CREATESAMPLE | FMOD_2D | (isLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF), nullptr, &sound);

		if (result != FMOD_OK)
		{
			Debug::LogError("Audio::PlaySound2D() - Failed to create sound. FMOD Error: " + std::string(FMOD_ErrorString(result)));
			return;
		}

		// INFO: Play Sound
		FMOD::Channel* channel = nullptr;
		result = system->playSound(sound, nullptr, false, &channel);

		if (result != FMOD_OK)
		{
			Debug::LogError("Audio::PlaySound2D() - Failed to play sound. FMOD Error: " + std::string(FMOD_ErrorString(result)));
			return;
		}

		channel->setVolume(volume);

		audios.emplace_back(audioName, sound, channel);
	}

	void Audio::PlaySound3D(const std::string& audioName, const FMOD_VECTOR& position, float volume, float radius, bool isLooping)
	{
		// INFO: Verify audio exists
		const std::filesystem::path& audioPath = AssetHandler::GetAudioPath(audioName);

		if (audioPath.empty())
		{
			Debug::LogError("Audio::PlaySound3D() - Failed to play 3D sound, see above errors. Audio Name: " + audioName);
			return;
		}

		FMOD_RESULT result{};

		// INFO: Load Sound
		FMOD::Sound* sound = nullptr;
		result = system->createSound(audioPath.string().c_str(), FMOD_CREATESAMPLE | FMOD_3D | (isLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF), nullptr, &sound);

		if (result != FMOD_OK)
		{
			Debug::LogError("Audio::PlaySound3D() - Failed to create sound. FMOD Error: " + std::string(FMOD_ErrorString(result)));
			return;
		}

		sound->set3DMinMaxDistance(0.1f, radius);

		// INFO: Play Sound
		FMOD::Channel* channel = nullptr;
		result = system->playSound(sound, nullptr, true, &channel);

		if (result != FMOD_OK)
		{
			Debug::LogError("Audio::PlaySound3D() - Failed to play sound. FMOD Error: " + std::string(FMOD_ErrorString(result)));
			return;
		}

		channel->set3DAttributes(&position, nullptr);
		channel->setVolume(volume);
		channel->setPaused(false);

		audios.emplace_back(audioName, sound, channel);
	}

	void Audio::PlayMusic(const std::string& audioName, float volume, bool isLooping)
	{
		// INFO: Verify audio exists
		const std::filesystem::path& audioPath = AssetHandler::GetAudioPath(audioName);

		if (audioPath.empty())
		{
			Debug::LogError("Audio::PlayMusic() - Failed to play music, see above errors. Audio Name: " + audioName);
			return;
		}

		FMOD_RESULT result{};

		// INFO: Load Music
		FMOD::Sound* music = nullptr;
		result = system->createStream(audioPath.string().c_str(), FMOD_CREATESTREAM | FMOD_2D | (isLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF), nullptr, &music);

		if (result != FMOD_OK)
		{
			Debug::LogError("Audio::PlayMusic() - Failed to create sound. FMOD Error: " + std::string(FMOD_ErrorString(result)));
			return;
		}

		// INFO: Play Music
		FMOD::Channel* channel = nullptr;
		result = system->playSound(music, nullptr, false, &channel);

		if (result != FMOD_OK)
		{
			Debug::LogError("Audio::PlayMusic() - Failed to play sound. FMOD Error: " + std::string(FMOD_ErrorString(result)));
			return;
		}

		channel->setVolume(volume);

		audios.emplace_back(audioName, music, channel);
	}

	void Audio::PlaySoundInEditor(const std::string& audioName, float volume)
	{
		if (editorAudio.IsPlaying())
		{
			editorAudio.channel->stop();
			editorAudio.sound->release();

			// INFO: If the double clicked audio is the same as the current audio playing, do not play it again (Stop audio)
			if (audioName == editorAudio.name) { return; }
		}

		// INFO: Verify audio exists
		const std::filesystem::path& audioPath = AssetHandler::GetAudioPath(audioName);

		if (audioPath.empty())
		{
			Debug::LogError("Audio::PlaySound2D() - Failed to play 2D sound, see above errors. Audio Name: " + audioName);
			return;
		}

		FMOD_RESULT result{};

		// INFO: Load Sound
		FMOD::Sound* sound = nullptr;
		result = system->createSound(audioPath.string().c_str(), FMOD_CREATESAMPLE | FMOD_2D | FMOD_LOOP_OFF, nullptr, &sound);

		if (result != FMOD_OK)
		{
			Debug::LogError("Audio::PlaySound2D() - Failed to create sound. FMOD Error: " + std::string(FMOD_ErrorString(result)));
			return;
		}

		// INFO: Play Sound
		FMOD::Channel* channel = nullptr;
		result = system->playSound(sound, nullptr, false, &channel);

		if (result != FMOD_OK)
		{
			Debug::LogError("Audio::PlaySound2D() - Failed to play sound. FMOD Error: " + std::string(FMOD_ErrorString(result)));
			return;
		}

		channel->setVolume(volume);

		editorAudio.name = audioName;
		editorAudio.sound = sound;
		editorAudio.channel = channel;
	}

	void Audio::StopSoundInEditor()
	{
		if (editorAudio.IsPlaying())
		{
			editorAudio.channel->stop();
			editorAudio.sound->release();
		}
	}

	void Audio::StopSound(std::string_view audioName)
	{
		for (auto& audio : audios)
		{
			if (audio.name == audioName)
			{
				audio.channel->stop();
				audio.sound->release();
				break;
			}
		}
	}

	void Audio::StopAllSounds()
	{
		for (auto& audio : audios)
		{
			audio.channel->stop();
			audio.sound->release();
		}

		audios.clear();
	}

	void Audio::ControlSounds(bool isPaused)
	{
		for (auto& audio : audios) { audio.channel->setPaused(isPaused); }
	}

	bool Audio::IsSoundPlaying(const std::string& audioName)
	{
		for (const auto& audio : audios)
		{
			if (audio.name == audioName)
			{
				return audio.IsPlaying();
			}
		}
		return false;
	}

	void Audio::SetListenerAttributes(const FMOD_VECTOR& position, const FMOD_VECTOR& velocity, const FMOD_VECTOR& forward, const FMOD_VECTOR& up)
	{
		system->set3DListenerAttributes(0, &position, &velocity, &forward, &up);
	}
}
