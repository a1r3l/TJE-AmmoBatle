#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "utils.h"

class Camera;
class Fighter;

class Controller
{
public:
	Controller();
	~Controller();
	
	Camera * player_camera;
	Fighter * player;
	SDL_Joystick * pad;

	static Controller * getInstance() {
		if (instance == NULL) {
			instance = new Controller();
			return instance;
		}
		else return instance;
	}

	//const Uint8* keystate;
	//bool mouse_locked;

	void update(float dt);
	void setController(Fighter * pl);

private:
	static Controller * instance;
};
#endif


