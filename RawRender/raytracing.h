#ifndef _RAYTRACING_H_
#define _RAYTRACING_H_

#include "scene.h"
#include "algebra.h"
#include "color.h"
#include "octree.h"

#ifdef __cplusplus
extern "C" {
#endif


extern int flagAmbiente;
extern int flagDifusa;
extern int flagEspecular;
extern int flagSombra;
extern int flagRefracao;

extern const int MAX_DEPTH;

extern int curDepth;

#ifndef M_PI
#define M_PI 3.14159265359
#endif

#define GI_RADIUS 0.250

Color rayTrace( Scene scene, int depth, int x, int y);

#ifdef __cplusplus
}
#endif

#endif
