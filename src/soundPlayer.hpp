#pragma once
#include <format>
#include "SDL3/SDL.h"
#include <array>



class SoundPlayer final
{
public:

	SoundPlayer();
	~SoundPlayer();

	enum class SoundId : uint8_t
	{
		Enter,
		Start,
		Bounce,
		Fall,
		HitWall,
		Click,
		Win,
		GameOver,
		Count
	};

	void play(SoundId id) const;

private:

	struct SoundData
	{
		SDL_AudioSpec spec;
		Uint8* data = nullptr;
		Uint32 length = 0;
	};

	std::array<SoundData, static_cast<size_t>(SoundId::Count)> sounds;
	SDL_AudioStream* stream = nullptr;

	void loadSound(SoundId id, const char* filePath);
};
