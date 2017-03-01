#ifndef WORLD_H
#define WORLD_H

#include "utils.h"
#include "mesh.h"
#include "texture.h"
#include "Entity.h"
#include "rendertotexture.h"
#include "shader.h"
#include <cmath>
#include "Bullet.h"
#include "Camera.h"
#include "Controller.h"
#include "Estados\GameState.h"
#include "IA.h"
#include "includes.h"

//class Camera;
class World {

public:

	World();
	//~World();
	static  World * getInstance() {
		if (instance == NULL) instance = new World();
		return instance;
	};

	bool init();

	// mejorr desdesde el root
	void addSceneChild(Entity * child);
	void addDynamicEntity(EntityCollider * dinamic);
	void addStaticEntity(EntityCollider * stEntity);
	void addAmmoEntity(EntityCollider * ammo);
	void addTurretEntity(EntityCollider * turret);
	
	//mejor que lo hagammos desde fuera
	void render(Camera * camera);

	bool setSkybox(const char* name, const char* texture);
	Camera* createCamera(Vector3 eye, Vector3 center, Vector3 up);
	void * createEntity(const char * name);
	void destroyDeleteables();
	
	//keyboard state
	const Uint8* keystate;

	//mouse state
	int mouse_state; //tells which buttons are pressed
	Vector2 mouse_position; //last mouse position
	Vector2 mouse_delta; //mouse movement in the last frame
	bool mouse_locked; //tells if the mouse is locked (not seen)

	//SDL_Joystick * pad;

	Entity * sceneRoot;

	EntityMesh * skybox;
	SoundManager * soundManager;
	BulletManager * bulletmanager;
	
	CollisionManager * collisionManager;
	Controller * controller;
	

	Fighter * player;
	Fighter * player2;
	Fighter * player3;
	IA * ia;
	IA * ia0;


	//CAMERAS
	Camera* free_camera; //our global camera
	Camera* player_camera;
	Camera* current_cam;

	std::vector<IA*> artificialIntelligence;
	std::vector<Turret*> artificialTurrets;
	
	
	void update(float elapsedtime);
	void renderHUD();
	void genericRender(std::string render);
	void renderIntro();
	void renderGameOver();
	void renderVictory();
	void renderNewGameMenu();
	void printString(float x, float y, float z, void* font, float r, float g, float b, char * string);
	//GameState * currentGameState;

	void loadScene();
	//Variables del WORLD

	//Contador para printar mensaje 
	float printMessage;
	float turretMessage;
	float ammoMessage;
	float enemyMessage;
	double elapsedspeed;

	int ammoDistroyed;

	float initTime;
	float finalTime;

private:
	static World * instance;
};

#endif