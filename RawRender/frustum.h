#ifndef _FRUSTUM_H_
#define _FRUSTUM_H_

#include "algebra.h"
#include "aabb.h"

#ifdef __cplusplus
extern "C" {
#endif

struct _Plane
{
	Vector n;

	float p;
};

typedef struct _Plane Plane;

int planeEvaluatePoint(Plane *pln, Vector *pt);

struct _Frustum
{
	Plane pNear;
	Plane pFar;
	Plane pLeft;
	Plane pRight;
	Plane pUp;
	Plane pDown;
};

typedef struct _Frustum* Frustum;

void frtBuildFrustum(Frustum frt);

Frustum frtCreate(Vector pos, Vector center, Vector up, float fovy, float aspect, float znear, float zfar);

int frtContains(Frustum frt, AABB box);

void frtPrintPlanes(Frustum frt);

#ifdef __cplusplus
}
#endif


#endif
