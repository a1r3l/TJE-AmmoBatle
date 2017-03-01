#ifndef ENTITY_H
#define ENTITY_H

#include "framework.h"
#include <vector>
#include "extra\coldet.h"
#include "Managers\SoundManager.h"

class Camera;

class Entity {

public:
	//Constructor 
	Entity();
	virtual ~Entity();

	//Metodos
	virtual void render(Camera * camera);
	virtual void update(float et);
	void addChild(Entity* child);
	
	//hacemos un pushback dentro del vector to_destroy
	void removeChild(Entity* child);
	void destroyChilds();

	void move(Vector3 vel) { localMatrix.traslateLocal(vel.x,vel.y,vel.z); }
	void rotate(float angle, Vector3 v) { localMatrix.rotateLocal(angle * DEG2RAD, v); }
	/*
		while(size>0) vector[0] erase. 
	*/
	Matrix44 getGlobalMatrix();

	//Atributos generadles de las Entityes
	std::string entityName;
	Matrix44 localMatrix;
	Matrix44 globalMatrix;
	std::vector <Entity*> children;
	std::vector <Entity*> to_destroy;
	Entity * parent;
};

class Mesh;
class Texture;
class EntityMesh : public Entity {
	
public:
		EntityMesh();
		virtual ~EntityMesh();
		
		void render(Camera * camera);
		void update(float et);
		void setup(const char * meshName = NULL, const char * textureName = NULL, const char * lowMesh = NULL);

		//Atributos especiales de las Meshes
		Mesh * mesh;
		Mesh * low_mesh;
		Texture * meshTexture;
		bool clip_test = true;
		
		bool two_sided = false;
		bool alphaContent = false;

		CollisionModel3D * collModel = newCollisionModel3D();
};

class EntityCollider : public EntityMesh {
public:

	EntityCollider();
	float LIVE;
	bool dead;
	bool ammo;
	bool turret;
	void setup(const char * meshName = NULL, const char * textureName = NULL, const char * lowMesh = NULL);
	void update(float dt);
	void render(Camera * cam);

	CollisionModel3D * collModel = newCollisionModel3D();
};

class Fighter : public EntityCollider {
public:
	Fighter();
	~Fighter();

	float speed;
	float coolDown;
	Vector3 velocity;

	void update(float dt);
	void setup(const char * meshName, const char * textureName, const char * lowMesh);
	void shoot();
};

class Ammonition : public EntityCollider {
public:
	Ammonition();
	 ~Ammonition();

	void setup(const char * meshName, const char * textureName, const char * lowMesh);
};

class Turret : public EntityCollider {
public:
	Turret();
	~Turret();
	
	Vector3 velocity;
	float coolDown;

	void setup(const char * meshName, const char * textureName, const char * lowmesh);
	void shoot();
};

//MANAGER DE COLISIONESS!!
class BulletManager;
class CollisionManager{
public:
	CollisionManager();
	static CollisionManager* getInstance() {
		if (collManager == NULL) {
			collManager = new CollisionManager();
			return collManager;
		}
		else return collManager;
	}
	
	std::vector<EntityCollider*> static_entities;
	std::vector<EntityCollider*> dynamic_entities;
	std::vector<EntityCollider*> ammo_boxes;
	std::vector<EntityCollider*> turrets;

	BulletManager * bullets;
	//Funcion para detectar las colisiones
	void CollisionDetection();
	//Collision contra estaticos y 
	void onCollision(EntityCollider * dynamicEnt, EntityCollider * dynamicEnt2);
	//On bullet Collision
	void onBulletCollision(float p[], EntityCollider * staticEnt, EntityCollider * dynamicEnt);

	//Saber si las ammos aun tienen defensa
	bool CollisionManager::turretsDestroyed(std::string i);
private:
	static CollisionManager * collManager;
};
#endif