#ifndef GAME_STATE_H
#define	GAME_STATE_H

class GameManager;
class Camera;
class GameState {

public:
	GameState();
	virtual ~GameState();

	virtual void init(GameManager * gameManager) = 0;
	virtual void update(float et) = 0;
	virtual void render(Camera * cam) = 0;

	void changeState(GameState * gameState);

	Camera * cam;
	GameManager * gameManager;
};
#endif // !GAME_STATE_H