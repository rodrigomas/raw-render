#ifndef _OCTREE_H_
#define _OCTREE_H_

#include "octreenode.h"
#include "ray.h"
#include "linkedlist.h"

#ifdef __cplusplus
extern "C" {
#endif

struct _Octree
{
	BOOL active;

	AABB box;

	OctreeNode root;

	OctreeNode outerLeaf;

	BOOL renderBox;

	int minBoxRenderLevel;

	LinkedList objects;
};

typedef struct _Octree* Octree;


Octree octreeCreate(AABB box, int maxLevel);

void octreeDestroy(Octree octree);

void octreeRender(Octree octree, Frustum f, BOOL evenFrame);

void octreeAddObject(Octree octree, Object obj);

Object octreeGetFirstIntersection(Octree octree, Ray r, float *distance, Vector *normal, Object skip);

void octreeEnableBoxRender(Octree octree, BOOL enable, int minLevel);

BOOL octreeShouldRenderBox(Octree octree);

int octreeGetMinBoxRenderLevel(Octree octree);

void octreeSetActive(Octree octree, BOOL a);

#ifdef __cplusplus
}
#endif

#endif
