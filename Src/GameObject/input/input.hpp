#pragma once
#include <SDL3/SDL.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_keyboard.h>
#include <unordered_map>
namespace IN {
	class InputState {
	public:
        std::unordered_map<SDL_Keycode, bool> keysDown;
        int mouseX = 0;
        int mouseY = 0;
        std::unordered_map<uint8_t, bool> mouseButtons;
	};

    class InputSystem {
    public:
    private:
    };
}