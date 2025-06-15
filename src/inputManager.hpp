#pragma once

#include <SDL3/SDL.h>
#include <unordered_map>

class InputManager
{
public:
	void handleEvent(const SDL_Event& e);

	bool isKeyPressed(SDL_Keycode key) const;
	bool isKeyReleased(SDL_Keycode key) const;
	bool isKeyHeld(SDL_Keycode key) const;

	void clear(); // To be called every frame to reset per-frame flags

private:
	std::unordered_map<SDL_Keycode, bool> mKeyHeld;
	std::unordered_map<SDL_Keycode, bool> mKeyPressed;
	std::unordered_map<SDL_Keycode, bool> mKeyReleased;
};
