#include <stdio.h>
#include <math.h>
#include "frustum.h"

#ifdef _WIN32
#include <Windows.h>
#endif

#include <GL/glu.h>
#include <GL/gl.h>

int planeEvaluatePoint(Plane *pln, Vector *pt)
{
	double a = algDot(*pt, pln->n);
	a += pln->p;

	if (a == 0.0f) return 0;
	if (a > 0.0f) return 1;
	return -1;
}

void frtBuildFrustum(Frustum frt)
{
	GLfloat m[16];

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	glGetFloatv(GL_MODELVIEW_MATRIX, m);
	glMultMatrixf(m);
	glGetFloatv(GL_PROJECTION_MATRIX, m);

	glPopMatrix();

	Vector r1 = algVector(m[0 + 0 * 4], m[0 + 1 * 4], m[0 + 2 * 4], 1.0);
	Vector r2 = algVector(m[1 + 0 * 4], m[1 + 1 * 4], m[1 + 2 * 4], 1.0);
	Vector r3 = algVector(m[2 + 0 * 4], m[2 + 1 * 4], m[2 + 2 * 4], 1.0);
	Vector r4 = algVector(m[3 + 0 * 4], m[3 + 1 * 4], m[3 + 2 * 4], 1.0);

	frt->pNear.n = r4;
	frt->pNear.n = algAdd(frt->pNear.n, r1);
	frt->pNear.p = m[3 + 3 * 4] + m[0 + 3 * 4];

	frt->pFar.n = r4;
	frt->pFar.n = algSub(frt->pFar.n, r1);
	frt->pFar.p = m[3 + 3 * 4] - m[0 + 3 * 4];

	frt->pRight.n = r4;
	frt->pRight.n = algAdd(frt->pRight.n, r2);
	frt->pRight.p = m[3 + 3 * 4] + m[1 + 3 * 4];

	frt->pLeft.n = r4;
	frt->pLeft.n = algSub(frt->pLeft.n, r2);
	frt->pLeft.p = m[3 + 3 * 4] - m[1 * 4 + 3 * 4];

	frt->pUp.n = r4;
	frt->pUp.n = algAdd(frt->pUp.n, r3);
	frt->pUp.p = m[3 + 3 * 4] + m[2 + 3 * 4];

	frt->pDown.n = r4;
	frt->pDown.n = algSub(frt->pDown.n, r3);
	frt->pDown.p = m[3 + 3 * 4] - m[2 + 3 * 4];
}

Frustum frtCreate(Vector pos, Vector center, Vector up, float fovy, float aspect, float znear, float zfar)
{
	glPushAttrib(GL_TRANSFORM_BIT);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluPerspective(fovy, aspect, znear, zfar);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	gluLookAt(pos.x, pos.y, pos.z, center.x, center.y, center.z, up.x, up.y, up.z);

	Frustum frt = (struct _Frustum*)malloc(sizeof(struct _Frustum));

	frtBuildFrustum(frt);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glPopAttrib();

	return frt;
}

int frtContains(Frustum frt, AABB box)
{
	Plane * plane[6];

	plane[0] = &frt->pNear;
	plane[1] = &frt->pFar;
	plane[2] = &frt->pLeft;
	plane[3] = &frt->pRight;
	plane[4] = &frt->pUp;
	plane[5] = &frt->pDown;

	int result = 1;

	for (int i = 0; i < 6; ++i) 
	{
		Plane * p = plane[i];

		Vector pv = algVector(
			p->n.x > 0 ? box->pMax.x : box->pMin.x,
			p->n.y > 0 ? box->pMax.y : box->pMin.y,
			p->n.z > 0 ? box->pMax.z : box->pMin.z,
			1.0);

		Vector nv = algVector(
			p->n.x < 0 ? box->pMax.x : box->pMin.x,
			p->n.y < 0 ? box->pMax.y : box->pMin.y,
			p->n.z < 0 ? box->pMax.z : box->pMin.z,
			1.0);

		int n = planeEvaluatePoint(p, &pv);
		if (n < 0) return -1;

		int m = planeEvaluatePoint(p, &nv);
		if (m < 0) result = 0;
	}

	return result;
}

void frtPrintPlanes(Frustum frt)
{
	Plane * p = &frt->pNear;
	printf("Near:\t%+.2f %+.2f %+.2f, %+.2f\t(%s)\n", p->n.x, p->n.y, p->n.z, p->p, "");
	p = &frt->pFar;
	printf("Far:\t%+.2f %+.2f %+.2f, %+.2f\t(%s)\n", p->n.x, p->n.y, p->n.z, p->p, "");

	p = &frt->pRight;
	printf("Right:\t%+.2f %+.2f %+.2f, %+.2f\t(%s)\n", p->n.x, p->n.y, p->n.z, p->p, "");
	p = &frt->pLeft;
	printf("Left:\t%+.2f %+.2f %+.2f, %+.2f\t(%s)\n", p->n.x, p->n.y, p->n.z, p->p, "");

	p = &frt->pUp;
	printf("Up:\t%+.2f %+.2f %+.2f, %+.2f\t(%s)\n", p->n.x, p->n.y, p->n.z, p->p, "");
	p = &frt->pDown;
	printf("Down:\t%+.2f %+.2f %+.2f, %+.2f\t(%s)\n", p->n.x, p->n.y, p->n.z, p->p, "");
	printf("--------------------------------------\n");
}
