#ifndef IA_H
#define IA_H

#include "utils.h"
#include <vector>

class Fighter;

class IA
{
public:
	IA();
	IA( Vector3 * );
	IA( Fighter *, Vector3 * = NULL );
	IA( Fighter *, Vector3 *, const std::vector<Vector3> );

	virtual ~IA();

	void update( float );
	void setIA( Fighter * );
	void addWaypoint( const Vector3 );
	void setVictim( Vector3 * ); //No fa pas falta
	Fighter * getVictim();

private:
	int i;
	bool perseguir;
	Vector3 * victim;
	Fighter * player;
	void carregarWaypoints(); //CONST EN UNA FUNCIÓ NO ALTERA CAP ELEMENT DE LA CLASSE.
	std::vector<Vector3> WayPoints;

	/* DESPRÉS AIXÓ HO VULL FER AMB UN STATE */
	void atacar();
	void patrullar();
};
#endif