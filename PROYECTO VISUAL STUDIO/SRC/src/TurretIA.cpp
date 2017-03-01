#include "TurretIA.h"
#include "Entity.h"

TurretIA::TurretIA() {}

TurretIA::TurretIA(Turret * turret, Fighter * player) {
	this->player = player;
	this->turret = turret;
}

TurretIA::~TurretIA() {

}


void TurretIA::update(float) {
	
	Vector3 target_position;
	//Les nostres coordenades.
	Matrix44 global_matrix = player->getGlobalMatrix();
	//Para calcular el ángulo necesitamos el vector que va desde la entidad hasta el objetivo
	Vector3 to_target = global_matrix.getTranslation() - target_position;
	//Para calcular un ángulo tomamos el vector front de la entidad y to_target y hacemos el producto escalar.
	Vector3 front = global_matrix.rotateVector(Vector3{ 0, 0, 1 });
	to_target.normalize();
	front.normalize();
}


void TurretIA::setIA(Fighter * player) {

}

Fighter * TurretIA::getVictim() {


}