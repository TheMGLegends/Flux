#pragma once

#include <fmod.hpp>
#include <string>
#include <vector>

namespace Flux
{
	struct AudioData
	{
	public:
		AudioData(std::string _name, FMOD::Sound* _sound, FMOD::Channel* _channel) : name(_name), sound(_sound), channel(_channel) {}
		~AudioData() = default;

		bool IsPlaying() const
		{
			bool isPlaying = false;
			channel->isPlaying(&isPlaying);
			return isPlaying;
		}

	public:
		std::string name;
		FMOD::Sound* sound;
		FMOD::Channel* channel;
	};

	class Audio
	{
	public:
		Audio() = delete;
		~Audio() = delete;
		Audio(const Audio&) = delete;
		Audio& operator=(const Audio&) = delete;

		static int Initialise();
		static void Update();
		static void Release();

		/// @brief Plays a 2D sound
		/// @param volume 0.0f to 1.0f (0.0f is silent, 1.0f is full volume)
		/// @param isLooping If false the sound will play once, if true the sound will loop
		static void PlaySound2D(const std::string& audioName, float volume = 1.0f, bool isLooping = false);

		/// @brief Plays a 3D sound
		/// @param position The position of the sound in 3D space
		/// @param volume 0.0f to 1.0f (0.0f is silent, 1.0f is full volume)
		/// @param radius The radius of the sound in 3D space
		/// @param isLooping If false the sound will play once, if true the sound will loop
		static void PlaySound3D(const std::string& audioName, const FMOD_VECTOR& position, float volume = 1.0f, float radius = 100.0f, bool isLooping = false);

		/// @brief Plays 2D music
		/// @param volume 0.0f to 1.0f (0.0f is silent, 1.0f is full volume)
		/// @param isLooping If false the music will play once, if true the music will loop
		static void PlayMusic(const std::string& audioName, float volume = 1.0f, bool isLooping = true);

		/// @brief Primarily used for looping music tracks
		static void StopSound(const std::string& audioName);
		static void StopAllSounds();

		static void ControlSounds(bool isPaused);

		static void SetListenerAttributes(const FMOD_VECTOR& position, const FMOD_VECTOR& velocity, const FMOD_VECTOR& forward, const FMOD_VECTOR& up);

	private:
		static FMOD::System* system;
		static std::vector<AudioData> audios;
	};
}

