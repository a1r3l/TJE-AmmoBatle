#ifndef INTRO_STATE_H
#define INTRO_STATE_H

#include "GameState.h"
#include "MenuState.h"
#include "../framework.h"
#include "../camera.h"
#include "../mesh.h"

class IntroState : public GameState {

public:
	IntroState();
	~IntroState();

	static IntroState* getInstance() {
		if (introState == NULL) {
			introState = new IntroState();
			return introState;
		}
		else return introState;
	}

	void init(GameManager* gameManager);
	void update(float et);
	void render(Camera * cam);
	
	EntityMesh * introQuad;

private:
	static IntroState * introState;
};

#endif // !INTRO_STATE_H