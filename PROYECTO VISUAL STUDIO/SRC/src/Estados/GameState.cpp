#include "GameState.h"
#include "../Managers/GameManager.h"


GameState::GameState() {
}
GameState::~GameState() {
}

void GameState::changeState(GameState* gameState) {
	gameManager->changeState(gameState);
}