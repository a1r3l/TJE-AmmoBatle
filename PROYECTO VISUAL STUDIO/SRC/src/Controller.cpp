#include "Controller.h"
#include "Camera.h"
#include "Mesh.h"
#include "game.h"

Controller * Controller::instance = NULL;

Controller::Controller()
{
	player_camera = new Camera();
}

Controller::~Controller() {
	delete player_camera;
}

void Controller::update(float dt) 
{
	//PRINT PER SABER LA POSICION DE AVION
	//Vector3 pos = player->getGlobalMatrix() * player->mesh->center;
	//std::cout << "POSICION: " <<"X:"<< pos.x <<"Y:" << pos.y << "Z:" << pos.z << std::endl;

	//Rotació sobre Z
	if (Game::instance->keystate[SDL_SCANCODE_E])	player->rotate(90 * dt, Vector3(0, 0, 1));
	if (Game::instance->keystate[SDL_SCANCODE_Q])	player->rotate(-90 * dt, Vector3(0, 0, 1));

	//Rotació sobre X
	if (Game::instance->keystate[SDL_SCANCODE_W] || Game::instance->keystate[SDL_SCANCODE_UP])		player->rotate(90 * dt, Vector3(1, 0, 0));
	if (Game::instance->keystate[SDL_SCANCODE_S] || Game::instance->keystate[SDL_SCANCODE_DOWN])	player->rotate(-90 * dt, Vector3(1, 0, 0));

	//Rotació sobre Y
	if (Game::instance->keystate[SDL_SCANCODE_A] || Game::instance->keystate[SDL_SCANCODE_LEFT])	player->rotate(-90 * dt, Vector3(0, 1, 0));
	if (Game::instance->keystate[SDL_SCANCODE_D] || Game::instance->keystate[SDL_SCANCODE_RIGHT])	player->rotate(90 * dt, Vector3(0, 1, 0));

	//Disparar
	if (Game::instance->keystate[SDL_SCANCODE_Z] || Game::instance->keystate[SDL_SCANCODE_SPACE])	player->shoot();
	
	//Accelerar
	if (Game::instance->keystate[SDL_SCANCODE_R]) 
	{
		player->speed = 200;
	}
	else {
		
		player->speed = 100;
	}

	if (pad) {
		JoystickState pad_state = getJoystickState(pad);
		
		if (Game::instance->GAMESTATE == 0) {
			if (pad_state.button[A_BUTTON]) {
				Game::instance->GAMESTATE++;
			}
		}	
		
		if (abs(pad_state.axis[LEFT_ANALOG_Y])> 0.3)	player->rotate(100 * pad_state.axis[LEFT_ANALOG_Y] * dt, Vector3(-1, 0, 0));
		if (abs(pad_state.axis[LEFT_ANALOG_X])> 0.3)	player->rotate(100 * pad_state.axis[LEFT_ANALOG_X] * dt, Vector3(0, 0, 1));
		
		if (abs(pad_state.axis[RIGHT_ANALOG_X])> 0.3)	player->rotate(100 * pad_state.axis[RIGHT_ANALOG_X] * dt, Vector3(0, 1, 0));
		
		if (pad_state.button[RIGHT_ANALOG_BUTTON])				player->rotate(100 * dt, Vector3(0, 1, 0));
		if (pad_state.button[LEFT_ANALOG_BUTTON])				player->rotate(100 * dt, Vector3(0, -1, 0));
		if (pad_state.axis[TRIGGERS_RT] > 0)					player->shoot();
		
		if (pad_state.button[A_BUTTON])	player->speed = 200;
		else player->speed = 100;

	}

	if ((Game::instance->mouse_state & SDL_BUTTON_LEFT) || Game::instance->mouse_locked) //is left button pressed?
	{
		player->rotate(0.20 * Game::instance->mouse_delta.x, Vector3(0, 0, 1));
		player->rotate(0.20 * Game::instance->mouse_delta.y, Vector3(1, 0, 0));
	}

	Matrix44 player_global = player->getGlobalMatrix();

	float z = player->mesh->halfsize.z * 2;
	float y = player->mesh->halfsize.y * 2;

	player_camera->lookAt(player_global*Vector3(0, y, z), player_global*Vector3(0, 0, -z * 10), player_global.rotateVector(Vector3(0, y, 0)));
}

void Controller::setController(Fighter * pl) {
	player = pl;

	float z = player->mesh->halfsize.z;
	float y = player->mesh->halfsize.y;
	
	float ww = Game::instance->window_width;
	float wh = Game::instance->window_height;

	player_camera->lookAt(player->getGlobalMatrix()*Vector3(0, y, z), player->getGlobalMatrix()*Vector3(), Vector3(0, y, 0));
	player_camera->setPerspective(70,ww/(float)wh, 1, 100000);

	pad = openJoystick(0);
}
