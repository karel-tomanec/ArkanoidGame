#include "soundPlayer.hpp"

#include <SDL3/SDL_audio.h>

SoundPlayer::SoundPlayer()
{
	const std::string soundsPath = "assets/sounds/";
	loadSound(SoundId::Enter, std::string(soundsPath + "enter.wav").c_str());
	loadSound(SoundId::Start, std::string(soundsPath + "start.wav").c_str());
	loadSound(SoundId::Bounce, std::string(soundsPath + "bounce.wav").c_str());
	loadSound(SoundId::Fall, std::string(soundsPath + "fall.wav").c_str());
	loadSound(SoundId::HitWall, std::string(soundsPath + "hit_wall.wav").c_str());
	loadSound(SoundId::Click, std::string(soundsPath + "click.wav").c_str());
	loadSound(SoundId::Win, std::string(soundsPath + "win.wav").c_str());
	loadSound(SoundId::GameOver, std::string(soundsPath + "game_over.wav").c_str());

	SDL_AudioSpec& spec = sounds[static_cast<size_t>(SoundId::Bounce)].spec;

	stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, nullptr, nullptr);
	if (!stream)
	{
		throw std::runtime_error(std::format("SDL_OpenAudioDeviceStream Error: {}", SDL_GetError()));
	}
	if (!SDL_ResumeAudioStreamDevice(stream))
	{
		throw std::runtime_error(std::format("SDL_ResumeAudioStreamDevice Error: {}", SDL_GetError()));
	}
}

SoundPlayer::~SoundPlayer()
{
	if (stream)
	{
		SDL_DestroyAudioStream(stream);
		stream = nullptr;
	}

	for (const auto& sound : sounds)
	{
		SDL_free(sound.data);
	}
}

void SoundPlayer::play(SoundId id) const
{
	if (!stream) return;

	const SoundData& sound = sounds[static_cast<size_t>(id)];

	if (SDL_GetAudioStreamQueued(stream) < static_cast<int>(sound.length))
	{
		SDL_PutAudioStreamData(stream, sound.data, sound.length);
	}
}

void SoundPlayer::loadSound(SoundId id, const char* filePath)
{
	SoundData sound;
	if (!SDL_LoadWAV(filePath, &sound.spec, &sound.data, &sound.length))
	{
		throw std::runtime_error(std::format("Failed to load WAV {}: {}", filePath, SDL_GetError()));
	}
	sounds[static_cast<size_t>(id)] = sound;
}
