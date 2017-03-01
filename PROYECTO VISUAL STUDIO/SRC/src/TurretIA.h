#ifndef TURRETIA_H
#define TURRETIA_H

#include "utils.h"
#include "includes.h"

class Fighter;
class Turret;

class TurretIA  {

public:
	TurretIA();
	TurretIA(Turret * turret,Fighter* player);

	virtual ~TurretIA();

	void update(float);

	void setIA(Fighter * player);

	Fighter * getVictim();
	
	
private:
	
	Fighter * player;
	Turret * turret;

	/* DESPRÉS AIXÓ HO VULL FER AMB UN STATE */
	//void atacar();
};
#endif // !TURRETIA_H