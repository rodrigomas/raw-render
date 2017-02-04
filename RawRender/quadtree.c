#include "quadtree.h"

QuadTree qudCreate(int xmin, int xmax, int ymin, int ymax)
{
	struct _QuadTree* quad = (struct _QuadTree*)malloc(sizeof(struct _QuadTree));

	quad->boundary.xmin = xmin;
	quad->boundary.xmax = xmax;
	quad->boundary.ymin = ymin;
	quad->boundary.ymax = ymax;

	quad->tl = NULL;
	quad->tr = NULL;
	quad->bl = NULL;
	quad->br = NULL;

	return quad;
}

void qudDestroy(QuadTree quad)
{
	if (quad == NULL) return;

	qudDestroy(quad->tl);
	qudDestroy(quad->tr);
	qudDestroy(quad->bl);
	qudDestroy(quad->br);

	free(quad);
}

int qudIsLeaf(QuadTree quad)
{
	if (quad == NULL)
	{
		return 0;
	}

	return (quad->bl == NULL) && (quad->br == NULL) && (quad->tl == NULL) && (quad->tr == NULL);
}

void qudGetLeafSizeRec(QuadTree quad, int *cnt)
{
	if (qudIsLeaf(quad) == 1)
	{
		*cnt = *cnt + 1;
	}
	else
	{
		qudGetLeafSizeRec(quad->bl, cnt);
		qudGetLeafSizeRec(quad->br, cnt);
		qudGetLeafSizeRec(quad->tl, cnt);
		qudGetLeafSizeRec(quad->tr, cnt);
	}

}

void qudFillLeafsRec(QuadTree quad, struct Rectangle *rects, int size, int *idx)
{
	if (qudIsLeaf(quad) == 1)
	{
		rects[*idx] = quad->boundary;
		*idx = *idx + 1;
	}
	else
	{
		qudGetLeafSizeRec(quad->bl, rects, size, idx);
		qudGetLeafSizeRec(quad->br, rects, size, idx);
		qudGetLeafSizeRec(quad->tl, rects, size, idx);
		qudGetLeafSizeRec(quad->tr, rects, size, idx);
	}
}

void qudFillLeafs(QuadTree quad, struct Rectangle *rects, int size)
{
	int idx = 0;

	qudFillLeafsRec(quad, rects, size, &idx);
}

int qudGetLeafSize(QuadTree quad)
{
	int cnt = 0;

	qudGetLeafSizeRec(quad, &cnt);

	return cnt;
}

void qudRender2D(QuadTree quad, int level, int maxLevel, int *colors)
{
	if (quad == NULL || level >= maxLevel) return;

	glPushAttrib(GL_CURRENT_BIT);
	glColor3b(colors[level] & 0xff, (colors[level] >> 8) & 0xff, (colors[level] >> 16) & 0xff);
	glBegin(GL_LINE_LOOP);	
		glVertex2i(quad->boundary.xmin, quad->boundary.ymin);
		glVertex2i(quad->boundary.xmax, quad->boundary.ymin);
		glVertex2i(quad->boundary.xmax, quad->boundary.ymax);
		glVertex2i(quad->boundary.xmin, quad->boundary.ymax);
	glEnd();
	glPopAttrib();

	qudRender2D(quad->tl, level + 1, maxLevel, colors);
	qudRender2D(quad->tr, level + 1, maxLevel, colors);
	qudRender2D(quad->bl, level + 1, maxLevel, colors);
	qudRender2D(quad->br, level + 1, maxLevel, colors);
}

void qudComputeSubdividion(QuadTree quad, IQuadTreeSubRule subdiv, void* data, int level, int maxLevel)
{
	int needSubDiv = 0;

	if (level >= maxLevel)
		return;

	if (data != NULL && subdiv != NULL && quad !=  NULL)
	{
		needSubDiv = subdiv(data, quad->boundary.xmin, quad->boundary.xmax, quad->boundary.ymin, quad->boundary.ymax);

		if (needSubDiv == 1)
		{
			quad->tl = qudCreate(quad->boundary.xmin, quad->boundary.xmax / 2, quad->boundary.ymin, quad->boundary.ymax / 2);
			quad->tr = qudCreate(quad->boundary.xmax / 2, quad->boundary.xmax, quad->boundary.ymin, quad->boundary.ymax / 2);
			quad->bl = qudCreate(quad->boundary.xmin, quad->boundary.xmax / 2, quad->boundary.ymax / 2, quad->boundary.ymax);
			quad->br = qudCreate(quad->boundary.xmax / 2, quad->boundary.xmax, quad->boundary.ymax / 2, quad->boundary.ymax);

			qudComputeSubdividion(quad->tl, subdiv, data, level + 1, maxLevel);
			qudComputeSubdividion(quad->tr, subdiv, data, level + 1, maxLevel);
			qudComputeSubdividion(quad->bl, subdiv, data, level + 1, maxLevel);
			qudComputeSubdividion(quad->br, subdiv, data, level + 1, maxLevel);
		}
	}	
}

