#include "game.h"
#include "IA.h"

/*GAMESTATES
0-> INTRO STRATE
1->MENU->NEWGAME STATE
2->MENU->QUITGAME STATE
3->GAMEPADCONTROLS
4->INGAME ACTIONS
5->PLAYSTATE
6->GAMEOVERSTATE
7->VICTORY STATE
8->PAUSE STATE
*/



Shader* shader = NULL;
float angle = 0;
RenderToTexture* rt = NULL;
Game* Game::instance = NULL;

//SDL_Joystick * pad = NULL;
//Fighter * player3 = NULL;
//Fighter * player2 = NULL;
//IA * ia = NULL;
//IA * ia0 = NULL;

//double elapsedspeed = 1;

Game::Game(SDL_Window* window)
{
	this->window = window;
	instance = this;
	// initialize attributes
	// Warning: DO NOT CREATE STUFF HERE, USE THE INIT 
	// things create here cannot access opengl
	SDL_GetWindowSize( window, &window_width, &window_height );
	std::cout << " * Window size: " << window_width << " x " << window_height << std::endl;
	sceneTree = World::getInstance();
	
	//keystate = NULL;
	//mouse_locked = false;
}

//Here we have already GL working, so we can create meshes and textures
void Game::init(void)
{
	loaded = false;
	GAMESTATE = 0;
	Apressed = 6;
	std::cout << " * Path: " << getPath() << std::endl;
	SDL_Joystick * pad = NULL;
	//OpenGL flags
	glEnable( GL_CULL_FACE ); //render both sides of every triangle
	glEnable( GL_DEPTH_TEST ); //check the occlusions using the Z buffer
	if (sceneTree->init()) std::cout << "MUNDO CARGADO" << std::endl;
}

//what to do when the image has to be draw
void Game::render(void)
{
	glClearColor(0.227, 0.360, 0.557, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//IntroState
	if (GAMESTATE == 0) {
		sceneTree->genericRender("intro2.tga");
		return;
	}
	
	//NewMenuState
	if (GAMESTATE == 1) {
		sceneTree->genericRender("menu_new.tga");
		return;
	}
	
	//QuitMenuState
	if (GAMESTATE == 2) {
		sceneTree->genericRender("menu_quit.tga");
		return;
	}
	
	//PadState
	if (GAMESTATE == 3) {
		sceneTree->genericRender("gamepad.tga");
		return;
	}

	//InstructionsState
	if (GAMESTATE == 4) {
		sceneTree->genericRender("instructions.tga");
		return;
	}

	//Put the camera matrices on the stack of OpenGL (only for fixed rendering)
	sceneTree->current_cam->set();
	
	//Renderizo el mundo GAMESTATE
	if (GAMESTATE == 5) {
		sceneTree->render(sceneTree->current_cam);
	}

	//GameOverState
	if (GAMESTATE == 6) {
		sceneTree->genericRender("gameOver.tga");
		return;
	}

	//GameWinSatte
	if (GAMESTATE == 7) {
		sceneTree->renderVictory();
		return;
	}

	//PAUSE SATE
	if (GAMESTATE == 8) {
		return;
	}
	
#ifdef _DEBUG
	glColor3f(1.0, 0.0, 0.0);

	glBegin(GL_LINES);

	glLineWidth(9);

	Vector3 meu = player->getGlobalMatrix() * player->mesh->center;
	Vector3 teu = player2->getGlobalMatrix() * player2->mesh->center;

	glVertex3f(meu.x, meu.y, meu.z);
	glVertex3f(teu.x, teu.y, teu.z);

	teu = player3->getGlobalMatrix() * player3->mesh->center;

	glVertex3f(meu.x, meu.y, meu.z);
	glVertex3f(teu.x, teu.y, teu.z);

	glColor3f(1.0, 1.0, 1.0);
	glEnd();

#endif // _DEBUG

	glDisable( GL_BLEND );
	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);
}

void Game::update(double seconds_elapsed)
{
	//INTROSTATE
	if (GAMESTATE == 0) {
		SoundManager::getInstance()->startStream("intro.mp3", false);
			if (pad) {
				JoystickState pad_state = getJoystickState(pad);
				if (pad_state.button[A_BUTTON] && Apressed > 100) {
					Game::instance->GAMESTATE++;
					Apressed = 0;
				}
			}
	}
	//MENU_NEW_STATE
	if (GAMESTATE == 1) {
		if (pad) {
			JoystickState pad_state = getJoystickState(pad);
			
			if (pad_state.button[A_BUTTON] && Apressed > 100) {
				Game::instance->GAMESTATE = 3;
				Apressed = 0;			
			}

			if (pad_state.button[DOWN_BUTTON]) {
				Game::instance->GAMESTATE++;
			}
		}
	}
	//MENUQUIT_STATE
	if (GAMESTATE == 2) {
		
		if (pad) {
			JoystickState pad_state = getJoystickState(pad);

			if (pad_state.button[A_BUTTON] && Apressed > 100) {
				exit(0);
				Apressed = 0;
			}

			if (pad_state.button[UP_BUTTON]) {
				Game::instance->GAMESTATE--;
			}
		}
	}
	//GAMEPAD_STATE
	if (GAMESTATE == 3) {
		if (pad) {
			JoystickState pad_state = getJoystickState(pad);
			if (pad_state.button[A_BUTTON] && Apressed > 100) {
				Game::instance->GAMESTATE++;
				Apressed = 0;
			}
		}
	}

	//INFORMATION_STATE
	if (GAMESTATE == 4) {
		if (pad) {
			JoystickState pad_state = getJoystickState(pad);
			if (pad_state.button[A_BUTTON] && Apressed > 100) {
				SoundManager::getInstance()->stopStream("intro.mp3");
				Game::instance->GAMESTATE++;
				World::getInstance()->initTime = getTime();
				Apressed = 0;
			}
		}
	}
	//GAMESTATE
	if (GAMESTATE == 5) {
		SoundManager::getInstance()->startStream("ingame.wav", false);
		sceneTree->update(seconds_elapsed);
		//IF PRESS START GOT TO PAUSE;
	}
	
	//GAMEOVERSTATE
	if (GAMESTATE == 6) {
		//sceneTree->update(seconds_elapsed);
		SoundManager::getInstance()->stopStream("ingame.wav");
		SoundManager::getInstance()->stopStream("spitfire.wav");
		if (pad) {
			JoystickState pad_state = getJoystickState(pad);
			if (pad_state.button[A_BUTTON] && Apressed > 100) {
				exit(0);
				Game::instance->GAMESTATE = 2;
				Apressed = 0;
			}
		}
	}
	
	//WINSTATE
	if (GAMESTATE == 7) {
		//sceneTree->update(seconds_elapsed);
		SoundManager::getInstance()->stopStream("ingame.wav");
		SoundManager::getInstance()->stopStream("spitfire.wav");

		if (pad) {
			JoystickState pad_state = getJoystickState(pad);
			if (pad_state.button[A_BUTTON] && Apressed > 100) {
				exit(0);
				Game::instance->GAMESTATE = 2;
				Apressed = 0;
			}
		}
	}

	//PAUSESTATE
	if (GAMESTATE == 8) {
		//sceneTree->update(seconds_elapsed);
	}

	Apressed ++;
}

//Keyboard event handler (sync input)
void Game::onKeyPressed( SDL_KeyboardEvent event )
{
	switch(event.keysym.sym)
	{
		case SDLK_ESCAPE: 
			exit(0); //ESC key, kill the app
		break;
		
		case SDLK_TAB:
			if (sceneTree->current_cam == sceneTree->controller->player_camera) {
				sceneTree->elapsedspeed = 0.02;
				sceneTree->free_camera->lookAt(sceneTree->controller->player_camera->eye, sceneTree->controller->player_camera->center, sceneTree->controller->player_camera->up);
				sceneTree->current_cam = sceneTree->free_camera;
			}
			else if (sceneTree->current_cam == sceneTree->free_camera) {
				sceneTree->elapsedspeed = 1;
				sceneTree->current_cam = sceneTree->controller->player_camera;
			}
		break;
	}
}


void Game::onMouseButton( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_MIDDLE) //middle mouse
	{
		mouse_locked = !mouse_locked;
		SDL_ShowCursor(!mouse_locked);
	}
}

void Game::setWindowSize(int width, int height)
{
    std::cout << "window resized: " << width << "," << height << std::endl;

	glViewport( 0,0, width, height );
	sceneTree->free_camera->aspect =  width / (float)height;
	window_width = width;
	window_height = height;
}

