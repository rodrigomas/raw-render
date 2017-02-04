#ifndef _AABB_H_
#define _AABB_H_

#include "algebra.h"

#ifdef __cplusplus
extern "C" {
#endif

struct _AABB
{
	Vector pMin;

	Vector pMax;
};

typedef struct _AABB * AABB;

AABB aabbCreate(Vector vmin, Vector vmax);

void aabbDestroy(AABB aabb);

void aabbSetDiagonal(AABB aabb, Vector pMin, Vector pMax);

BOOL aabbIntersects(AABB aabb, AABB box);

void aabbRenderWireframe(AABB aabb);

#ifdef __cplusplus
}
#endif

#endif