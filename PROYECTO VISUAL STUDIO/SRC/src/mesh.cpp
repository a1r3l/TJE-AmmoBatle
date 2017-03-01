#include "mesh.h"
#include "texture.h"
#include <cassert>
#include "includes.h"
#include "shader.h"
#include "extra/textparser.h"
#include "utils.h"
#include "limits"
#include "Bullet.h"

Mesh::Mesh()
{
	vertices_vbo_id = 0;
	normals_vbo_id = 0;
	uvs_vbo_id = 0;
	colors_vbo_id = 0;
}

Mesh::Mesh( const Mesh& m )
{
	vertices = m.vertices;
	normals = m.normals;
	uvs = m.uvs;
	colors = m.colors;
}

Mesh::~Mesh()
{
	if(vertices_vbo_id) glDeleteBuffersARB(1, &vertices_vbo_id);
	if(normals_vbo_id) glDeleteBuffersARB(1, &normals_vbo_id);
	if(uvs_vbo_id) glDeleteBuffersARB(1, &uvs_vbo_id);
	if(colors_vbo_id) glDeleteBuffersARB(1, &colors_vbo_id);
}

void Mesh::clear()
{
	vertices.clear();
	normals.clear();
	uvs.clear();
	colors.clear();
}


void Mesh::render(int primitive)
{
	assert(vertices.size() && "No vertices in this mesh");

    glEnableClientState(GL_VERTEX_ARRAY);

	if(vertices_vbo_id)
	{
		glBindBufferARB( GL_ARRAY_BUFFER_ARB, vertices_vbo_id );
		glVertexPointer(3, GL_FLOAT, 0, NULL );
	}
	else
		glVertexPointer(3, GL_FLOAT, 0, &vertices[0] );

	if (normals.size())
	{
		glEnableClientState(GL_NORMAL_ARRAY);
		if(normals_vbo_id)
		{
			glBindBufferARB( GL_ARRAY_BUFFER_ARB, normals_vbo_id );
			glNormalPointer(GL_FLOAT, 0, NULL );
		}
		else
			glNormalPointer(GL_FLOAT, 0, &normals[0] );
	}

	if (uvs.size())
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		if(uvs_vbo_id)
		{
			glBindBufferARB( GL_ARRAY_BUFFER_ARB, uvs_vbo_id );
			glTexCoordPointer(2, GL_FLOAT, 0, NULL );
		}
		else
			glTexCoordPointer(2, GL_FLOAT, 0, &uvs[0] );
	}

	if (colors.size())
	{
		glEnableClientState(GL_COLOR_ARRAY);
		if(colors_vbo_id)
		{
			glBindBufferARB( GL_ARRAY_BUFFER_ARB, colors_vbo_id );
			glColorPointer(4, GL_FLOAT, 0, NULL );
		}
		else
			glColorPointer(4, GL_FLOAT, 0, &colors[0] );
	}
    
	glDrawArrays(primitive, 0, (GLsizei)vertices.size() );
	glDisableClientState(GL_VERTEX_ARRAY);

	if (normals.size())
		glDisableClientState(GL_NORMAL_ARRAY);
	if (uvs.size())
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	if (colors.size())
		glDisableClientState(GL_COLOR_ARRAY);
	glBindBufferARB( GL_ARRAY_BUFFER_ARB, 0 );
}

void Mesh::render( int primitive, Shader* sh )
{
	if(!sh || !sh->compiled)
        return render(primitive);
    
	assert(vertices.size() && "No vertices in this mesh");

    int vertex_location = sh->getAttribLocation("a_vertex");
	assert(vertex_location != -1 && "No a_vertex found in shader");

	if(vertex_location == -1)
		return;

    glEnableVertexAttribArray(vertex_location);
	if(vertices_vbo_id)
	{
		glBindBufferARB( GL_ARRAY_BUFFER_ARB, vertices_vbo_id );
		glVertexAttribPointer(vertex_location, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	}
	else
		glVertexAttribPointer(vertex_location, 3, GL_FLOAT, GL_FALSE, 0, &vertices[0] );

	int normal_location = -1;
    if (normals.size())
    {
        normal_location = sh->getAttribLocation("a_normal");
        if(normal_location != -1)
        {
            glEnableVertexAttribArray(normal_location);
			if(normals_vbo_id)
			{
				glBindBufferARB( GL_ARRAY_BUFFER_ARB, normals_vbo_id );
	            glVertexAttribPointer(normal_location, 3, GL_FLOAT, GL_FALSE, 0, NULL );
			}
			else
	            glVertexAttribPointer(normal_location, 3, GL_FLOAT, GL_FALSE, 0, &normals[0] );
        }
    }
    
	int uv_location = -1;
	if (uvs.size())
	{
        uv_location = sh->getAttribLocation("a_uv");
        if(uv_location != -1)
        {
            glEnableVertexAttribArray(uv_location);
			if(uvs_vbo_id)
			{
				glBindBufferARB( GL_ARRAY_BUFFER_ARB, uvs_vbo_id );
	            glVertexAttribPointer(uv_location, 2, GL_FLOAT, GL_FALSE, 0, NULL );
			}
			else
	            glVertexAttribPointer(uv_location, 2, GL_FLOAT, GL_FALSE, 0, &uvs[0] );
        }
    }
    
	int color_location = -1;
	if (colors.size())
	{
        color_location = sh->getAttribLocation("a_color");
        if(color_location != -1)
        {
            glEnableVertexAttribArray(color_location);
			if(colors_vbo_id)
			{
				glBindBufferARB( GL_ARRAY_BUFFER_ARB, colors_vbo_id );
	            glVertexAttribPointer(color_location, 4, GL_FLOAT, GL_FALSE, 0, NULL );
			}
			else
	            glVertexAttribPointer(color_location, 4, GL_FLOAT, GL_FALSE, 0, &colors[0] );
        }
    }
    
	glDrawArrays(primitive, 0, (GLsizei)vertices.size() );

	glDisableVertexAttribArray( vertex_location );
	if(normal_location != -1) glDisableVertexAttribArray( normal_location );
	if(uv_location != -1) glDisableVertexAttribArray( uv_location );
	if(color_location != -1) glDisableVertexAttribArray( color_location );
	glBindBufferARB( GL_ARRAY_BUFFER_ARB, 0 );
}

void Mesh::uploadToVRAM()
{
	//delete old
	if(vertices_vbo_id) glDeleteBuffersARB(1, &vertices_vbo_id);
	if(normals_vbo_id) glDeleteBuffersARB(1, &normals_vbo_id);
	if(uvs_vbo_id) glDeleteBuffersARB(1, &uvs_vbo_id);
	if(colors_vbo_id) glDeleteBuffersARB(1, &colors_vbo_id);

	glGenBuffersARB( 1, &vertices_vbo_id ); //generate one handler (id)
	glBindBufferARB( GL_ARRAY_BUFFER_ARB, vertices_vbo_id ); //bind the handler
	glBufferDataARB( GL_ARRAY_BUFFER_ARB, vertices.size() * 3 * sizeof(float), &vertices[0], GL_STATIC_DRAW_ARB ); //upload data

	if(normals.size())
	{
		glGenBuffersARB( 1, &normals_vbo_id); //generate one handler (id)
		glBindBufferARB( GL_ARRAY_BUFFER_ARB, normals_vbo_id ); //bind the handler
		glBufferDataARB( GL_ARRAY_BUFFER_ARB, normals.size() * 3 * sizeof(float), &normals[0], GL_STATIC_DRAW_ARB ); //upload data
	}

	if(uvs.size())
	{
		glGenBuffersARB( 1, &uvs_vbo_id); //generate one handler (id)
		glBindBufferARB( GL_ARRAY_BUFFER_ARB, uvs_vbo_id ); //bind the handler
		glBufferDataARB( GL_ARRAY_BUFFER_ARB, uvs.size() * 2 * sizeof(float), &uvs[0], GL_STATIC_DRAW_ARB ); //upload data
	}

	if(colors.size())
	{
		glGenBuffersARB( 1, &colors_vbo_id); //generate one handler (id)
		glBindBufferARB( GL_ARRAY_BUFFER_ARB, colors_vbo_id ); //bind the handler
		glBufferDataARB( GL_ARRAY_BUFFER_ARB, uvs.size() * 4 * sizeof(float), &colors[0], GL_STATIC_DRAW_ARB ); //upload data
	}

}

void Mesh::createQuad(float center_x, float center_y, float w, float h, bool flip_uvs )
{
	vertices.clear();
	normals.clear();
	uvs.clear();
	colors.clear();

	//create six vertices (3 for upperleft triangle and 3 for lowerright)

	vertices.push_back( Vector3( center_x + w*0.5f, center_y + h*0.5f, 0.0f ) );
	vertices.push_back( Vector3( center_x - w*0.5f, center_y - h*0.5f, 0.0f ) );
	vertices.push_back( Vector3( center_x + w*0.5f, center_y - h*0.5f, 0.0f ) );
	vertices.push_back( Vector3( center_x - w*0.5f, center_y + h*0.5f, 0.0f ) );
	vertices.push_back( Vector3( center_x - w*0.5f, center_y - h*0.5f, 0.0f ) );
	vertices.push_back( Vector3( center_x + w*0.5f, center_y + h*0.5f, 0.0f ) );

	//texture coordinates
	uvs.push_back( Vector2(1.0f,flip_uvs ? 0.0f : 1.0f) );
	uvs.push_back( Vector2(0.0f,flip_uvs ? 1.0f : 0.0f) );
	uvs.push_back( Vector2(1.0f,flip_uvs ? 1.0f : 0.0f) );
	uvs.push_back( Vector2(0.0f,flip_uvs ? 0.0f : 1.0f) );
	uvs.push_back( Vector2(0.0f,flip_uvs ? 1.0f : 0.0f) );
	uvs.push_back( Vector2(1.0f,flip_uvs ? 0.0f : 1.0f) );

	//all of them have the same normal
	normals.push_back( Vector3(0.0f,0.0f,1.0f) );
	normals.push_back( Vector3(0.0f,0.0f,1.0f) );
	normals.push_back( Vector3(0.0f,0.0f,1.0f) );
	normals.push_back( Vector3(0.0f,0.0f,1.0f) );
	normals.push_back( Vector3(0.0f,0.0f,1.0f) );
	normals.push_back( Vector3(0.0f,0.0f,1.0f) );
}


void Mesh::createPlane(float size)
{
	vertices.clear();
	normals.clear();
	uvs.clear();
	colors.clear();

	//create six vertices (3 for upperleft triangle and 3 for lowerright)

	vertices.push_back( Vector3(size,0,size) );
	vertices.push_back( Vector3(size,0,-size) );
	vertices.push_back( Vector3(-size,0,-size) );
	vertices.push_back( Vector3(-size,0,size) );
	vertices.push_back( Vector3(size,0,size) );
	vertices.push_back( Vector3(-size,0,-size) );

	//all of them have the same normal
	normals.push_back( Vector3(0,1,0) );
	normals.push_back( Vector3(0,1,0) );
	normals.push_back( Vector3(0,1,0) );
	normals.push_back( Vector3(0,1,0) );
	normals.push_back( Vector3(0,1,0) );
	normals.push_back( Vector3(0,1,0) );

	//texture coordinates
	uvs.push_back( Vector2(1,1) );
	uvs.push_back( Vector2(1,0) );
	uvs.push_back( Vector2(0,0) );
	uvs.push_back( Vector2(0,1) );
	uvs.push_back( Vector2(1,1) );
	uvs.push_back( Vector2(0,0) );
}

bool Mesh::writeBinMESH(const char * filename) {

	meshValues values;
	values.info[0] = 'M';
	values.info[1] = 'E';
	values.info[2] = 'S';
	values.info[3] = 'H';

	values.num_normals = normals.size();
	values.num_vertex = vertices.size();
	values.num_uvs = uvs.size();
	values.center = center;
	values.halfsize = halfsize;

	FILE* file = fopen(filename,"wb");
	if (file == NULL) return false;
	
	fwrite(&values,sizeof(meshValues),1,file);

	if (!values.num_vertex) {
		std::cout << "No hay vertices" << std::endl;
		exit(0);
	}
							fwrite(&vertices[0], sizeof(Vector3), vertices.size(), file);
	if (values.num_uvs)		fwrite(&uvs[0], sizeof(Vector2), uvs.size(), file);
	if (values.num_normals)	fwrite(&normals[0], sizeof(Vector3), normals.size(), file);
	
	fclose(file);

	return true;
}

bool Mesh::readBinMESH(const char * filename) {

	meshValues values;
	FILE* file = fopen(filename, "rb");

	if (file == NULL) return FALSE;
	
	fread(&values,sizeof(meshValues),1,file);
	
	vertices.resize(values.num_vertex);
	uvs.resize(values.num_uvs);
	normals.resize(values.num_normals);
	
	center = values.center;
	halfsize = values.halfsize;

	fread(&vertices[0], sizeof(Vector3),values.num_vertex,file);
	if (values.num_uvs)fread(&uvs[0], sizeof(Vector2), values.num_uvs, file);
	if(values.num_normals) fread(&normals[0], sizeof(Vector3), values.num_normals, file);
	
	fclose(file);

	return true;
}

bool Mesh::createASEmesh(const char * meshname) {

	long t1 = getTime();
	std::string readFile;
	readFile = meshname + std::string(".bin");

	if (readBinMESH(readFile.c_str())) {
		long t2 = getTime();
		float tfinal = (t2 - t1) * 0.001;
		//std::cout << "printASEmesh ha tardado: " << tfinal << " segundos en generar todos lo vertices. " << std::endl;
		return TRUE;
	}

	/*Inicializo el textparser y le paso el archivo de donde voy a coger la mesh*/
	TextParser * t = new TextParser();
	if (t->create(meshname)) {
		
		/*Limpio todos los buffers para asegurarme que no hay datos antiguos.*/
		vertices.clear();
		normals.clear();
		uvs.clear();
		colors.clear();

		/*Saco el numero de vertices y el numero de caras de la mesh*/
		t->seek("*MESH_NUMVERTEX");
		int vert_num = t->getint();

		t->seek("*MESH_NUMFACES");
		int face_num = t->getint();

		std::vector<Vector3> vertices_unicos;

		//std::cout << "El numero de vertices es: " << vert_num << std::endl;
		//std::cout << "El numero de caras es: " << face_num << std::endl;

		int x = std::numeric_limits<int>::infinity();
		int y = std::numeric_limits<int>::infinity();
		int z = std::numeric_limits<int>::infinity();

		Vector3 min = Vector3(x,y,z);
		Vector3 max = Vector3(-x,-y,-z);

		/*Extraemos los vertices unicos de la amesh*/
		for (int i = 0; i < vert_num; i++) {
			t->seek("*MESH_VERTEX");
			t->getint();
			float x = t->getfloat();
			float z = t->getfloat();
			float y = t->getfloat();

			if (x < min.x) min.x = x;
				else if(x > max.x) max.x = x;
			
			if (y < min.y) min.y = y;
				else if (y > max.y) max.y = y;
			
			if (z < min.z) min.z = z;
				else if (z > max.z) max.z = z;

			vertices_unicos.push_back(Vector3(x, y, z));
		}
			/*Calculamos El centro y el HalfSize de la mesh*/
			center = (max + min) * 0.5;
			halfsize = max - center;
			
		/*Con los datos de los puntos unicos antes extraidos generamos los puntos de todos los triangulos y los guardamos en vertices para ser pintados*/
		for (int i = 0; i < face_num; i++) {
			t->seek("*MESH_FACE");
			t->getword();
			t->getword();
			int A = t->getint();
			t->getword();
			int B = t->getint();
			t->getword();
			int C = t->getint();

			vertices.push_back(vertices_unicos[A]);
			vertices.push_back(vertices_unicos[C]);
			vertices.push_back(vertices_unicos[B]);
		}

		/*Cargamos las vertex de textura*/
		t->seek("*MESH_NUMTVERTEX");
		int tVertex_num = t->getint();
		std::vector< Vector2 > uvs_unicas;

		for (int i = 0; i < tVertex_num; i++) {
			t->seek("*MESH_TVERT");
			t->getint();
			
			float x = t->getfloat();
			float y = t->getfloat();

			uvs_unicas.push_back(Vector2(x, y));
		}
		
		t->seek("*MESH_NUMTVFACES");
		int tFaces_num = t->getint();

		/*Indexa los verticess*/
		for (int i = 0; i < tFaces_num; i++) {
			t->seek("*MESH_TFACE");
			t->getword();
			int A = t->getint();
			int B = t->getint();
			int C = t->getint();

			uvs.push_back(uvs_unicas[A]);
			uvs.push_back(uvs_unicas[C]);
			uvs.push_back(uvs_unicas[B]);
		}

		/*Cargamos las Normales de los Vertices*/
		for (int i = 0; i < 3 * face_num; i++) {
			
			t->seek("*MESH_VERTEXNORMAL");
			t->getword();
			
			float A = t->getfloat();
			float B = t->getfloat();
			float C = t->getfloat();

			normals.push_back(Vector3(A,C,-B));
		}
		
		std::string filename;
		filename = meshname + std::string(".bin");
		writeBinMESH(filename.c_str());

		/*Calculamos el tiempo que tarda en crear todos lo vertices*/
		long t2 = getTime();
		float tfinal = (t2 - t1) * 0.001;
		//std::cout << "printASEmesh ha tardado: " << tfinal << " segundos en generar todos lo vertices. " << std::endl;
	}

	else {
		std::cout << "La mesh seleccionada no se ha podido cargar" << std::endl;
		return false;
		exit(0);
	}
}


MeshManager * MeshManager::instance = NULL;

Mesh * MeshManager::getMesh(const char * meshName){
	std::string path = std::string("data/")+meshName;

	std::map<std::string, Mesh*>::iterator it;
	it = projectMeshes.find(path);

	if (it != projectMeshes.end()) return it->second;
	
	else {
		Mesh * nMesh = new Mesh();
		nMesh->createASEmesh(path.c_str());
		projectMeshes[path] = nMesh;
		nMesh->uploadToVRAM();
		return nMesh;
	}
}

