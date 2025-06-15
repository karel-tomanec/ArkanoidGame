#include "inputManager.hpp"

void InputManager::handleEvent(const SDL_Event& e)
{
	if (e.type == SDL_EVENT_KEY_DOWN && !e.key.repeat)
	{
		mKeyHeld[e.key.key] = true;
		mKeyPressed[e.key.key] = true;
	}
	else if (e.type == SDL_EVENT_KEY_UP)
	{
		mKeyHeld[e.key.key] = false;
		mKeyReleased[e.key.key] = true;
	}
}

bool InputManager::isKeyPressed(SDL_Keycode key) const
{
	return mKeyPressed.contains(key) && mKeyPressed.at(key);
}

bool InputManager::isKeyReleased(SDL_Keycode key) const
{
	return mKeyReleased.contains(key) && mKeyReleased.at(key);
}

bool InputManager::isKeyHeld(SDL_Keycode key) const
{
	return mKeyHeld.contains(key) && mKeyHeld.at(key);
}

void InputManager::clear()
{
	mKeyPressed.clear();
	mKeyReleased.clear();
}
