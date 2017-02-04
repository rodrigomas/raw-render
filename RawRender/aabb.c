#ifdef _WIN32
#include <Windows.h>
#endif

#include <GL/glu.h>
#include <GL/gl.h>

#include "aabb.h"


AABB aabbCreate(Vector vmin, Vector vmax)
{
	AABB aabb = (struct _AABB *)malloc(sizeof(struct _AABB));

	aabb->pMax = vmax;
	aabb->pMin = vmin;

	return aabb;
}

void aabbDestroy(AABB aabb)
{

}

void aabbSetDiagonal(AABB aabb, Vector pMin, Vector pMax)
{
	if (pMin.x > pMax.x || pMin.y > pMax.y || pMin.z > pMax.z)
		return;

	aabb->pMax = pMax;
	aabb->pMin = pMin;
}

BOOL aabbIntersects(AABB aabb, AABB  box)
{
	if (((aabb->pMin.x >= box->pMin.x && aabb->pMin.x <= box->pMax.x) || (box->pMin.x >= aabb->pMin.x && box->pMin.x <= aabb->pMax.x)) &&
		((aabb->pMin.y >= box->pMin.y && aabb->pMin.y <= box->pMax.y) || (box->pMin.y >= aabb->pMin.y && box->pMin.y <= aabb->pMax.y)) &&
		((aabb->pMin.z >= box->pMin.z && aabb->pMin.z <= box->pMax.z) || (box->pMin.z >= aabb->pMin.z && box->pMin.z <= aabb->pMax.z)))
	{
		return 1;
	}

	return 0;
}

void aabbRenderWireframe(AABB aabb)
{
	Vector p[8];

	p[0] = algVector(aabb->pMin.x, aabb->pMin.y, aabb->pMin.z, 1);
	p[1] = algVector(aabb->pMax.x, aabb->pMin.y, aabb->pMin.z, 1);
	p[2] = algVector(aabb->pMax.x, aabb->pMax.y, aabb->pMin.z, 1);
	p[3] = algVector(aabb->pMin.x, aabb->pMax.y, aabb->pMin.z, 1);

	p[4] = algVector(aabb->pMin.x, aabb->pMin.y, aabb->pMax.z, 1);
	p[5] = algVector(aabb->pMax.x, aabb->pMin.y, aabb->pMax.z, 1);
	p[6] = algVector(aabb->pMax.x, aabb->pMax.y, aabb->pMax.z, 1);
	p[7] = algVector(aabb->pMin.x, aabb->pMax.y, aabb->pMax.z, 1);

	glPushAttrib(GL_POLYGON_BIT | GL_LIGHTING_BIT);
	glDisable(GL_LIGHTING);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glBegin(GL_QUADS);
	glColor3f(0.3f, 0.8f, 0.3f);

	glVertex3fv((GLfloat*)&p[0]);
	glVertex3fv((GLfloat*)&p[1]);
	glVertex3fv((GLfloat*)&p[2]);
	glVertex3fv((GLfloat*)&p[3]);

	glVertex3fv((GLfloat*)&p[7]);
	glVertex3fv((GLfloat*)&p[6]);
	glVertex3fv((GLfloat*)&p[5]);
	glVertex3fv((GLfloat*)&p[4]);

	glVertex3fv((GLfloat*)&p[4]);
	glVertex3fv((GLfloat*)&p[5]);
	glVertex3fv((GLfloat*)&p[1]);
	glVertex3fv((GLfloat*)&p[0]);

	glVertex3fv((GLfloat*)&p[7]);
	glVertex3fv((GLfloat*)&p[3]);
	glVertex3fv((GLfloat*)&p[2]);
	glVertex3fv((GLfloat*)&p[6]);

	glVertex3fv((GLfloat*)&p[1]);
	glVertex3fv((GLfloat*)&p[5]);
	glVertex3fv((GLfloat*)&p[6]);
	glVertex3fv((GLfloat*)&p[2]);

	glVertex3fv((GLfloat*)&p[4]);
	glVertex3fv((GLfloat*)&p[0]);
	glVertex3fv((GLfloat*)&p[3]);
	glVertex3fv((GLfloat*)&p[7]);
	glEnd();

	glPopAttrib();
}