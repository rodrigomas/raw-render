#include "scene.h"
#include "linkedlist.h"
#include "raytracing.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/timeb.h>


int sceLoadOBJ(Scene scene, const char* filename, int idxMat, int defMat, Material **matV, int *matCnt, Object **objV, int *objCnt)
{
	FILE * file = fopen(filename, "r");

	int isEOF = 0;

	char currMat[128] = "000";

	char *matext = strstr(filename, ".obj");

	if (file == NULL)
	{
		printf("Impossible to open the file !\n");
		return 1;
	}	

	int nfaces = 0;

	int NewM = 0;
	Color ka;
	Color kd;
	Color ks;

	char map_Ka[30];
	char map_Kd[30];
	char map_Ks[30];
	char map_Ns[30];
	char map_Bump[30];
	char map_Disp[30];

	float Ns;

	LinkedList temp_material;
	LinkedList materials;

	LinkedList temp_vertices;
	LinkedList temp_uvs;
	LinkedList temp_normals;

	LinkedList vertexIndices;
	LinkedList uvIndices;
	LinkedList normalIndices;

	LinkedList triangles;

	strcpy(currMat, defMat);

	temp_material = lstCreate();
	materials = lstCreate();

	temp_vertices = lstCreate();
	temp_uvs = lstCreate();
	temp_normals = lstCreate();

	vertexIndices = lstCreate();
	uvIndices = lstCreate();
	normalIndices = lstCreate();

	triangles = lstCreate();

	while (!isEOF)
	{
		char lineHeader[128];

		int res = fscanf(file, "%s", lineHeader);

		if (res == EOF)
			break;

		if (strcmp(lineHeader, "v") == 0)
		{
			Vector *v = (Vector*)malloc(sizeof(Vector));

			fscanf(file, "%f %f %f\n", &v->x, &v->y, &v->z);

			lstAddValue(temp_vertices, v, 0);
		}
		else if (strcmp(lineHeader, "vt") == 0)
		{
			Vector *uv = (Vector*)malloc(sizeof(Vector));

			fscanf(file, "%f %f\n", &uv->x, &uv->y);

			lstAddValue(temp_uvs, uv, 0);
		}
		else if (strcmp(lineHeader, "vn") == 0)
		{
			Vector *normal = (Vector*)malloc(sizeof(Vector));

			fscanf(file, "%f %f %f\n", &normal->x, &normal->y, &normal->z);

			lstAddValue(temp_normals, normal, 0);
		}
		else if (strcmp(lineHeader, "f") == 0)
		{
			unsigned int *vertexIndex, *uvIndex, *normalIndex;

			vertexIndex = (unsigned int*)malloc(sizeof(unsigned int) * 3);
			uvIndex = (unsigned int*)malloc(sizeof(unsigned int) * 3);
			normalIndex = (unsigned int*)malloc(sizeof(unsigned int) * 3);

			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);

			if (matches != 9)
			{
				printf("File can't be read by our simple parser : ( Try exporting with other options\n");
				return 2;
			}

			lstAddValue(vertexIndices, &vertexIndex[0], 0);
			lstAddValue(vertexIndices, &vertexIndex[1], 0);
			lstAddValue(vertexIndices, &vertexIndex[2], 0);

			lstAddValue(uvIndices, &uvIndex[0], 0);
			lstAddValue(uvIndices, &uvIndex[1], 0);
			lstAddValue(uvIndices, &uvIndex[2], 0);

			lstAddValue(normalIndices, &normalIndex[0], 0);
			lstAddValue(normalIndices, &normalIndex[1], 0);
			lstAddValue(normalIndices, &normalIndex[2], 0);

			char *ff = (char*)malloc((strlen(currMat) + 1) * sizeof(char));

			strcpy(ff, currMat);

			lstAddValue(temp_material, ff, 0);

			nfaces++;
			
		}
		else if (strcmp(lineHeader, "usemtl") == 0)
		{
			fscanf(file, "%s", currMat);
		}
	}

	fclose(file);	

	strncpy(matext, ".mat", 4);

	file = fopen(filename, "r");

	if (file == NULL)
	{
		printf("Impossible to open the file !\n");
		return 3;
	}

	while (!isEOF)
	{
		char lineHeader[128];

		int res = fscanf(file, "%s", lineHeader);

		if (res == EOF)
			break;

		if (strcmp(lineHeader, "newmtl") == 0)
		{
			if (NewM == 1)
			{
				Material m = matCreate(NULL, kd, ks, Ns, 1.0, 0.0, 1.0);

				if (strcmp(map_Kd, "") != 0)
				{
					m->diffuseTexture = imageLoad(map_Kd);
				}

				if (strcmp(map_Ks, "") != 0)
				{
					m->specularTexture = imageLoad(map_Kd);
				}

				if (strcmp(map_Bump, "") != 0)
				{
					m->normalTexture = imageLoad(map_Bump);
				}

				if (strcmp(map_Disp, "") != 0)
				{
					m->displacementTexture = imageLoad(map_Disp);
				}

				lstAddValue(materials, m, 0);				
			}

			fscanf(file, "%s", currMat);
			NewM = 1;

			Ns = 0;
			
			memset(map_Ka, 0, sizeof(map_Ka));
			memset(map_Kd, 0, sizeof(map_Kd));
			memset(map_Ks, 0, sizeof(map_Ks));
			memset(map_Ns, 0, sizeof(map_Ns));
			memset(map_Bump, 0, sizeof(map_Bump));
			memset(map_Disp, 0, sizeof(map_Disp));

			memset(&ka, 0, sizeof(Color));
			memset(&kd, 0, sizeof(Color));
			memset(&ks, 0, sizeof(Color));
			
		}
		else if (strcmp(lineHeader, "Ka") == 0)
		{
			fscanf(file, "%f %f %f", &ka.red, &ka.green, &ka.blue);
		}
		else if (strcmp(lineHeader, "Kd") == 0)
		{
			fscanf(file, "%f %f %f", &kd.red, &kd.green, &kd.blue);
		}
		else if (strcmp(lineHeader, "Ks") == 0)
		{
			fscanf(file, "%f %f %f", &ks.red, &ks.green, &ks.blue);
		}
		else if (strcmp(lineHeader, "map_Ka") == 0)
		{
			fscanf(file, "%s", map_Ka);
		}
		else if (strcmp(lineHeader, "map_Kd") == 0)
		{
			fscanf(file, "%s", map_Kd);
		}
		else if (strcmp(lineHeader, "map_Ks") == 0)
		{
			fscanf(file, "%s", map_Ks);
		}
		else if (strcmp(lineHeader, "map_Ns") == 0)
		{
			fscanf(file, "%s", map_Ns);
		}
		else if (strcmp(lineHeader, "map_Bump") == 0 || strcmp(lineHeader, "bump") == 0)
		{
			fscanf(file, "%s", map_Bump);
		}
		else if (strcmp(lineHeader, "map_Disp") == 0 || strcmp(lineHeader, "disp") == 0)
		{
			fscanf(file, "%s", map_Disp);
		}
		else if (strcmp(lineHeader, "Ns") == 0)
		{
			fscanf(file, "%f", &Ns);
		}		
	}

	fclose(file);

	////////////////////////////

	LinkedListNode fst = lstGetHead(materials);

	int mtss = lstGetSize(materials);
	Material *mts = (Material *)malloc(mtss * sizeof(Material));
	register int i = 0;

	while (fst != NULL)
	{
		mts[i] = (Material)fst->value;

		fst = fst->next;
	}

	lstDestroy(materials);

	/////////////////////////

	int nverts = lstGetSize(temp_vertices);
	Vector **vecs = (Vector**)malloc(nverts * sizeof(Vector*));
	fst = lstGetHead(temp_vertices);

	i = 0;
	while (fst != NULL)
	{
		vecs[i] = (Vector*)fst->value;

		fst = fst->next;
		i++;
	}

	lstDestroy(temp_vertices);

	///////////////////////////////

	int nuvs = lstGetSize(temp_uvs);
	Vector **uvs = (Vector**)malloc(nuvs * sizeof(Vector*));
	fst = lstGetHead(temp_uvs);

	i = 0;
	while (fst != NULL)
	{
		uvs[i] = (Vector*)fst->value;

		fst = fst->next;

		i++;
	}

	lstDestroy(temp_uvs);

	/////////////////////////////////

	int nnormals = lstGetSize(temp_normals);
	Vector **norms = (Vector**)malloc(nnormals * sizeof(Vector*));
	fst = lstGetHead(temp_normals);
	i = 0;
	while (fst != NULL)
	{
		norms[i] = (Vector*)fst->value;

		fst = fst->next;

		i++;
	}

	lstDestroy(temp_normals);

	//////////////////////////////////////

	LinkedListNode vi = lstGetHead(vertexIndices);
	LinkedListNode ni = lstGetHead(normalIndices);
	LinkedListNode ti = lstGetHead(uvIndices);
	LinkedListNode mat = lstGetHead(temp_material);

	Object *objs = (Object *)malloc(nfaces * sizeof(Object));

	i = 0;

	while (vi != NULL)
	{
		int id = defMat;

		char *matX = mat->value;

		for (register int j = 0; j < mtss; j++)
		{
			if (strcmp(matX, mts[j]) == 0)
			{
				id = idxMat + j;
			}
		}

		Vector p0 = *(vecs[*((int*)(vi->value))]); vi = vi->next;
		Vector p1 = *(vecs[*((int*)(vi->value))]); vi = vi->next;
		Vector p2 = *(vecs[*((int*)(vi->value))]); vi = vi->next;

		Vector t0 = *(uvs[*((int*)(ti->value))]); ti = ti->next;
		Vector t1 = *(uvs[*((int*)(ti->value))]); ti = ti->next;
		Vector t2 = *(uvs[*((int*)(ti->value))]); ti = ti->next;

		Vector n0 = *(norms[*((int*)(ni->value))]); ni = ni->next;
		Vector n1 = *(norms[*((int*)(ni->value))]); ni = ni->next;
		Vector n2 = *(norms[*((int*)(ni->value))]); ni = ni->next;

		objs[i] = objCreateTriangleNormal(id, p0, p1, p2, t0, t1, t2, n0, n1, n2);

		mat = mat->next;

		i++;
	}

	vi = lstGetHead(vertexIndices);
	ni = lstGetHead(normalIndices);
	ti = lstGetHead(uvIndices);
	mat = lstGetHead(temp_material);

	while (vi != NULL) { free(vi->value); vi = vi->next; }
	while (ni != NULL) { free(ni->value); ni = ni->next; }
	while (ti != NULL) { free(ti->value); ti = ti->next; }
	while (mat != NULL) { free(mat->value); mat = mat->next; }

	for (i = 0; i < nverts; i++) { free(vecs[i]); } 
	for (i = 0; i < nnormals; i++) { free(norms[i]); } 
	for (i = 0; i < nuvs; i++) { free(uvs[i]); } 

	free(vecs);
	free(norms);
	free(uvs);

	lstDestroy(vertexIndices);
	lstDestroy(normalIndices);
	lstDestroy(uvIndices);
	lstDestroy(temp_material);
	
	*matV = mts;
	*matCnt = mtss;

	*objV = objs;
	*objCnt = nfaces;

	return 0;
}

Color sceGetBackgroundColor( Scene scene, Vector eye, Vector ray )
{
	double planeDistance, divisor, distance, scaleU, scaleV;
	Vector farOrigin, farNormal, farU, farV, point, pointFromOrigin;

	if( scene->bgImage == NULL || scene->camera == NULL )
	{
		return scene->bgColor;
	}

	/* Obtém informações sobre o far plane */
	camGetFarPlane( scene->camera, &farOrigin, &farNormal, &farU, &farV );

	/* Cos(alpha) entre a normal do plano e o raio */
	divisor = algDot( ray, farNormal );

	/* Se o raio se distancia ou é paralelo ao far plane */
	if( divisor > 0 || -divisor < EPSILON )
	{
		return scene->bgColor;
	}

	planeDistance = algDot( farOrigin, farNormal );

	distance = ( ( planeDistance - algDot( eye, farNormal ) ) / divisor );

	/* Se o raio se distancia do far plane */
	if( distance < 0 )
	{
		return scene->bgColor;
	}

	point = algAdd( eye, algScale( distance, ray ) );

	pointFromOrigin = algSub( point, farOrigin );

	divisor = algDot( farU, farU );
	scaleU = ( algDot( farU, pointFromOrigin ) / divisor );

	divisor = algDot( farV, farV );
	scaleV = ( algDot( farV, pointFromOrigin ) / divisor );

	/* Se o raio não intercepta o far plane (ou seja, a imagem de fundo)... */
	if( scaleU < 0 || scaleV < 0 || scaleU > 1 || scaleV > 1 )
	{
		return scene->bgColor;
	}
	
	return imageGetPixel( scene->bgImage,
							(int)( scaleU * camGetScreenWidth( scene->camera ) ),
							(int)( scaleV * camGetScreenHeight( scene->camera ) ) );
}

Color sceGetAmbientLight( Scene scene )
{
	return scene->ambientLight;
}

Camera sceGetCamera( Scene scene )
{
    return scene->camera;
}

int sceGetObjectCount( Scene scene )
{
	return scene->objectCount;
}

Object sceGetObject( Scene scene, int index )
{
	if( index < 0 || index >= scene->objectCount )
	{
		return NULL;
	}

	return scene->objects[index];
}

int sceGetLightCount( Scene scene )
{
	return scene->lightCount;
}

Light sceGetLight( Scene scene, int index )
{
	if( index < 0 || index >= scene->lightCount )
	{
		return NULL;
	}

	return scene->lights[index];
}

int sceUpdateBoundaries(int *setted, Vector *minp, Vector *maxp, Vector *center, Vector *pos1)
{
	if (*setted == 0)
	{
		*setted = 1;
		minp->x = pos1->x;
		minp->y = pos1->y;
		minp->z = pos1->z;

		maxp->x = pos1->x;
		maxp->y = pos1->y;
		maxp->z = pos1->z;
	}
	else
	{
		if (pos1->x < minp->x) minp->x = pos1->x;
		if (pos1->y < minp->y) minp->y = pos1->y;
		if (pos1->z < minp->z) minp->z = pos1->z;

		if (pos1->x > maxp->x) maxp->x = pos1->x;
		if (pos1->y > maxp->y) maxp->y = pos1->y;
		if (pos1->z > maxp->z) maxp->z = pos1->z;
	}

	center->x += pos1->x;
	center->y += pos1->y;
	center->z += pos1->z;
}

Scene sceLoad( const char *filename )
{
	FILE *file;
	char buffer[512];

	Scene scene;
	
	Color bgColor;
	Color ambientLight;
	char backgroundFileName[FILENAME_MAXLEN];

	/* Camera */
	Vector eye = algVector( 0,0,0,1 );
	Vector at = algVector( 0,0,0,1 );
	Vector up = algVector( 0,0,0,1 );
	double fovy;
	double nearp;
	double farp;
	int screenWidth;
	int screenHeight;

	/* Material */
	Color diffuse;
	Color specular;
	double specularExponent;
	double reflective;
	double refractive;
	double opacity;
	char textureFileName[FILENAME_MAXLEN];
	
	/* Lights & Objects */
	Color lightColor;
	int material;
	Vector pos1 = algVector( 0,0,0,1 );
	Vector pos2 = algVector( 0,0,0,1 );
	Vector pos3 = algVector( 0,0,0,1 );
	Vector tex1 = algVector( 0,0,0,1 );
	Vector tex2 = algVector( 0,0,0,1 );
	Vector tex3 = algVector( 0,0,0,1 );

	Vector minp = algVector(0, 0, 0, 1);
	Vector maxp = algVector(0, 0, 0, 1);
	Vector center = algVector(0, 0, 0, 1);
	int minset = 0;

	double radius;

	int octreeMode = 1;

	int piPos = 0;

	char objectFileName[FILENAME_MAXLEN];

	char piFileName[FILENAME_MAXLEN];

	char perfImageFileName[FILENAME_MAXLEN];

	char outImageFileName[FILENAME_MAXLEN];
	
	file = fopen( filename, "rt" );
	if( !file )
	{
		return NULL;
	}

	scene = (struct _Scene *)malloc( sizeof(struct _Scene) );
	if( !scene )
	{
		return NULL;
	}

	/* Default (undefined) values: */
	scene->camera = NULL;
	scene->bgImage = NULL;
	scene->objectCount = 0;
	scene->lightCount = 0;
	scene->materialCount = 0;
	scene->maxOctreeLevel = 5;
	scene->rendered = 0;
	scene->perfImage = NULL;
	scene->Maps = NULL;
	scene->nMaps = 0;
	scene->imgWidth = 640;
	scene->imgHeight = 480;
	scene->subSampling = 1;
	scene->useGI = 0;
	scene->samples = 200;
	scene->piImagesCnt = 0;

	scene->materials = (Material*)malloc(MAX_MATERIALS * sizeof(Material));
	scene->objects = (Object*)malloc(MAX_OBJECTS * sizeof(Object));
	scene->lights = (Light*)malloc(MAX_LIGHTS * sizeof(Light));
	
	scene->lightCapacity = MAX_LIGHTS;
	scene->objectCapacity = MAX_OBJECTS;
	scene->materialCapacity = MAX_MATERIALS;
	
	while( fgets( buffer, sizeof(buffer), file ) ) 
	{
		if( sscanf( buffer, "RT %lf\n", &at ) == 1 )
		{
			/* Ignore File Version Information */
		}
		else if( sscanf( buffer, "CAMERA %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %d %d\n", &eye.x, &eye.y, &eye.z, &at.x, &at.y, &at.z, &up.x, &up.y, &up.z, &fovy, &nearp, &farp, &screenWidth, &screenHeight ) == 14) 
		{
			if( scene->camera )
			{
				camDestroy( scene->camera );
			}

			scene->camera = camCreate( eye, at, up, fovy, nearp, farp, screenWidth, screenHeight );

			scene->imgWidth = camGetScreenWidth(scene->camera);
			scene->imgHeight = camGetScreenHeight(scene->camera);

			scene->nMaps = 9;

			scene->Maps = (Vector**)malloc(scene->nMaps * sizeof(Vector*));

			for (register int i = 0; i < scene->nMaps; i++)
			{
				scene->Maps[i] = (Vector*)malloc(scene->imgWidth * scene->imgHeight  * sizeof(Vector));

				memset(scene->Maps[i], 0, scene->imgWidth * scene->imgHeight  * sizeof(Vector));
			}

		} 
		else if( sscanf( buffer, "SCENE %lf %lf %lf %lf %lf %lf %s\n", &bgColor.red, &bgColor.green, &bgColor.blue, &ambientLight.red, &ambientLight.green, &ambientLight.blue, backgroundFileName ) == 7 ) 
		{
			bgColor = colorNormalize( bgColor );
			ambientLight = colorNormalize( ambientLight );

			scene->bgColor = bgColor;
			scene->ambientLight = ambientLight;

			if( scene->bgImage )
			{
				imageDestroy( scene->bgImage );
			}

			if( strcmp( backgroundFileName, "null") == 0 )
			{
				scene->bgImage = NULL;
			} 
			else 
			{
				char completeBackgroundFileName[FILENAME_MAXLEN];
				char *slash;

				strcpy(completeBackgroundFileName, filename);
				slash = strrchr(completeBackgroundFileName, '\\');

				if (slash != NULL) {
					++slash;
					*slash = '\0';
					strcat(completeBackgroundFileName, backgroundFileName);
					scene->bgImage = imageLoad( completeBackgroundFileName );
				} else {
					scene->bgImage = imageLoad( backgroundFileName );
				}
			}
		} 
		else if( sscanf( buffer, "MATERIAL %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %s\n", &diffuse.red, &diffuse.green, &diffuse.blue, &specular.red, &specular.green, &specular.blue, &specularExponent, &reflective, &refractive, &opacity, textureFileName ) == 11 ) 
		{
			Image image = NULL;
			
			if( strcmp( textureFileName, "null") != 0 )
			{
				char completeTextureFileName[FILENAME_MAXLEN];
				char *slash;

				strcpy(completeTextureFileName, filename);
				slash = strrchr(completeTextureFileName, '\\');

				if (slash != NULL) {
					++slash;
					*slash = '\0';
					strcat(completeTextureFileName, textureFileName);
					image = imageLoad( completeTextureFileName );
				} else {
					image = imageLoad( textureFileName );
				}
			}

			if( scene->materialCount >= scene->materialCapacity)
			{
				Material* m = NULL;

				m = scene->materials;

				scene->materials = (Material*)malloc(scene->materialCapacity * 2 * sizeof(Material));
				scene->materialCapacity = scene->materialCapacity * 2;

				for (register int i = 0; i < scene->materialCount - 1; i++)
				{
					scene->materials[i] = m[i];
				}

				free(m);
				//imageDestroy( image );
				//fprintf( stderr, "sceLoad: Foi ultrapassado o limite de definicoes de materiais na cena. Ignorando." );
				//continue;
			}

			diffuse = colorNormalize( diffuse );
			specular = colorNormalize( specular );

			scene->materials[scene->materialCount++] = matCreate( image, diffuse, specular, specularExponent, reflective, refractive, opacity );
		} 
		else if( sscanf( buffer, "LIGHT %lf %lf %lf %lf %lf %lf\n", &pos1.x, &pos1.y, &pos1.z, &lightColor.red, &lightColor.green, &lightColor.blue ) == 6 )
		{
			if( scene->lightCount >= scene->lightCapacity)
			{
				Light* l = NULL;
				l = scene->lights;

				scene->lights = (Light*)malloc(scene->lightCapacity * 2 * sizeof(Light));
				scene->lightCapacity = scene->lightCapacity * 2;

				for (register int i = 0; i < scene->lightCount - 1; i++)
				{
					scene->lights[i] = l[i];
				}

				free(l);
				//fprintf( stderr, "sceLoad: Foi ultrapassado o limite de definicoes de luzes na cena. Ignorando." );
				//continue;
			}

			lightColor = colorNormalize( lightColor );

			scene->lights[scene->lightCount++] = lightCreate( pos1, lightColor );
		} 
		else if( sscanf( buffer, "SPHERE %d %lf %lf %lf %lf\n", &material, &radius, &pos1.x,&pos1.y,&pos1.z ) == 5 ) 
		{
			if (scene->objectCount >= scene->objectCapacity)
			{
				Object* o = NULL;
				o = scene->objects;

				scene->objects = (Object*)malloc(scene->objectCount * 2 * sizeof(Object));
				scene->objectCapacity = scene->objectCapacity * 2;

				for (register int i = 0; i < scene->objectCount - 1; i++)
				{
					scene->objects[i] = o[i];
				}

				free(o);
			}

			sceUpdateBoundaries(&minset, &minp, &maxp, &center, &pos1);			

			//if( scene->objectCount >= MAX_OBJECTS )
			//{
			//	fprintf( stderr, "sceLoad: Foi ultrapassado o limite de definicoes de objetos na cena. Ignorando." );
			//	continue;
			//}

			scene->objects[scene->objectCount++] = objCreateSphere( material, pos1, radius );
		} 
		else if( sscanf( buffer, "TRIANGLE %d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf\n", &material, &pos1.x, &pos1.y, &pos1.z, &pos2.x, &pos2.y, &pos2.z, &pos3.x, &pos3.y, &pos3.z, &tex1.x, &tex1.y, &tex2.x, &tex2.y, &tex3.x, &tex3.y) == 16 ) 
		{
		/*	if( scene->objectCount >= MAX_OBJECTS )
			{
				fprintf( stderr, "sceLoad: Foi ultrapassado o limite de definicoes de objetos na cena. Ignorando." );
				continue;
			}*/

			if (scene->objectCount >= scene->objectCapacity)
			{
				Object* o = NULL;
				o = scene->objects;

				scene->objects = (Object*)malloc(scene->objectCount * 2 * sizeof(Object));
				scene->objectCapacity = scene->objectCapacity * 2;

				for (register int i = 0; i < scene->objectCount - 1; i++)
				{
					scene->objects[i] = o[i];
				}

				free(o);
			}

			sceUpdateBoundaries(&minset, &minp, &maxp, &center, &pos1);
			sceUpdateBoundaries(&minset, &minp, &maxp, &center, &pos2);
			sceUpdateBoundaries(&minset, &minp, &maxp, &center, &pos3);

			scene->objects[scene->objectCount++] = objCreateTriangle( material, pos1, pos2, pos3, tex1, tex2, tex3 );
		}
	  	else if( sscanf( buffer, "BOX %d %lf %lf %lf %lf %lf %lf\n", &material, &pos1.x, &pos1.y, &pos1.z, &pos2.x, &pos2.y, &pos2.z ) == 7 ) 
		{
			/*if( scene->objectCount >= MAX_OBJECTS )
			{
				fprintf( stderr, "sceLoad: Foi ultrapassado o limite de definicoes de objetos na cena. Ignorando." );
				continue;
			}*/

			if (scene->objectCount >= scene->objectCapacity)
			{
				Object* o = NULL;
				o = scene->objects;

				scene->objects = (Object*)malloc(scene->objectCount * 2 * sizeof(Object));
				scene->objectCapacity = scene->objectCapacity * 2;

				for (register int i = 0; i < scene->objectCount - 1; i++)
				{
					scene->objects[i] = o[i];
				}

				free(o);
			}

			sceUpdateBoundaries(&minset, &minp, &maxp, &center, &pos1);
			sceUpdateBoundaries(&minset, &minp, &maxp, &center, &pos2);

			scene->objects[scene->objectCount++] = objCreateBox( material, pos1, pos2 );
		}
		else if (sscanf(buffer, "PI %s\n", perfImageFileName) == 1)
		{
			scene->perfImage = piLoad(perfImageFileName);
		}
		else if (sscanf(buffer, "PICNT %d\n", &scene->piImagesCnt) == 1)
		{
			scene->piImages = (PerfomanceImage*)malloc(scene->piImagesCnt * sizeof(PerfomanceImage));

			for (register int i = 0; i < scene->piImagesCnt; i++)
			{
				scene->piImages[i] = NULL;
			}
		}
		else if (sscanf(buffer, "PIP %d %s\n", &piPos, piFileName) == 1)
		{
			scene->piImages[piPos] = piLoad(piFileName);
		}
		else if (sscanf(buffer, "OBJ %s\n", objectFileName) == 1)
		{
			Material *mx;
			int mcnt = 0;

			Object *ox;
			int ocnt = 0;

			sceLoadOBJ(scene, objectFileName, scene->materialCount, 0, &mx, &mcnt, &ox, &ocnt);

			if (scene->objectCount + ocnt >= scene->objectCapacity)
			{
				Object* o = NULL;
				o = scene->objects;

				scene->objects = (Object*)malloc( (scene->objectCount * 2 + ocnt) * sizeof(Object));
				scene->objectCapacity = scene->objectCount * 2 + ocnt;

				for (register int i = 0; i < scene->objectCount - 1; i++)
				{
					scene->objects[i] = o[i];
				}

				free(o);
			}

			for (register int i = 0; i < ocnt; i++)
			{
				scene->objects[scene->objectCount++] = ox[i];

				Triangle *t = (Triangle*)scene->objects[scene->objectCount++]->data;

				sceUpdateBoundaries(&minset, &minp, &maxp, &center, &t->v0);
				sceUpdateBoundaries(&minset, &minp, &maxp, &center, &t->v1);
				sceUpdateBoundaries(&minset, &minp, &maxp, &center, &t->v2);
			}

			free(ox);

			if (scene->materialCount >= scene->materialCapacity)
			{
				Material* m = NULL;

				m = scene->materials;

				scene->materials = (Material*)malloc(scene->materialCapacity * 2 * sizeof(Material));
				scene->materialCapacity = scene->materialCapacity * 2;

				for (register int i = 0; i < scene->materialCount - 1; i++)
				{
					scene->materials[i] = m[i];
				}

				//imageDestroy( image );
				//fprintf( stderr, "sceLoad: Foi ultrapassado o limite de definicoes de materiais na cena. Ignorando." );
				//continue;
			}

			for (register int i = 0; i < mcnt; i++)
			{
				scene->materials[scene->materialCount++] = mx[i];
			}

			free(mx);
		}
		else if (sscanf(buffer, "EXPORT %s\n", outImageFileName) == 1)
		{
			strcpy(scene->OutputPrefix, outImageFileName);
		}
		else if (sscanf(buffer, "OCTREE MODE %d\n", &octreeMode) == 1)
		{

		}
		else if (sscanf(buffer, "SUBSAMPLING %d\n", &scene->subSampling) == 1)
		{

		}
		else if (sscanf(buffer, "SAMPLES %d\n", &scene->samples) == 1)
		{

		}
		else if (sscanf(buffer, "GI MODE %d\n", &scene->useGI) == 1)
		{

		}
		else
		{
			if (buffer[0] != '!') 
			{ 
				printf( "sceLoad: Ignorando comando:\n %s\n", buffer );
			}
		}
	}

	AABB b = aabbCreate(minp, maxp);

	scene->octree = octreeCreate(b, scene->maxOctreeLevel);
	scene->octree->active = octreeMode;

	for (register int i = 0; i < scene->objectCount; i++)
	{
		octreeAddObject(scene->octree, scene->objects[i]);
	}

	if( scene->camera && scene->bgImage )
	{
		scene->bgImage = imageResize( scene->bgImage, camGetScreenWidth( scene->camera ),
			camGetScreenHeight( scene->camera ) );
	}

	if (scene->piImagesCnt > 0 && scene->perfImage == NULL)
	{
		scene->perfImage = piEstimateFromSequency(scene->piImages, scene->piImagesCnt);
	} 

	if (scene->perfImage == NULL)
	{
		PerfomanceImageData d;

		d.CpuTime = 0;
		d.RayCnt = 0;

		scene->perfImage = piCreate(scene->imgWidth, scene->imgHeight, d, 1);
	}

	fclose( file );

	return scene;
}

int sceSaveOutput(Scene scene, char *filename)
{
	if (scene->rendered == 0)
		return 0;

	char FileName[MAX_PATH];

	if (filename != NULL)
	{
		strcpy(FileName, filename);
	}

	//1=Color, 2=Depth, 3=Normal, 4=Shadow, 5=Reflection, 6=Refraction, 7=Diffuse, 8=Specular, 9=GI

	char Maps[9][50] = { "Color", "Depth", "Normal", "Shadow", "Reflection", "Refraction", "Diffuse", "Specular", "GI" };

	for (register int i = 0; i < scene->nMaps; i++)
	{
		sprintf(FileName, "%s_%s.raw", scene->OutputPrefix, Maps[i]);

		FILE *fp = fopen(FileName, "wb");

		fwrite(&scene->imgWidth, sizeof(int), 1, fp);
		fwrite(&scene->imgHeight, sizeof(int), 1, fp);

		fwrite(scene->Maps[i], sizeof(Vector), scene->imgWidth * scene->imgHeight, fp);

		fclose(fp);
	}		

	sprintf(FileName, "%s_PI.raw", scene->OutputPrefix);

	piSave(scene->perfImage, FileName);
}

void sceSetMap(Scene scene, int map, int p, double x, double y, double z, double w)
{
	scene->Maps[map][p].x = x;
	scene->Maps[map][p].y = y;
	scene->Maps[map][p].z = z;
	scene->Maps[map][p].w = w;
}

void sceIncMap(Scene scene, int map, int p, double x, double y, double z, double w)
{
	scene->Maps[map][p].x += x;
	scene->Maps[map][p].y += y;
	scene->Maps[map][p].z += z;
	scene->Maps[map][p].w += w;
}

void sceSetMapW(Scene scene, int map, int p, double w)
{
	scene->Maps[map][p].w = w;
}

void sceSetMapDiv(Scene scene, int map, int p, double value)
{
	scene->Maps[map][p].x /= value;
	scene->Maps[map][p].y /= value;
	scene->Maps[map][p].z /= value;
}

int sceGetMaterialCount( Scene scene )
{
	return scene->materialCount;
}

Material sceGetMaterial( Scene scene, int index )
{
	return scene->materials[index];
}

void sceDestroy( Scene scene )
{
	int i;

	camDestroy( scene->camera );
	imageDestroy( scene->bgImage );

	piDestroy(scene->perfImage);

	for( i = 0; i < scene->objectCount; ++i )
	{
		objDestroy( scene->objects[i] );
	}

	for( i = 0; i < scene->materialCount; ++i )
	{
		matDestroy( scene->materials[i] );
	}

	for (register int i = 0; i < scene->nMaps; i++)
	{
		free(scene->Maps[i]);
	}

	free(scene->Maps);

	octreeDestroy(scene->octree);

	free(scene->materials);
	free(scene->objects);
	free(scene->lights);
	
	free( scene );

	for (register int i = 0; i < scene->piImagesCnt; i++)
	{
		if (scene->piImages[i] != NULL)
		{
			piDestroy(scene->piImages[i]);
		}
	}

	if (scene->piImagesCnt > 0)
	{
		free(scene->piImages);
	}
}
