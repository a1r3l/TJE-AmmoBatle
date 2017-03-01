Aurel Ioan Patrutiu  158689
Isaac Rossell Ripoll 163500

COMO JUGAR:

Con el mando de la XBOX 360, controlamos un Spitfire.
Nuestra misión es destruir 2 cajas situadas en la isla que viene a ser el mapa de nuestro juego.
Para poder recoger las cajas hemos de matar las dos torretas que la protegen,
después dispararlas hasta que exploten y asi destruirlas.
Dos Wildcats protegen la zona aérea, patrullan a una altura superior a la nuestra,
si nos localizan nos intentaran matar.

Ganar:
Para poder llegar al estado de GANAR la partida hemos de recorrer el mapa buscando las cajas de municion
enemigas situadas en el HUD con unos corchetes rojos. Una vez destruidas als torretas que las protegen
deberemos destruir als dos cajas de municion. Una vez destruidas las dos habremos ganado la partida.

Perder:
Para perder el juego hemos de realizar diferentes acciones que nos mataran. Por un lado si colisionamos
contra cualquier objeto de la escena (agua, isla, torretas, cajas de municion). Por el otro tambien moriremos cuando 
los aviones enemigos nos disparen y hagan que nuestra vida vaya hasta cero. 

Reinicio de Juego:
Al ser un juego sabemos que este debe poder reiniciarse sin tener que cerrar la aplicación. Por problemas en el desarrollo
finalmente esta caracteristica no la hemos podido satisfacer. Cada vez que se gana la partida o se pierde la aplicació te
redirige a un estado de VICTORIA (con tres posibles recompensas (MEDALLA ORO,PLATA,BRONCE)) o DERROTA. Si en cada uno de
estos estados apretamos el boton A del mando en vez de redirigirnos al menus como deberia ser, no cierra la aplicación.
Por lo tanto para poder volver a jugar hemos de reiniciar el juego. 

DESCRIPCIÓN DEL PROYECTO:

Game:

Esta el la classe desde donde gestionaremos el juego. Este sera en el cargado de llevar los estados de juego desde la 
introduccion hasta el gameover final. Dependiendo de en que estado estemos Game se encargara de renderizar la pantalla correspondiente
y de hacer update de los elementos necesarios. 

World:

Es la classe donde cargamos todos los recursos graficos de nuestro juego. Este es el encargado de llamar a todas las funciones
de carga de aviones, islas y todos los demas recursos con sus correspondientes posiciones en el mundo y lo diferentes vectores 
con los cuales trabajaremos durante la ejecución.

Coldet:

Hemos utilizador la biblioteca coldet para ralizar las colisiones,definiendo cuales de nuestros objetoso colisionan con quales.
Para usar coldet hemos creado una classe EntityColider, que viene a representar todas las entidades que pueden colisionar. A parte de sto
tambien tenemos un manager de colisiones que explicaremos mas abajo en el apartado Managers. 

Bullets:
Es la classe que viene a representar las balas en nuestro juego. Para poder gestionar estas balas tenemos un BulletManager que 
explicaremos en el apartado de Managers. 

Controler:
Nos permite controlar el jugado, con un mapeo del mando de la XBOX 360,
nos permite disparar RT, Joystick analógico izquierdo haces el Pitch y el Roll, 
con el derecho o los botones RB y LB haces el Yaw.

IA:
Para poder darle vida inteligente a los aviones enemigos, hemos usado una ia
muy simple, la qual tiene dos faces, partulla y atacar.
Si el avión no nos ve, sigue un conjunto de WayPoints predeterminados,
si nos ponemos dentro de su rango de visión empezarà a perseguirnos y 
en cuanto de encuentre a un distáncia inferior empezarà a dispararnos.

MANAGERS:

TextureManager:
Es el encargado de cargar las texturas, en una hashtable desde donde nos servira de texturas sin necesidad
de volver a cargarlas nueva mente. Cada vez que le pidamos una textura nos devolvera un puntero a esta. 

MeshManager:
Es el encargado de cargar las meshes, en una hashtable desde donde nos servira de meshes sin necesidad
de volver a cargarlas nuevamente. Cada vez que le pidamos una mesh nos devolvera un puntero a esta. 

SoundManager:
Es el encargado de cargar los sonidos, ejecutarlos y paralos a peticion de la aplicacion.  Tiene dos hashtables,
una para sonidos y una para streams. Dentro vamso guardando los sonidos que cargamos y luego los buscamos y ejcutamos 
a peticion de la aplicacion. 

Colision Manager:
Es el manager que se encarga de las collisiones dentro del juego. Gracias a ala herramienta COLDET hacemos que la isla
 y el mar tengan una collision model. Los demas elementos del juego no disponen de una model porque realizamos las 
colisiones calculando las esferas de cada objeto. Como con las isalas es imposible hacerle una esfera, le creamos la
modelCollision con COLDET y comprobaremos collisiones con esta.
Este manager tiene la funcion de detectar collisiones, saber que hacer cuando haya una collision de bala contra 
cualquier objeto de la escena y saber que hacer cuando haya una colision entre objetos de la escena. 

Bullet Manager:
Este es el manager que se encarga de aprovisionar de balas a los aviones y torretas (no se ha llegado a implementar).
Tiene una piscina de balas , las cuales las va gestionando y asignando cada una cuando las entidades se las pide.  

LINK A YOUTUBE CON EL JUEGO FUNCIONANDO!

https://www.youtube.com/watch?v=IfypJbomH1s



