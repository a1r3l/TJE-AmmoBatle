#include "GameManager.h"
#include "../Estados/IntroState.h"

void GameManager::setup(SDL_Window * window) {
	// We set the instance to introduction
	this->window = window;
	SDL_GetWindowSize(window, &window_width, &window_height);
	changeState(IntroState::getInstance());
	keystate = NULL;
	mouse_locked = false;
}

void GameManager::update(float et) {
	gameState->update(et);
}

void GameManager::render() {
	gameState->render(gameState->cam);
}

void GameManager::changeState(GameState* state) {
	if (state != gameState) {
		gameState = state;

		// We send a reference to the application manager
		gameState->init(this);
	}
}
