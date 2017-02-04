#ifndef _OCTREENODE_H_
#define _OCTREENODE_H_

#include "aabb.h"
#include "object.h"
#include "ray.h"
#include "linkedlist.h"
#include "frustum.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _OctreeNode * OctreeNode;

struct _OctreeNode
{
	OctreeNode child[8];

	AABB box[8];

	int level;

	BOOL leaf;

	int maxLevel;

	AABB lbox;

	LinkedList objects;
};

OctreeNode octreeNodeCreate(AABB box, int level, int maxLevel, BOOL leaf);

void octreeNodeDestroy(OctreeNode octreenode);

BOOL octreeNodeIsLeaf(OctreeNode octreenode);

void octreeNodeRender(OctreeNode octreenode, Frustum f, BOOL evenFrame);

void octreeNodeAddObject(OctreeNode octreenode, Object obj);

Object octreeNodeGetFirstIntersection(OctreeNode octreenode, Ray r, float *distance, Vector *normal, Object skip);

#ifdef __cplusplus
}
#endif

#endif
