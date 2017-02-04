#include "octree.h"

Octree octreeCreate(AABB box, int maxLevel)
{
	Octree node = (struct _Octree *)malloc(sizeof(struct _Octree));

	node->active = TRUE;

	node->root = octreeNodeCreate(box, 0, maxLevel, FALSE);

	node->outerLeaf = octreeNodeCreate(box, maxLevel, maxLevel, TRUE);
	node->renderBox = FALSE;
	node->box = box;
	node->objects = lstCreate();
}

void octreeDestroy(Octree octree)
{
	octreeNodeDestroy(octree->root);
	octreeNodeDestroy(octree->outerLeaf);
	lstDestroy(octree->objects);
}

void octreeRender(Octree octree, Frustum f, BOOL evenFrame)
{
	if (octree->renderBox && octree->minBoxRenderLevel <= 0) aabbRenderWireframe(octree->box);

	octreeNodeRender(octree->root, f, evenFrame);
	octreeNodeRender(octree->outerLeaf, f, evenFrame);
}

void octreeAddObject(Octree octree, Object obj)
{
	if (aabbIntersects(octree->box, obj->boundingBox) == TRUE)
		octreeNodeAddObject(octree->root, obj);
	else
		octreeNodeAddObject(octree->outerLeaf, obj);

	lstAddValue(octree->objects, (void*)obj, TRUE);
}

Object octreeGetFirstIntersection(Octree octree, Ray r, float *distance, Vector *normal, Object skip)
{	
	Object closer = NULL;
	float minD = -1;
	float d = -1;
	Vector minN, n;

	if (lstGetHead(octree->objects) == NULL)
		return NULL;

	if (!octree->active) 
	{
		register LinkedListNode it = lstGetHead(octree->objects);

		while (it != NULL)
		{
			Object o = (Object)it->value;

			if (skip != NULL && o == skip) continue;

			d = objIntercept(o, r->pos, r->dir);

			if (d > 0)
			{
				if (minD < 0 || d < minD) 
				{
					Vector p = algScale(d, r->dir);

					p = algAdd(p, r->pos);

					n = objNormalAt(o, p);

					closer = o;
					minD = d;
					minN = n;
				}
			}
		}

		if (closer != NULL) 
		{
			*distance = minD;
			*normal = minN;
		}

		return closer;
	}
	else {
	//	for (it = objects.begin(); it != objects.end(); it++)
		//	(*it)->ResetLastRay();

		Object closer;

		closer = octreeNodeGetFirstIntersection(octree->root, r, &d, &n, skip);

		if (closer != NULL) 
		{
			*distance = d;
			*normal = n;
		}

		minD = d;
		minN = n;

		float minD_ = -1;
		Vector minN_;
		Object closer_;

		closer_ = octreeNodeGetFirstIntersection(octree->outerLeaf, r, &minD_, &minN_, skip);

		if (minD_ >= 0 && minD_ < minD) 
		{
			closer = closer_;
			minD = minD_;
			minN = minN_;
		}

		*distance = minD;
		*normal = minN;

		return closer;
	}

}

void octreeEnableBoxRender(Octree octree, BOOL enable, int minLevel)
{
	octree->renderBox = enable;
	octree->minBoxRenderLevel = minLevel;
}

BOOL octreeShouldRenderBox(Octree octree)
{
	return octree->renderBox;
}

int octreeGetMinBoxRenderLevel(Octree octree)
{
	return octree->minBoxRenderLevel;
}

void octreeSetActive(Octree octree, BOOL a) 
{ 
	octree->active = a; 
}