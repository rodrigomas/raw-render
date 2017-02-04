#ifndef _SCENE_H_
#define _SCENE_H_

#include "image.h"
#include "light.h"
#include "camera.h"
#include "object.h"
#include "material.h"
#include "octree.h"
#include "performanceimage.h"

#define MAX_MATERIALS	64
#define MAX_OBJECTS		1000
#define MAX_LIGHTS		8
#define FILENAME_MAXLEN	256

#ifndef EPSILON
#define EPSILON	1.0e-10
#endif

//1 = Color, 2 = Depth, 3 = Normal, 4 = Shadow, 5 = Reflection, 6 = Refraction, 7 = Diffuse, 8 = Specular, 9 = GI

#define MAP_COLOR 0
#define MAP_DEPTH 1
#define MAP_NORMAL 2
#define MAP_SHADOW 3
#define MAP_REFLECTION 4
#define MAP_REFRACTION 5
#define MAP_DIFFUSE 6
#define MAP_SPECULAR 7
#define MAP_GI 8

#ifdef __cplusplus
extern "C" {
#endif

struct _Scene
{
	Octree octree;

	PerfomanceImage perfImage;

	PerfomanceImage *piImages;
	int piImagesCnt;

	int maxOctreeLevel;

	int rendered;	

	Vector **Maps;
	int nMaps;

	int imgWidth;
	int imgHeight;

	Camera camera;

	Color bgColor;

	Color ambientLight;

	Image bgImage;

	int materialCount;
	int materialCapacity;

	Material *materials;

	int objectCount;
	int objectCapacity;

	Object *objects;

	int lightCount;
	int lightCapacity;

	Light *lights;

	char OutputPrefix[MAX_PATH];

	int useGI;
	int subSampling;
	int samples;
};

typedef struct _Scene * Scene;


Color sceGetBackgroundColor( Scene scene, Vector eye, Vector ray );

Color sceGetAmbientLight( Scene scene );

Camera sceGetCamera( Scene scene );

int sceGetObjectCount( Scene scene );

Object sceGetObject( Scene scene, int index );

int sceGetLightCount( Scene scene );

Light sceGetLight( Scene scene, int index );

Scene sceLoad( const char *filename );

int sceGetMaterialCount( Scene scene );

Material sceGetMaterial( Scene scene, int index );

void sceDestroy( Scene scene );

int sceLoadOBJ(Scene scene, const char* filename, int idxMat, int defMat, Material **matV, int *matCnt, Object **objV, int *objCnt);

int sceSaveOutput(Scene scene, char *filename);

void sceSetMapW(Scene scene, int map, int p, double w);
void sceSetMap(Scene scene, int map, int p, double x, double y, double z, double w);
void sceIncMap(Scene scene, int map, int p, double x, double y, double z, double w);
void sceSetMapDiv(Scene scene, int map, int p, double value);

#ifdef __cplusplus
}
#endif

#endif

