#include "IntroState.h"
#include "../texture.h"

IntroState * IntroState::introState = NULL;

IntroState::IntroState() {

}

IntroState::~IntroState() {

}

void IntroState::init(GameManager* gManager) {
	this->gameManager = gManager;
	
	cam = new Camera();	
	cam->setOrthographic(0, 600, 0, 800, 0.1, 2);
	cam->set();
	
	introQuad = new EntityMesh();

	introQuad->mesh = new Mesh();
	introQuad->mesh->createQuad(
	introQuad->meshTexture = new Texture(gameManager->window_width, );

	introQuad->meshTexture = TextureManager::getInstance()->getTexture("images/loading");
}

void IntroState::update(float et) {

}

void IntroState::render(Camera * cam) {
	introQuad->render(cam);
}