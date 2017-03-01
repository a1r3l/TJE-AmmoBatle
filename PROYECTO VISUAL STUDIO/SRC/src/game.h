/*  by Javi Agenjo 2013 UPF  javi.agenjo@gmail.com
	This class encapsulates the game, is in charge of creating the game, getting the user input, process the update and render.
*/

#ifndef GAME_H
#define GAME_H

#include "includes.h"
#include "camera.h"
#include "World.h"
#include "Managers\SoundManager.h"

class Game
{
public:
	static Game* instance;

	//window
	SDL_Window* window;
	int window_width;
	int window_height;
	//SoundManager * soundManager;
    float time;
	int GAMESTATE;
	bool loaded;
	//keyboard state
	SDL_Joystick * pad;
	const Uint8* keystate;
	int Apressed;

	//mouse state
	int mouse_state; //tells which buttons are pressed
	Vector2 mouse_position; //last mouse position
	Vector2 mouse_delta; //mouse movement in the last frame
	bool mouse_locked; //tells if the mouse is locked (not seen)
	
	World * sceneTree;
	
	std::vector<IA*> artificialIntelligence;

	Game(SDL_Window* window);
	void init( void );
	void render( void );
	void update( double dt );

	void onKeyPressed( SDL_KeyboardEvent event );
	void onMouseButton( SDL_MouseButtonEvent event );
    void onResize( SDL_Event e );
    
	void setWindowSize(int width, int height);
};


#endif 