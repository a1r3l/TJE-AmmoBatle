#include "IA.h"
#include "Mesh.h"
#include "game.h"

/************************************************** IA **********************************************
 * Diferents constructors, tots són punters ja que necessitem les dades actualitzades.				*
 * () El primer només carrega els punts a segui.													*
 * ( _victim ) Ens diu aquí ha de seguir.															*
 * ( _player, _victim, _WayPoints ) Com el de dalt, peró a més 'especifica el waypoitns que es		*
 *		seguiràn, per tal de poder fer bé les dues illes.											*
 ****************************************************************************************************/

IA::IA() 
{
	carregarWaypoints();
}



IA::IA( Vector3 * _victim )
{
	victim = _victim;
	carregarWaypoints();
}



IA::IA( Fighter * _player, Vector3 * _victim )
{
	if( _victim )
		victim = _victim;

	player = _player;
	carregarWaypoints();
}

IA::IA(Fighter * _player, Vector3 * _victim, const std::vector<Vector3> _WayPoints)
{
	player = _player;
	victim = _victim;
	WayPoints = _WayPoints;
	perseguir = i = 0;
}

IA::~IA() {
	player = NULL;
	WayPoints.clear();
}

void IA::setIA( Fighter * _player )
{
	player = _player;
}

void IA::setVictim(Vector3 * _victim)
{
	victim = _victim;
}

void IA::addWaypoint(const Vector3 next)
{
	WayPoints.push_back( next );
}

/***************************************** carregarWaypoints **************************************** 
 * Ens dona el diferents putns a patrullar, tot i ens diu per a quin comencem i si estem perseguint	*
 * l'enemic.																						*
 ****************************************************************************************************/
void IA::carregarWaypoints()
{
	perseguir = i = 1;

	WayPoints.push_back( Vector3{ 9000, 600, 16000 } );
	WayPoints.push_back( Vector3{ 9000, 600, 30000 } );
	WayPoints.push_back( Vector3{ 18000, 600, 27000 } );
	WayPoints.push_back( Vector3{ 19000, 600, 14000 } );
}

void IA::update(float dt) 
{
	Vector3 target_position;

	if (!perseguir) //Les coordenades de la entity a la que volem apuntar.

		target_position = WayPoints.at(i);

	else

		target_position = *victim;

	Matrix44 global_matrix = player->getGlobalMatrix();	//Les nostres coordenades.

	//Para calcular el ángulo necesitamos el vector que va desde la entidad hasta el objetivo
	Vector3 to_target = global_matrix.getTranslation() - target_position;
	
	//Para calcular un ángulo tomamos el vector front de la entidad y to_target y hacemos el producto escalar.
	Vector3 front = global_matrix.rotateVector( Vector3{ 0, 0, 1 } );

	to_target.normalize();

	front.normalize();

	float angle = to_target.dot(front);			//L'angle que hem de girar.										PRODUCTE ESCALAR

	Vector3 axis_ws = to_target.cross(front);	//Els vectors en els que rotem, peró en		¡¡*¡¡GLOBAL!!*!!	PRODUCTE VECTORIAL

	//Para pasar de coordenadas de mundo a local usamos la inversa de la matriz model. 
	Matrix44 inv = global_matrix;				//inv = entity->model;

	inv.inverse();								//inv.inverse();

													//Sobre els eixos en els que rotem		¡¡*¡¡LOCALMENT!!*!!	
	Vector3 axis_ls = inv.rotateVector(axis_ws);	//axis = inv.rotateVector( axis );

	player->localMatrix.rotateLocal( ( 1.0 - angle ) * dt * 5.0, axis_ls);		//Finament rotem localment amb l'angle i els  vectors.

	Vector3 top_ws = global_matrix.rotateVector(Vector3{ 0, 1, 0 });			//Manternir-lo amb la cabina cap amunt

	angle = top_ws.dot( Vector3{ 0, 1, 0 } );									//Manternir-lo amb la cabina cap amunt

	player->localMatrix.rotateLocal( (1.0 - angle) * dt, Vector3{ 0, 0, 1 });	//Miri endanvat, que miri amb la cua

	Vector3 posAvioEnElMon = player->getGlobalMatrix() * player->mesh->center; //Les coordenades de l'avió en el mon by Aurel.

	if ( posAvioEnElMon.distance( *victim ) < 1000 ) // Sí l'avió està a menys de 1000, el perseguim.

		perseguir = true;

	else if(posAvioEnElMon.distance(target_position) > 1000 || posAvioEnElMon.distance(*victim) > 300)

		perseguir = false;

	if (!perseguir) //Mentre no el perseguim
	{
		if (target_position.distance(posAvioEnElMon) < 1000) //Sí està a una distància menor de 1000 del waypoint, canviem de waypoint.
		{
			i++;

			if (i == WayPoints.size())	//Per no sortir-nos del vector.

				i = 0;
		}
	}
	//Sí realemnt estem sequint l'enemic el disparem, COSA QUE S'HA DE MILLORAR MOLT I MIRAR BÉ COM ÉS FA EL CAMICAZE.
	else if (posAvioEnElMon.distance(*victim) < 200) player->shoot();
}

Fighter * IA::getVictim() {
	return player;
}