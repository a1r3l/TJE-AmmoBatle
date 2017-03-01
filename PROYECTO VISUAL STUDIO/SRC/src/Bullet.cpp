#include "Bullet.h"
#include "Mesh.h"
#include "includes.h"

BulletManager * BulletManager::instance = NULL;

BulletManager::BulletManager() 
{
	instance = this;
	memset( bullets_vector, 0, sizeof( Bullet) * NUM_BULL );
}

void BulletManager::render(Camera * camera) 
{
	Mesh bullMesh;
	
	for (int i = 0; i < NUM_BULL; ++i) 
	{
		Bullet& bull = bullets_vector[i];

		if (bull.TTL <= 0) 
			continue;

		bullMesh.vertices.push_back(bull.position);
		bullMesh.vertices.push_back(bull.last_position);

		bullMesh.colors.push_back( Vector4(1, 1, .3, 1) );
		bullMesh.colors.push_back( Vector4(0, 0, 0, 1) );
	}
		
	if (bullMesh.vertices.size() == 0)
		return;

	glLineWidth(3);
    bullMesh.render(GL_LINES);
}

void BulletManager::update(float elapsedTime) 
{
	for (int i = 0; i < NUM_BULL; ++i) 
	{
		Bullet& bull = bullets_vector[i];
		
		if (bull.TTL <= 0) 
			continue;

		bull.TTL -= elapsedTime;
		bull.last_position = bull.position;
		bull.position = bull.position + bull.velocity * elapsedTime;
	}
}

void BulletManager::createBullet(Vector3 pos, Vector3 vel, float TTL, void* author) 
{
	for (int i = 0; i < NUM_BULL; ++i) 
	{
		Bullet&  bull = bullets_vector[i];
		
		if (bull.TTL > 0) 
			continue;

		//std::cout << "Creo Bala: "<< i << std::endl;

		bull.TTL = TTL;
		bull.position = pos;
		bull.velocity = vel;
		bull.author = author;
		bull.last_position = pos;
		
		return;
	}
}
