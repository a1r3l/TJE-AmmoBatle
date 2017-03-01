#ifndef  GAME_MANAGER_H
#define	 GAME_MANAGER_H

#include "../Estados/GameState.h"
#include "../includes.h"
#include "../framework.h"

class GameManager {

public:

	
	SDL_Window* window;
	int window_width;
	int window_height;
	float time;


	//keyboard state
	const Uint8* keystate;
	//mouse state
	int mouse_state; //tells which buttons are pressed
	Vector2 mouse_position; //last mouse position
	Vector2 mouse_delta; //mouse movement in the last frame
	bool mouse_locked; //tells if the mouse is locked (not seen)


	void setup(SDL_Window *window);
	void update(float et);
	void render();

	//void keyPressed(int key);
	//void keyReleased(int key);
	//void mouseMoved(int x, int y);
	//void mouseDragged(int x, int y, int button);
	//void mousePressed(int x, int y, int button);
	//void mouseReleased(int x, int y, int button);
	//void mouseEntered(int x, int y);
	//void mouseExited(int x, int y);
	//void windowResized(int w, int h);

	void changeState(GameState* state);

private:

	GameState * gameState;
};
#endif // ! GAME_MANAGER_H