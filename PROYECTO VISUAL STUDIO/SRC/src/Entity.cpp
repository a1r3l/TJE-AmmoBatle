#include "Entity.h"
#include "Mesh.h"
#include "Texture.h"
#include "Camera.h"
#include "Bullet.h"
#include "game.h"

Entity::Entity() {
	parent = NULL;
}
Entity::~Entity() {
	//std::cout<< "He borrado un Entity"<<std::endl;
}

Matrix44 Entity::getGlobalMatrix() 
{	
	if (parent)

		this->globalMatrix = this->localMatrix * parent->getGlobalMatrix();
		
	else 
		
		globalMatrix = localMatrix;

	return globalMatrix;
}

void Entity::addChild(Entity* child) {
	children.push_back(child);
	child->parent = this;
}

void Entity::render(Camera * cam) {
	for (int i = 0; i < children.size(); i++)
	{
		children[i]->render(cam);
	}
}

void Entity::update(float et) {
	for (int i = 0; i < children.size(); i++)
	{
		children[i]->update(et);
	}
}

void Entity::removeChild(Entity* child) {
	to_destroy.push_back(child);
}

//cogelo con pinzas
void Entity::destroyChilds() {
	for (int i = 0; i < to_destroy.size(); i++) {
		if (to_destroy[i]->children.size() > 0) {
			for (int j = 0; j < to_destroy[i]->children.size(); j++)  
				to_destroy[i]->children.erase(to_destroy[i]->children.begin() + j );
		}
		delete to_destroy[i];
		to_destroy.erase(to_destroy.begin() + i);
	}
}

//FUNCIONES PARA ENTITY MESH

EntityMesh::EntityMesh() {
	mesh = low_mesh = NULL;
	meshTexture = NULL;
}

EntityMesh::~EntityMesh() {
	
	if (parent) {
		for (int j = 0; j < parent->children.size(); j++) {
			if (parent->children[j] == this) {
				parent->children.erase(parent->children.begin() + j);
			}
		}
	}
		parent = NULL;
	if (children.size() != 0) {
		for (int i = 0; i < children.size(); i++) {
			delete children[i];
		}
	}

}

void EntityMesh::render(Camera * cam) {
	
	if (mesh) {
		Matrix44 global_matrix = this->getGlobalMatrix();
		Vector3 center = global_matrix * mesh->center;
		if (clip_test || cam->testSphereInFrustum(center,mesh->halfsize.length()*5)) {
			//Activamos bind si hay textura
			if (meshTexture) meshTexture->bind();
			//desactivamos cullface en caso de que sea de dos caras
			if(two_sided) glDisable(GL_CULL_FACE);
			//Encendemos el BLENDING en caso de que tenga alpha 
			if (alphaContent) {
				glEnable(GL_BLEND);
				glDepthMask(GL_FALSE);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}
				glPushMatrix();
				global_matrix.multGL();
				if (center.distance(cam->eye) < mesh->halfsize.length() * 3)
				{
					mesh->render(GL_TRIANGLES);
				}
				else if (low_mesh) low_mesh->render(GL_TRIANGLES); 
					else {
						 mesh->render(GL_TRIANGLES);
					}

			if (alphaContent) {
				glDepthMask(GL_TRUE);
				glDisable(GL_BLEND);
			}
			if(two_sided) glEnable(GL_CULL_FACE);
			if (meshTexture) meshTexture->unbind();
				glPopMatrix();
		}
	}
	//propagamos hacia lo hijos
	for (int i = 0; i < children.size();i++) {
		children[i]->render(cam);
	}
}

void EntityMesh::update(float dt) {
	//Si el Entity es la helice la hacemos girar.
	//if (this->entityName == "helice")
		//localMatrix.rotateLocal(90 * DEG2RAD, Vector3(0, 0, 1));
	
	for (int i = 0; i < children.size(); i++) {
		children[i]->update(dt);
	}
}

void EntityMesh::setup(const char * meshName, const char * textureName, const char * lowMesh) {
	if(meshName)	mesh = MeshManager::getInstance()->getMesh(meshName);
	if(lowMesh)		low_mesh = MeshManager::getInstance()->getMesh(lowMesh);
	if(textureName)	meshTexture = TextureManager::getInstance()->getTexture(textureName);

	//Creamos la collisionModel
	if(mesh) {
		for (int i = 0; i < mesh->vertices.size(); i += 3) {
			//collModel->setTriangleNumber(mesh->vertices.size()/3);
			float point1[3] = {mesh->vertices[i].x,mesh->vertices[i].y,mesh->vertices[i].z};
			float point2[3] = {mesh->vertices[i+1].x,mesh->vertices[i+1].y,mesh->vertices[i+1].z};
			float point3[3] = {mesh->vertices[i+2].x,mesh->vertices[i+2].y,mesh->vertices[i+2].z};
			collModel->addTriangle(point1,point2,point3);
		}
		collModel->finalize();
	}
}


//ENTITY COLLIDER CODE
EntityCollider::EntityCollider() {
	mesh = low_mesh = NULL;
	meshTexture = NULL;
}

void EntityCollider::setup(const char * meshName, const char * textureName, const char * lowMesh) {
	
	if (meshName)		mesh = MeshManager::getInstance()->getMesh(meshName);
	if (lowMesh)		low_mesh = MeshManager::getInstance()->getMesh(lowMesh);
	if (textureName)	meshTexture = TextureManager::getInstance()->getTexture(textureName);

	//Creamos la collisionModel
	if (mesh) {
		for (int i = 0; i < mesh->vertices.size(); i += 3) 
		{
			float point1[3] = { mesh->vertices[ i ].x, mesh->vertices[ i ].y, mesh->vertices[ i ].z };
			float point2[3] = { mesh->vertices[i + 1].x, mesh->vertices[i + 1].y, mesh->vertices[i + 1].z };
			float point3[3] = { mesh->vertices[i + 2].x, mesh->vertices[i + 2].y, mesh->vertices[i + 2].z };

			collModel->addTriangle(point1, point2, point3);
		}
		collModel->finalize();
	}
}

void EntityCollider::update(float dt) 
{
	for (int i = 0; i < children.size(); i++)

		children[i]->update( dt );
}

void EntityCollider::render(Camera * cam) {
	if (mesh) {
		Matrix44 global_matrix = this->getGlobalMatrix();
		Vector3 center = global_matrix * mesh->center;
		if (clip_test || cam->testSphereInFrustum(center, mesh->halfsize.length() * 5)) {
			//Activamos bind si hay textura
			if (meshTexture) meshTexture->bind();
			//desactivamos cullface en caso de que sea de dos caras
			if (two_sided) glDisable(GL_CULL_FACE);
			//Encendemos el BLENDING en caso de que tenga alpha 
			if (alphaContent) {
				glEnable(GL_BLEND);
				glDepthMask(GL_FALSE);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}
			glPushMatrix();
			global_matrix.multGL();
			if (center.distance(cam->eye) < mesh->halfsize.length() * 3)
			{
				mesh->render(GL_TRIANGLES);
			}
			else if (low_mesh) low_mesh->render(GL_TRIANGLES);
			else {
				mesh->render(GL_TRIANGLES);
			}

			if (alphaContent) {
				glDepthMask(GL_TRUE);
				glDisable(GL_BLEND);
			}
			if (two_sided) glEnable(GL_CULL_FACE);
			if (meshTexture) meshTexture->unbind();
			glPopMatrix();
		}
	}
	//propagamos hacia lo hijos
	for (int i = 0; i < children.size(); i++) {
		children[i]->render(cam);
	}
}

Turret::Turret() {
	LIVE = 50;
	dead = false;
	collModel = NULL;
	two_sided = false;
	turret = true;
	ammo = false;
	coolDown = 51;

}

Turret::~Turret() {
	if (parent) {
		for (int j = 0; j < parent->children.size(); j++) {
			if (parent->children[j] == this) {
				parent->children.erase(parent->children.begin() + j);
			}
		}
	}

	std::vector<EntityCollider*> dynamic = Game::instance->sceneTree->collisionManager->turrets;
	for (int i = 0; i < dynamic.size(); i++) {
		if (dynamic[i] == this)
			Game::instance->sceneTree->collisionManager->turrets.erase(Game::instance->sceneTree->collisionManager->turrets.begin() + i);
	}

	parent = NULL;
	if (children.size() != 0) {
		for (int i = 0; i < children.size(); i++) {
			delete children[i];
		}
	}
}

void Turret::setup(const char * meshName, const char * textureName, const char * lowMesh)
{
	if (meshName)		mesh = MeshManager::getInstance()->getMesh(meshName);
	if (textureName)	meshTexture = TextureManager::getInstance()->getTexture(textureName);
	if (lowMesh)		low_mesh = MeshManager::getInstance()->getMesh(lowMesh);

	EntityMesh * meshEntity = new EntityMesh();
	meshEntity->entityName = "cañon";
	meshEntity->setup("turret_cannon.ASE", textureName);
	//Posiconar les èlices a davant de l'avió.
	Vector3 pos = this->mesh->center;
	//NIDA LA VERIATA
	meshEntity->localMatrix.setTranslation(pos.x, pos.y, pos.z);
	//L'afeigm a l'avió.
	this->addChild(meshEntity);
}

void Turret::shoot() {
	if (coolDown > 50) {
		Vector3 pos, vel;
		BulletManager *  bm = BulletManager::getInstance();
		Matrix44 global = this->getGlobalMatrix();
		//Velociatat de la bala
		vel = global.rotateVector(Vector3(0, 0, -1000));

		//Bala desde el CAÑON
		pos = global * Vector3(1.8, 0, -1.5);
		bm->createBullet(pos, vel, 5, this);

		SoundManager::getInstance()->startSound("shot.wav", true, 0);
		coolDown = 0;
	}
}
Ammonition::Ammonition() {
	LIVE = 100;
	dead = false;
	collModel = NULL;
	two_sided = true;
	turret = false;
	ammo = true;
}
Ammonition::~Ammonition() {

	if (parent) {
		for (int j = 0; j < parent->children.size(); j++) {
			if (parent->children[j] == this) {
				parent->children.erase(parent->children.begin() + j);
			}
		}
	}

	std::vector<EntityCollider*> dynamic = Game::instance->sceneTree->collisionManager->ammo_boxes;
	for (int i = 0; i < dynamic.size(); i++) {
		if (dynamic[i] == this)
			Game::instance->sceneTree->collisionManager->ammo_boxes.erase(Game::instance->sceneTree->collisionManager->ammo_boxes.begin() + i);
	}

	parent = NULL;
	if (children.size() != 0) {
		for (int i = 0; i < children.size(); i++) {
			delete children[i];
		}
	}
}

void Ammonition::setup(const char * meshName, const char * textureName, const char * lowMesh)
{
	if (meshName)		mesh = MeshManager::getInstance()->getMesh(meshName);
	if (textureName)	meshTexture = TextureManager::getInstance()->getTexture(textureName);
	if (lowMesh)		low_mesh = MeshManager::getInstance()->getMesh(lowMesh);
}



Fighter::Fighter() {
	LIVE = 100;
	speed = 100;
	dead = false;
	collModel = NULL;
	coolDown = 11;
	turret = false;
	ammo = false;
}

Fighter::~Fighter() {
	
	if (parent) {
		for (int j = 0; j < parent->children.size(); j++) {
			if (parent->children[j] == this) {
				parent->children.erase(parent->children.begin() + j);
			}
		}
	}
	
	std::vector<EntityCollider*> dynamic = Game::instance->sceneTree->collisionManager->dynamic_entities;
	for (int i = 0; i < dynamic.size(); i++) {
		if (dynamic[i] == this) 
			Game::instance->sceneTree->collisionManager->dynamic_entities.erase(Game::instance->sceneTree->collisionManager->dynamic_entities.begin() + i);
	}

	parent = NULL;
	if (children.size() != 0) {
		for (int i = 0; i < children.size(); i++) {
			delete children[i];
		}
	}
}

void Fighter::setup(const char * meshName, const char * textureName, const char * lowMesh) 
{
	if (meshName)		mesh = MeshManager::getInstance()->getMesh(meshName);
	if (textureName)	meshTexture = TextureManager::getInstance()->getTexture(textureName);
	if (lowMesh)		low_mesh = MeshManager::getInstance()->getMesh(lowMesh);
	//Creamos la helice de los aviones
	EntityMesh * meshEntity = new EntityMesh();
	meshEntity->entityName = "helice";
	meshEntity->setup("helice.ASE", "helice.TGA");
	//simprimeix pels dos costats
	meshEntity->two_sided = true;
	//Té alpha, transparencia
	meshEntity->alphaContent = true;
	//Posiconar les èlices a davant de l'avió.
	Vector3 pos = this->mesh->center;
	pos.z -= this->mesh->halfsize.z;
	//NIDA LA VERIATA
	meshEntity->localMatrix.setTranslation(pos.x, pos.y, pos.z);
	//L'afeigm a l'avió.
	this->addChild(meshEntity);
}


void Fighter::update(float dt) 
{
	move( Vector3( 0, 0, -( speed * dt * 2 ) ) );
	coolDown++;
	
	SoundManager::getInstance()->startStream("spitfire.wav",false);
	
	for (int i = 0; i < children.size(); ++i) 

		if (children[i]->entityName == "helice")

			children[i]->localMatrix.rotateLocal( 90 * speed * dt * DEG2RAD, Vector3(0, 0, 1) );
}


void Fighter::shoot() 
{
	if (coolDown > 20) {
		Vector3 pos, vel;
		BulletManager *  bm = BulletManager::getInstance();
		Matrix44 global = this->getGlobalMatrix();
		//Velociatat de la bala
		vel = global.rotateVector(Vector3(0, 0, -1000));

		//Bala des de la esquerra.
		pos = global * Vector3(1.8, 0, -1.5);
		bm->createBullet(pos, vel, 5, this);

		//Bala desde la ametralladora derecha.
		pos = global * Vector3(-1.9, 0, -1.5);
		bm->createBullet(pos, vel, 5, this);

		SoundManager::getInstance()->startSound("shot.wav", true, 0);
		coolDown = 0;
	}
}

//COLLLISION MANAGER
CollisionManager * CollisionManager::collManager = NULL;

CollisionManager::CollisionManager() {
	bullets = BulletManager::getInstance();
}

void CollisionManager::onCollision(EntityCollider * dynamicEnt, EntityCollider * dynamicEnt2) {
	if (dynamicEnt == World::getInstance()->player || dynamicEnt2 == World::getInstance()->player) {
		delete World::getInstance()->controller;
		World::getInstance()->current_cam = World::getInstance()->free_camera;
		SoundManager::getInstance()->startSound("explosion.wav", true, 1);
		Game::instance->GAMESTATE = 6;
	}
}

void CollisionManager::onBulletCollision(float p[], EntityCollider * staticEnt, EntityCollider * dynamicEnt) {
	
	//LOGICA DE COLLISION CON EL MAPA
	if (staticEnt) {
		//std::cout << "BulletCollision con:" << staticEnt->entityName << std::endl;
	}

	//LOGICA DE COLISION CON LOS AVIONES
	if (dynamicEnt) {
		if (dynamicEnt->LIVE > 0) dynamicEnt->LIVE -= 5;
		else if (dynamicEnt->LIVE <= 0 && !dynamicEnt->dead) {
			
			dynamicEnt->dead = true;
			if (dynamicEnt->turret) World::getInstance()->turretMessage = 3;
			
			if (dynamicEnt->ammo) {
				World::getInstance()->ammoMessage = 3;
				World::getInstance()->ammoDistroyed++;
			}

			if(!dynamicEnt->ammo && !dynamicEnt->turret && dynamicEnt != World::getInstance()->player) World::getInstance()->enemyMessage = 3;
			
			if (dynamicEnt == World::getInstance()->player) {
				delete World::getInstance()->controller;
				World::getInstance()->current_cam = World::getInstance()->free_camera;
				Game::instance->GAMESTATE = 6;
			}

			Game::instance->sceneTree->sceneRoot->removeChild(dynamicEnt);
			SoundManager::getInstance()->startSound("explosion.wav",true,1);
		}
		//std::cout << "BulletCollision con: " << dynamicEnt->entityName  << std::endl;
	}
}

bool CollisionManager::turretsDestroyed(std::string i) {
		std::string turret = "Turret"+i+"1";
		std::string turret2 = "Turret"+i+"2";
		bool muertas = true;

		for (int j = 0; j < turrets.size(); j++) {
			if (turrets[j]->entityName == turret || turrets[j]->entityName == turret2)
			{	
				muertas = false;
			}
		}
return muertas;
}

void CollisionManager::CollisionDetection()
{
	//Balas contra AMMOBOXES
	for (int r = 0; r < ammo_boxes.size(); r++) 
	{
		EntityCollider * ammoBox = ammo_boxes[r];
		Matrix44 globalMatrix = ammoBox->getGlobalMatrix();
		Vector3 center = globalMatrix * ammoBox->mesh->center;

		float radius = ammoBox->mesh->halfsize.length()/2;

		for (int b = 0; b < NUM_BULL; b++)
		{
			float p[3];
			Bullet * bull = &bullets->bullets_vector[b];

			if (bull->TTL > 0)

				if (SphereRayCollision(center.v, radius, bull->last_position.v, bull->position.v, p)) {
					
					if (ammoBox->entityName == "Ammonition1") {
						if (turretsDestroyed("1")) {
							onBulletCollision(p, NULL, ammoBox);
							bull->TTL = 0;
						}
						else {
							World::getInstance()->printMessage = 3;
							//std::cout << "DEBES MATAR PRIMERO LAS TORRETAS DE LA 1" << std::endl;
							bull->TTL = 0;
						}
					}

					if (ammoBox->entityName == "Ammonition2") {
						if (turretsDestroyed("2")) {
							onBulletCollision(p, NULL, ammoBox);
							bull->TTL = 0;
						}
						else {
							World::getInstance()->printMessage = 3;
							//std::cout << "DEBES MATAR PRIMERO LAS TORRETAS DE LA 2" << std::endl;
							bull->TTL = 0;
						}
					}
				}
		}
	}

	//BALAS CONTRA TORRETAS
	for (int r = 0; r < turrets.size(); r++)
	{
		EntityCollider * turret = turrets[r];
		Matrix44 globalMatrix = turret->getGlobalMatrix();
		Vector3 center = globalMatrix * turret->mesh->center;

		float radius = turret->mesh->halfsize.length() * 10;

		for (int b = 0; b < NUM_BULL; b++)
		{
			float p[3];
			Bullet * bull = &bullets->bullets_vector[b];

			if (bull->TTL > 0)

				if (SphereRayCollision(center.v, radius, bull->last_position.v, bull->position.v, p)) {
					onBulletCollision(p, NULL, turret);
					bull->TTL = 0;
				}
		}
	}

	//Balas contra entidades DINAMICAS
	for (int j = 0; j < dynamic_entities.size(); j++) 
	{
		EntityCollider * dynamicEnt = dynamic_entities[j];

		Matrix44 globalMatrix = dynamicEnt->getGlobalMatrix();
		Vector3 center = globalMatrix * dynamicEnt->mesh->center;

		float radius = dynamicEnt->mesh->halfsize.length()/2;

		for (int b = 0; b < NUM_BULL; b++) 
		{
			float p[3];
			Bullet * bull = &bullets->bullets_vector[b];
			
			if (bull->TTL > 0) 

				if (SphereRayCollision(center.v, radius, bull->last_position.v, bull->position.v, p) && bull->author != dynamicEnt) {
					onBulletCollision(p, NULL, dynamicEnt);
					bull->TTL = 0;
				}
		}
	}

	//Dynamics contra Dynamics contra Statics
	for (int m = 0; m < dynamic_entities.size(); m++) {
		EntityCollider * dynamicEnt = dynamic_entities[m];
		Matrix44 dynGlobal = dynamicEnt->getGlobalMatrix();
		Vector3 center = dynGlobal * dynamicEnt->mesh->center;
		float radius = dynamicEnt->mesh->halfsize.length();
		//Statics
		for (int n = 0; n < static_entities.size(); n++) {
			EntityCollider * staticEnt = static_entities[n];
			staticEnt->collModel->setTransform(staticEnt->getGlobalMatrix().m);
			if (staticEnt->collModel->sphereCollision(center.v, radius)) {
				onCollision(dynamicEnt,NULL);
			}
		}

	//Dynamics
		for (int r = 0; r < dynamic_entities.size(); r++) {
			EntityCollider * dynamicEnt2 = dynamic_entities[r];
			Matrix44 dynGlobal2 = dynamicEnt2->getGlobalMatrix();
			Vector3 center2 = dynGlobal2 * dynamicEnt2->mesh->center;
			float radius2 = dynamicEnt2->mesh->halfsize.length();
			if (dynamicEnt != dynamicEnt2) {
				float p[3];
				if (SphereSphereCollision(center.v, radius, center2.v, radius2, p))onCollision(dynamicEnt,dynamicEnt2);
			}
		}

		//Choque contra las cajas de municion
		for (int m = 0; m < ammo_boxes.size(); m++) {
			EntityCollider * ammo = ammo_boxes[m];
			Matrix44 dynGlobal2 = ammo->getGlobalMatrix();
			Vector3 center2 = dynGlobal2 * ammo->mesh->center;
			float radius2 = ammo->mesh->halfsize.length();
			if (dynamicEnt != ammo) {
				float p[3];
				if (SphereSphereCollision(center.v, radius, center2.v, radius2, p))onCollision(dynamicEnt, ammo);
			}
		}
		
		//Choque contra las cajas de municion
		for (int m = 0; m < turrets.size(); m++) {
			EntityCollider * turret = turrets[m];
			Matrix44 dynGlobal2 = turret->getGlobalMatrix();
			Vector3 center2 = dynGlobal2 * turret->mesh->center;
			float radius2 = turret->mesh->halfsize.length();
			if (dynamicEnt != turret) {
				float p[3];
				if (SphereSphereCollision(center.v, radius, center2.v, radius2, p))onCollision(dynamicEnt, turret);
			}
		}
	}
}