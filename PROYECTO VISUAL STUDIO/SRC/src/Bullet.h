#ifndef BULLET_H
#define BULLET_H
#define NUM_BULL 1024

#include "framework.h"
#include <vector>

class Bullet {

public:
	Bullet() {};
	
	Vector3 position;
	Vector3 last_position;
	Vector3 velocity;
	float TTL;
	float power;
	void* author;
	char type;
};

class Camera;
class BulletManager {

public:
	static BulletManager * instance;
	Bullet bullets_vector[NUM_BULL];

//public:
	BulletManager();

	static BulletManager * getInstance() {
		if (instance == NULL) instance = new BulletManager();
		return instance;
	};

	void render(Camera * camera);
	void update(float elapsedTime);
	void createBullet(Vector3 pos, Vector3 vel, float TTL, void* author);
};

#endif 