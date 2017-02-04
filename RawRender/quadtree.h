#ifndef _QUAD_TREE_H_
#define _QUAD_TREE_H_

#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <time.h>

#include <iup.h>                  
#include <iupgl.h>              
#include <gl/gl.h>            
#include <gl/glu.h>        
#include <gl/glut.h>       

struct Rectangle
{
	int xmin, xmax, ymin, ymax;
};

typedef int(*IQuadTreeSubRule)(void*, int, int, int, int);

struct _QuadTree
{
	struct Rectangle boundary;

	//4 children
	struct _QuadTree* tl;
	struct _QuadTree* tr;
	struct _QuadTree* bl;
	struct _QuadTree* br;
};

typedef struct _QuadTree* QuadTree;

QuadTree qudCreate(int xmin, int xmax, int ymin, int ymax);

void qudDestroy(QuadTree quad);

int qudIsLeaf(QuadTree quad);

void qudFillLeafs(QuadTree quad, struct Rectangle *rects, int size);

void qudRender2D(QuadTree quad, int level, int maxLevel, int *colors);

void qudComputeSubdividion(QuadTree quad, IQuadTreeSubRule subdiv, void* data, int level, int maxLevel);

int qudGetLeafSize(QuadTree quad);

#endif // ! 

