#ifndef MENU_STATE_H
#define MENU_STATE_H

#include "GameState.h"
#include "../framework.h"

//#define STB_IMAGE_IMPLEMENTATION
//#include "../extra/stb_easyfont.h"

class Camera;
class MenuState : public GameState {

public:
	MenuState() {};
	~MenuState() {};

	static MenuState* getInstance() {
		if (menuState == NULL) {
			menuState = new MenuState();
			return menuState;
		}
		else return menuState;
	}

	void init(GameManager* gameManager);
	void update(float et);
	void render(Camera * cam);

	std::string  titleText;
private:
	static MenuState * menuState;

};

#endif