



//Ejemplo de llamada//

//Coges la posición de la transformada
unsigned int x, y, z;
Vector3 pos = world->main_world->children[i]->getGlobalPosition();

x = pos.x;
y = pos.y;
z = pos.z;


glDisable(GL_DEPTH_TEST);

//Printeas Vida del enemigo
std::string hp_info = "HP: " + std::to_string((int)world->main_world->children[i]->getHP());
char *cstr = new char[hp_info.length() + 1];
strcpy(cstr, hp_info.c_str());
printString(x + 5, y + 5, z + 5, GLUT_BITMAP_TIMES_ROMAN_10, 0, 1, 0, cstr);//Llamada a la función
delete [] cstr;
