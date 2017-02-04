#include "octreenode.h"

OctreeNode octreeNodeCreate(AABB box, int level, int maxLevel, BOOL leaf)
{
	OctreeNode node = (struct _OctreeNode *)malloc(sizeof(struct _OctreeNode));

	for (int i = 0; i < 8; i++)
		node->child[i] = NULL;

	node->lbox = box;
	node->level = level;
	node->maxLevel = maxLevel;
	node->leaf = leaf;	
	node->objects = lstCreate();

	if (node->leaf == FALSE)
	{
		Vector min = box->pMin;
		Vector max = box->pMax;
		Vector mid = algVector((max.x + min.x) / 2.0f, (max.y + min.y) / 2.0f, (max.z + min.z) / 2.0f, 1.0);

		{
			aabbSetDiagonal(node->box[0], algVector(min.x, min.y, min.z, 1.0), algVector(mid.x, mid.y, mid.z, 1.0));
			aabbSetDiagonal(node->box[1], algVector(mid.x, min.y, min.z, 1.0), algVector(max.x, mid.y, mid.z, 1.0));
			aabbSetDiagonal(node->box[2], algVector(min.x, mid.y, min.z, 1.0), algVector(mid.x, max.y, mid.z, 1.0));
			aabbSetDiagonal(node->box[3], algVector(mid.x, mid.y, min.z, 1.0), algVector(max.x, max.y, mid.z, 1.0));

			aabbSetDiagonal(node->box[4], algVector(min.x, min.y, mid.z, 1.0), algVector(mid.x, mid.y, max.z, 1.0));
			aabbSetDiagonal(node->box[5], algVector(mid.x, min.y, mid.z, 1.0), algVector(max.x, mid.y, max.z, 1.0));
			aabbSetDiagonal(node->box[6], algVector(min.x, mid.y, mid.z, 1.0), algVector(mid.x, max.y, max.z, 1.0));
			aabbSetDiagonal(node->box[7], algVector(mid.x, mid.y, mid.z, 1.0), algVector(max.x, max.y, max.z, 1.0));
		}
	}


	return node;
}

void octreeNodeDestroy(OctreeNode octreenode)
{
	for (int i = 0; i < 8; i++)
		if (octreenode->child[i] != NULL)
			octreeNodeDestroy(octreenode->child[i]);

	lstDestroy(octreenode->objects);

	free(octreenode);
}

BOOL octreeNodeIsLeaf(OctreeNode octreenode)
{
	return octreenode->leaf;
}

void octreeNodeRender(OctreeNode octreenode, Frustum f, BOOL evenFrame)
{
	int res;

	//bool renderBox = Octree::ShouldRenderBox() && (Octree::GetMinBoxRenderLevel() <= level + 1);

	BOOL renderBox = TRUE;

	for (int i = 0; i < 8; i++) 
	{
		if (octreenode->child[i] == NULL)
			continue;

		if (f == NULL) 
		{
			if (renderBox)
				aabbRenderWireframe(octreenode->box[i]);

			octreeNodeRender(octreenode->child[i], f, evenFrame);

		} else {
			
			res = frtContains(f, octreenode->box[i]);
			
			if (res >= 0) 
			{
				if (renderBox)
					aabbRenderWireframe(octreenode->box[i]);

				octreeNodeRender(octreenode->child[i], res == 1 ? NULL : f, evenFrame);
			}
		}
	}
}

void octreeNodeAddObject(OctreeNode octreenode, Object obj)
{
	AABB bb = obj->boundingBox;

	if (octreeNodeIsLeaf(octreenode) == TRUE)
	{
		lstAddValue(octreenode->objects, (void*)obj, TRUE);

		return;
	}

	for (int i = 0; i < 8; i++) {
		if (aabbIntersects(bb, octreenode->box[i]))
		{
			if (octreenode->child[i] == NULL) 
			{
				if (octreenode->level > octreenode->maxLevel - 2)
				{
					octreenode->child[i] = octreeNodeCreate(octreenode->box[i], octreenode->level + 1, octreenode->maxLevel, TRUE);
				}
				else
				{
					octreenode->child[i] = octreeNodeCreate(octreenode->box[i], octreenode->level + 1, octreenode->maxLevel, FALSE);
				}
			}

			octreeNodeAddObject(octreenode->child[i], obj);
		}
	}
}

Object octreeNodeGetFirstIntersection(OctreeNode octreenode, Ray r, float *distance, Vector *normal, Object skip)
{
	Object obj = NULL, ret = NULL;

	Object closer = NULL;
	float minD = -1;
	float d = -1;
	Vector minN, n, contact;

	if (octreeNodeIsLeaf(octreenode) == TRUE)
	{
		register LinkedListNode it = lstGetHead(octreenode->objects);

		while (it != NULL)
		{			
			Object o = (Object)it->value;

			if (skip != NULL && (o) == skip) continue;

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

			it = it->next;
		}

		if (minD >= 0 && (*distance == -1 || *distance >= minD)) {
			*distance = minD;
			*normal = minN;
			return closer;
		}

		return NULL;
	}
	else
	{
		for (int i = 0; i < 8; i++)
		{
			if (octreenode->child[i] != NULL && rayIntersects(r, octreenode->box[i]) == TRUE)
			{
				ret = octreeNodeGetFirstIntersection(octreenode->child[i], r, distance, normal, skip);

				if (ret != NULL) obj = ret;
			}
		}
	}

	return obj;
}