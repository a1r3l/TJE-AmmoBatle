#include "World.h"
#include "game.h"
#include "Estados\IntroState.h"

//#include "glutbitmap.h"

World * World::instance = NULL;

World::World(){
	
	sceneRoot = new Entity();
	skybox = new EntityMesh();

	//SDL_Joystick * pad = NULL;
	keystate = NULL;
	mouse_locked = false;
	elapsedspeed = 1;
	player3 = NULL;
	player2 = NULL;
	ia = NULL;
	ia0 = NULL;

	bulletmanager = BulletManager::getInstance();
	collisionManager = CollisionManager::getInstance();
	controller = Controller::getInstance();
	soundManager = SoundManager::getInstance();

	printMessage = 0;
	turretMessage = 0;
	ammoMessage = 0;
	enemyMessage = 0;
	ammoDistroyed = 0;
}

bool World::init() {

	Game::instance->pad = openJoystick(0);

	if (soundManager->loadSample("shot.wav")) std::cout << "Cargo SHOT" << std::endl;
	if (soundManager->loadSample("explosion.wav")) std::cout << "Cargo EXPLOSION" << std::endl;
	if (soundManager->loadStream("spitfire.wav")) std::cout << "Cargo SPITFIRE" << std::endl;
	if (soundManager->loadStream("ingame.wav")) std::cout << "Cargo INGAME" << std::endl;
	if (soundManager->loadStream("intro.mp3")) std::cout << "Cargo INGAME" << std::endl;
	
	soundManager->streamVol("ingame.wav", 0.2);
	soundManager->soundVol("shot.wav", 0.01);

	//We set The freeCamera
	free_camera = createCamera(Vector3(0, 0, 25), Vector3(0, 0, 0), Vector3(0, 1, 0));
	current_cam = free_camera;

	loadScene();

	//Creo al player1
	player = (Fighter*)createEntity("spitfire");
	//player->speed = 0;
	player->localMatrix.traslate(0, 1000, 0);
	sceneRoot->addChild(player);
	addDynamicEntity(player);

	//Creo al player2
	player2 = (Fighter*)createEntity("wildcat");
	player2->entityName = "WildCat1";
	player2->localMatrix.traslate(2848, 608, 1888);
	sceneRoot->addChild(player2);
	addDynamicEntity(player2);

	//Creo al player3
	player3 = (Fighter*)createEntity("wildcat");
	player3->entityName = "WildCat2";
	player3->localMatrix.traslate(4255, 653, -1000);
	sceneRoot->addChild(player3);
	addDynamicEntity(player3);

	controller->setController(player);
	current_cam = controller->player_camera;

	std::vector<Vector3> illa1, illa2;

	illa1.push_back(Vector3{ 4255, 635, -1044 });
	illa1.push_back(Vector3{ 2848, 608, 1888 });
	illa1.push_back(Vector3{ 50, 491, -2629 });
	illa1.push_back(Vector3{ -5357,1089,2699 });

	illa2.push_back(Vector3{ -5357,1089,2699 });
	illa1.push_back(Vector3{ 50, 491, -2629 });
	illa2.push_back(Vector3{ 2848, 608, 1888 });
	illa2.push_back(Vector3{ 4255, 635, -1044 });

	ia = new IA(player3, &current_cam->eye, illa1);//player->getGlobalMatrix() * player->mesh->center,
	ia0 = new IA(player2, &current_cam->eye, illa2);//player->getGlobalMatrix() * player->mesh->center,

	artificialIntelligence.push_back(ia);
	artificialIntelligence.push_back(ia0);

	SDL_ShowCursor(!mouse_locked); //hide or show the mouse
	return true;
}

void World::addSceneChild(Entity * child) {
	sceneRoot->addChild(child);
}

void World::render(Camera * camera) {
	skybox->localMatrix.setTranslation(camera->eye.x, camera->eye.y, camera->eye.z);
	glDisable(GL_DEPTH_TEST);
		skybox->render(camera);
	glEnable(GL_DEPTH_TEST);
	sceneRoot->render(camera);
	bulletmanager->render(camera);
	renderHUD();
}

bool World::setSkybox(const char* name, const char* texture) {
	skybox->setup(name, texture);
	skybox->clip_test = false;
	return true;
}

Camera* World::createCamera(Vector3 eye, Vector3 center, Vector3 up) {
	float window_width = Game::instance->window_width;
	float window_height = Game::instance->window_height;

	Camera * camera = new Camera();
	camera->lookAt(eye,center,up); //position the camera and point to 0,0,0
	camera->setPerspective(70, window_width / (float)window_height, 0.1, 100000); //set the projection, we want to be perspective
	return camera;
}

void World::update(float seconds_elapsed) {
	
	if (ammoDistroyed == 2) {
		Game::instance->GAMESTATE = 7;
		finalTime = getTime();
	}

	//Contador para printar mensaje por pantalla
	printMessage -= seconds_elapsed;
	turretMessage -= seconds_elapsed;
	ammoMessage -= seconds_elapsed;
	enemyMessage -= seconds_elapsed;

	bulletmanager->update(seconds_elapsed);
	sceneRoot->update(seconds_elapsed * elapsedspeed);
	//ia->update(seconds_elapsed);
	double speed = seconds_elapsed * 100; //the speed is defined by the seconds_elapsed so it goes constant

	if (current_cam == free_camera)
	{
		//mouse input to rotate the cam
		if ((mouse_state & SDL_BUTTON_LEFT) || mouse_locked) //is left button pressed?
		{
			free_camera->rotate(mouse_delta.x * 0.005, Vector3(0, -1, 0));
			free_camera->rotate(mouse_delta.y * 0.005, free_camera->getLocalVector(Vector3(-1, 0, 0)));
		}
		//async input to move the camera around
		if (keystate[SDL_SCANCODE_LSHIFT]) speed *= 10; //move faster with left shift
		if (keystate[SDL_SCANCODE_W] || keystate[SDL_SCANCODE_UP])    current_cam->move(Vector3(0, 0, 1) * speed);
		if (keystate[SDL_SCANCODE_S] || keystate[SDL_SCANCODE_DOWN])  current_cam->move(Vector3(0, 0, -1) * speed);
		if (keystate[SDL_SCANCODE_A] || keystate[SDL_SCANCODE_LEFT])  current_cam->move(Vector3(1, 0, 0) * speed);
		if (keystate[SDL_SCANCODE_D] || keystate[SDL_SCANCODE_RIGHT]) current_cam->move(Vector3(-1, 0, 0) * speed);

	}
	else if (current_cam == controller->player_camera) {
		controller->update(seconds_elapsed);
	}

	for (int i = 0; i < artificialIntelligence.size(); i++) artificialIntelligence[i]->update(seconds_elapsed);

#ifdef _DEBUG
	std::cout << current_cam->eye.x << " " << current_cam->eye.y << " " << current_cam->eye.z << std::endl;
#endif // DEBUG

	//to navigate with the mouse fixed in the middle
	if (mouse_locked)
	{
		int center_x = floor(Game::instance->window_width*0.5);
		int center_y = floor(Game::instance->window_height*0.5);
		//center_x = center_y = 50;
		SDL_WarpMouseInWindow(Game::instance->window, center_x, center_y); //put the mouse back in the middle of the screen
																 //SDL_WarpMouseGlobal(center_x, center_y); //put the mouse back in the middle of the screen

		this->mouse_position.x = center_x;
		this->mouse_position.y = center_y;
	}

	//angle += seconds_elapsed * 10;
	//Buscamos collisiones
	collisionManager->CollisionDetection();
	destroyDeleteables();
}



void* World::createEntity(const char * name) {
	
	if (name == "spitfire") {
		Fighter* player = new Fighter();
		player->entityName = "spitfire";
		player->setup("spitfire.ASE", "spitfire_color_spec.tga", "spitfire_low.ASE");
		return player;
	}
	
	else if (name == "wildcat") {
		
		Fighter* player1 = new Fighter();
		player1->entityName = "wildcat";
		player1->setup("wildcat.ASE", "wildcat.tga", "wildcat_low.ASE");
		return player1;
	}
	
	else if (name == "turret") {
		Turret * meshEntity = new Turret();
		meshEntity->setup("turret_base.ASE", "turret.TGA",NULL);
		return meshEntity;
	}

	else if (name == "island") {
		EntityCollider * meshEntity = new EntityCollider();
		meshEntity->setup("island.ASE", "island_color_luz.TGA");
		return meshEntity;
	}
	
	else if (name == "water") {
		EntityCollider * meshEntity = new EntityCollider();
		meshEntity->setup("agua.ASE", "agua.TGA");
		return meshEntity;
	}
	
	else if (name == "terrain_air") {
		EntityCollider * meshEntity = new EntityCollider();
		meshEntity->setup("terrain_airport.ASE", "terrain_airport.TGA");
		return meshEntity;
	}

	else if (name == "ammonition") {
		Ammonition * meshEntity = new Ammonition();
		meshEntity->setup("box.ASE", "ammo.TGA",NULL);
		return meshEntity;
	}

	else if (name == "quad") {
		EntityCollider * meshEntity = new EntityCollider();
		meshEntity->entityName = "quad";
		meshEntity->setup("box.ASE");
		return meshEntity;
	}
}
void World::addDynamicEntity(EntityCollider * dinamic) {
	collisionManager->dynamic_entities.push_back(dinamic);
}
void World::addStaticEntity(EntityCollider * stEntity){
	collisionManager->static_entities.push_back(stEntity);
}
void World::addAmmoEntity(EntityCollider * ammo) {
	collisionManager->ammo_boxes.push_back(ammo);
}
void World::addTurretEntity(EntityCollider * turret) {
	collisionManager->turrets.push_back(turret);
}

void World::loadScene() {
	//Seteamos la SkyBox
	setSkybox("sky.ASE", "sky.TGA");
	//Cargamos los Mapas
	EntityCollider * meshEntity = (EntityCollider*)createEntity("island");
	meshEntity->entityName = "ISLA";
	sceneRoot->addChild(meshEntity);
	addStaticEntity(meshEntity);
	
	for (int i = -3; i < 3; i++) {
		for (int j = -3; j < 3; j++) {
			EntityCollider * waterEntity = (EntityCollider*)createEntity("water");
			waterEntity->entityName = "WATER";
			waterEntity->localMatrix.traslate(i * (2 * waterEntity->mesh->halfsize.x), -10, j * (2 * waterEntity->mesh->halfsize.z));
			sceneRoot->addChild(waterEntity);
			addStaticEntity(waterEntity);
		}
	}

	EntityCollider * muni1 = (EntityCollider*)createEntity("ammonition");
	muni1->entityName = "Ammonition1";
	muni1->two_sided = true;
	muni1->localMatrix.traslate(1514,40,-750);
	sceneRoot->addChild(muni1);
	addAmmoEntity(muni1);
		
	//TORRETA1
		EntityCollider * torret = (EntityCollider*)createEntity("turret");
		torret->entityName = "Turret11";
		torret->localMatrix.scale(30, 30, 30);
		torret->localMatrix.traslate(1286, 0, -824);
		sceneRoot->addChild(torret);
		addTurretEntity(torret);
		
		//TORRETA2
		EntityCollider * torret1 = (EntityCollider*)createEntity("turret");
		torret1->entityName = "Turret12";
		torret1->localMatrix.scale(30, 30, 30);
		torret1->localMatrix.traslate(1770, 38, -662);
		sceneRoot->addChild(torret1);
		addTurretEntity(torret1);

	
	EntityCollider * muni2 = (EntityCollider*)createEntity("ammonition");
	muni2->entityName = "Ammonition2";
	muni2->two_sided = true;
	muni2->localMatrix.traslate(-3647, 50, 3209);
	sceneRoot->addChild(muni2);
	addAmmoEntity(muni2);
	
	//TORRETA1
	EntityCollider * torret2 = (EntityCollider*)createEntity("turret");
	torret2->entityName = "Turret21";
	torret2->localMatrix.scale(30, 30, 30);
	torret2->localMatrix.traslate(-3766, 0, 3541);
	sceneRoot->addChild(torret2);
	addTurretEntity(torret2);

	//TORRETA2
	EntityCollider * torret21 = (EntityCollider*)createEntity("turret");
	torret21->entityName = "Turret22";
	torret21->localMatrix.scale(30, 30, 30);
	torret21->localMatrix.rotateLocal(180 * DEG2RAD, Vector3(0, 1, 0));
	torret21->localMatrix.traslate(-3625, -10, 2901);
	sceneRoot->addChild(torret21);
	addTurretEntity(torret21);
}

void World::destroyDeleteables() {

	if (sceneRoot->to_destroy.size() != 0) {
		std::vector <IA*> artInt = Game::instance->artificialIntelligence;
 		for (int i = 0; i < sceneRoot->to_destroy.size(); ++i) {
			
			for (int j = 0; j < artInt.size(); j++) {
				if (sceneRoot->to_destroy[i] == artInt[j]->getVictim()) {
					delete artInt[j];
					Game::instance->artificialIntelligence.erase(Game::instance->artificialIntelligence.begin() + j);
				}
			}
			
			delete sceneRoot->to_destroy[i];
			sceneRoot->to_destroy.erase(sceneRoot->to_destroy.begin() + i);
		}
	}
}

void World::printString(float x, float y, float z, void* font, float r, float g, float b, char * string) {
	glColor3f(r, g, b);
	glRasterPos3f(x, y, z);
	int len, i;
	len = (int)strlen(string);
	for (i = 0; i < len; i++)
		glutBitmapCharacter(font, string[i]);
}

void World::renderHUD() {

	float windowHeigth = Game::instance->window_height;
	float windowWidth = Game::instance->window_width;
	
	//PREPROCESED
	Mesh prep;
	prep.createQuad(windowWidth*0.5, windowHeigth*0.5, windowWidth, windowHeigth, true);
	
	//HUD DE VIDA
	float maxLive = 100;
	float live = player->LIVE;
	float MaxLiveWidth = windowWidth/4;
	float LiveWidth = (MaxLiveWidth * (live / maxLive));
	float liveCenterX = MaxLiveWidth/2 + 10;
	float liveCenterY = 50;
	
	Mesh FondoLiveQuad;
	FondoLiveQuad.createQuad(liveCenterX, liveCenterY, MaxLiveWidth + 5, 40 );
	Mesh FondoInLiveQuad;
	FondoInLiveQuad.createQuad(liveCenterX, liveCenterY, MaxLiveWidth, 35);
	Mesh LiveQuad;
	LiveQuad.createQuad(liveCenterX - ((maxLive - live) / 2)*(MaxLiveWidth / maxLive), liveCenterY, LiveWidth, 35);

	//HUD PARA EL CROSHAIR
	Mesh quad;
	quad.createQuad(windowWidth * 0.5, windowHeigth * 0.5, 50, 50);
	
	
	
	//HUD PARA LAS CAJAS
	Mesh fondocajas;
	fondocajas.createQuad(windowWidth - 100, 50 , 100,50);
	Mesh caja1;
	caja1.createQuad(windowWidth - 125 , 50, 40, 40);
	Mesh caja2;
	caja2.createQuad(windowWidth-75,50, 40, 40);

	//HUD MINIMAPA
	//Mesh MiniMapa;
	//MiniMapa.createQuad(100,Game::instance->window_height-100,200,200);
	
	//CARGA DE TEXTURAS
	Texture * texture = TextureManager::getInstance()->getTexture("cross.tga");
	Texture * objTexture = TextureManager::getInstance()->getTexture("corchetes.tga");
	Texture * prepText = TextureManager::getInstance()->getTexture("prep.tga");
	//Texture * islandtext = TextureManager::getInstance()->getTexture("island_color_luz.tga");

	Matrix44 player_global = player->getGlobalMatrix();
	Vector3 center = player_global * player->mesh->center;

	
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	
	//PINTO OBJETIVOS
	for (int i = 0; i < collisionManager->dynamic_entities.size();i++) {
		if (player != collisionManager->dynamic_entities[i]) {

			Matrix44 globalParent = collisionManager->dynamic_entities[i]->getGlobalMatrix();
			Vector3 pos = globalParent * collisionManager->dynamic_entities[i]->mesh->center;
				
			std::string info = "[  ]" ;
			char *cstr = new char[info.length()];
			strcpy(cstr, info.c_str());
			printString(pos.x, pos.y, pos.z, GLUT_BITMAP_TIMES_ROMAN_10, 0, 1, 0, cstr);
			delete[] cstr;
		}
	}
	
	for (int i = 0; i < collisionManager->ammo_boxes.size(); i++) {

			Matrix44 globalParent = collisionManager->ammo_boxes[i]->getGlobalMatrix();
			Vector3 pos = globalParent * collisionManager->ammo_boxes[i]->mesh->center;

			std::string info = "[  ]";
			char *cstr = new char[info.length()];
			strcpy(cstr, info.c_str());
			printString(pos.x, pos.y, pos.z, GLUT_BITMAP_TIMES_ROMAN_10, 1, 0, 0, cstr);
			delete[] cstr;
	}
	
	for (int i = 0; i < collisionManager->turrets.size(); i++) {

			Matrix44 globalParent = collisionManager->turrets[i]->getGlobalMatrix();
			Vector3 pos = globalParent * collisionManager->turrets[i]->mesh->center;

			std::string info = "[  ]";
			char *cstr = new char[info.length()];
			strcpy(cstr, info.c_str());
			printString(pos.x, pos.y, pos.z, GLUT_BITMAP_TIMES_ROMAN_10, 1, 0, 0, cstr);
			delete[] cstr;
	}

	//Camera 2D
	Camera cam2D;
	cam2D.setOrthographic(0, windowWidth, windowHeigth, 0, -1, 1);
	cam2D.set();

	if (printMessage > 0) {

		float posWidth = windowWidth *0.5 - 230;
		float posHeigth = windowHeigth * 0.25;

		std::string info = "[KILL TURRETS FIRST!]";
		char *cstr = new char[info.length()];
		strcpy(cstr, info.c_str());
		printString(posWidth, posHeigth, 0, GLUT_BITMAP_TIMES_ROMAN_24, 1, 0, 0, cstr);
		delete[] cstr;
	}
	
	if (turretMessage > 0) {

		float posWidth = windowWidth *0.5 - 130;
		float posHeigth = windowHeigth * 0.25;

		std::string info = "[TURRET DESTROYED!]";
		char *cstr = new char[info.length()];
		strcpy(cstr, info.c_str());
		printString(posWidth, posHeigth, 0, GLUT_BITMAP_TIMES_ROMAN_24, 1, 0, 0, cstr);
		delete[] cstr;
	}
	
	if (ammoMessage > 0) {

		float posWidth = windowWidth *0.5 - 130;
		float posHeigth = windowHeigth * 0.25;

		std::string info = "[AMMO DESTROYED!]";
		char *cstr = new char[info.length()];
		strcpy(cstr, info.c_str());
		printString(posWidth, posHeigth, 0, GLUT_BITMAP_TIMES_ROMAN_24, 1, 0, 0, cstr);
		delete[] cstr;
	}
	
	if (enemyMessage > 0) {

		float posWidth = windowWidth *0.5 - 130;
		float posHeigth = windowHeigth * 0.25;

		std::string info = "[ENEMIGO DESTRUIDO!]";
		char *cstr = new char[info.length()];
		strcpy(cstr, info.c_str());
		printString(posWidth, posHeigth, 0, GLUT_BITMAP_TIMES_ROMAN_24, 1, 0, 0, cstr);
		delete[] cstr;
	}
	
	/*Mesh points;
	Mesh * island = MeshManager::getInstance()->getMesh("island.ASE");

	for (int i = 0; i < collisionManager->dynamic_entities.size(); i++) {
		EntityCollider * enti = collisionManager->dynamic_entities[i];
		Matrix44 global = enti->getGlobalMatrix();
		Vector3 pos = global * enti->mesh->center;
		pos = (pos / island->halfsize) * 200;
		pos.y = pos.z;
		pos.z = 0;

		pos.x += 100;
		pos.y += Game::instance->window_height - 100;

		points.vertices.push_back(pos);
	}*/

	//PINTADO DE COSAS CON BLENDING
	glEnable(GL_BLEND);
	
		//PINTADO DE PREPROCESED
		prepText->bind();
			glColor3f(0, 0, 0);
			prep.render(GL_TRIANGLES);
			glColor3f(1, 1, 1);
		prepText->unbind();

		//PINTADO DE CROSSHAIR
		texture->bind();
			glColor3f(0, 0, 0);
				quad.render(GL_TRIANGLES);
			glColor3f(1, 1, 1);
		texture->unbind();
	
		//CAJAS

		std::string info = "PLAYER LIVE";
		char *cstr = new char[info.length()];
		strcpy(cstr, info.c_str());
		printString(20, 22, 0, GLUT_BITMAP_TIMES_ROMAN_24, 0, 1, 0, cstr);
		delete[] cstr;

		std::string info1 = "OBJECTIVE";
		char *cstr1 = new char[info1.length()];
		strcpy(cstr1, info1.c_str());
		printString(windowWidth - 150, 22, 0, GLUT_BITMAP_TIMES_ROMAN_24, 0, 1, 0, cstr1);
		delete[] cstr1;

		glColor3f(0, 0, 0);
			fondocajas.render(GL_TRIANGLES);
		glColor3f(0.41, 0.41, 0.41);
		if (World::getInstance()->ammoDistroyed == 1) {
			Texture * tex = TextureManager::getInstance()->getTexture("ammo.tga");
			tex->bind(),
				caja1.render(GL_TRIANGLES);
			tex->unbind();
		}
		else caja1.render(GL_TRIANGLES);
			caja2.render(GL_TRIANGLES);
		glColor3f(1, 1, 1);
		
		//MINIMAPA
		/*islandtext->bind();
			MiniMapa.render(GL_TRIANGLES);
			glColor3f(1, 0, 0);
			points.render(GL_POINTS);
			glColor3f(1, 1, 1);
		islandtext->unbind();*/
	
	glDisable(GL_BLEND);

//PINTADO DE LA VIDA
	glColor3f(0, 0, 0);
	FondoLiveQuad.render(GL_TRIANGLES);
	glColor3f(0.41,0.41,0.41);
	FondoInLiveQuad.render(GL_TRIANGLES);
	glColor3f(0, 0.39, 0);
	LiveQuad.render(GL_TRIANGLES);
	glColor3f(1, 1, 1);
}

void World::renderVictory() {

	float windowHeigth = Game::instance->window_height;
	float windowWidth = Game::instance->window_width;

	Camera cam2D;
	cam2D.setOrthographic(0, windowWidth, windowHeigth, 0, -1, 1);
	cam2D.set();

	Mesh victoryImage;
	victoryImage.createQuad(windowWidth*0.5, windowHeigth*0.5, windowWidth, windowHeigth, true);
	
	float time = (finalTime - initTime)/1000;

	std::string typeVict;

		if(time < 60) typeVict = "victoryA.tga";
			else if(time < 120) typeVict = "victoryB.tga";
				else if(time > 120)typeVict = "victoryC.tga";

	Texture * victoryTextureA = TextureManager::getInstance()->getTexture(typeVict.c_str());

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	victoryTextureA->bind();
		victoryImage.render(GL_TRIANGLES);
	victoryTextureA->unbind();

	SDL_GL_SwapWindow(Game::instance->window);
}

void World::genericRender(std::string render) {

	float windowHeigth = Game::instance->window_height;
	float windowWidth = Game::instance->window_width;

	Camera cam2D;
	cam2D.setOrthographic(0, windowWidth, windowHeigth, 0, -1, 1);
	cam2D.set();

	Mesh genericImage;
	genericImage.createQuad(windowWidth*0.5, windowHeigth*0.5, windowWidth, windowHeigth, true);

	Texture * genericTexture = TextureManager::getInstance()->getTexture(render.c_str());

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	genericTexture->bind();
	genericImage.render(GL_TRIANGLES);
	genericTexture->unbind();

	SDL_GL_SwapWindow(Game::instance->window);
}