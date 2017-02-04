#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "algebra.h"

#ifdef __cplusplus
extern "C" {
#endif

struct _Camera
{
	Vector eye;

	Vector at;

	Vector up;

	double fovy;

	double nearp;

	double farp;

	double screenWidth;

	double screenHeight;

	/* Estado interno */

	Vector xAxis;
	Vector yAxis;
	Vector zAxis;

	Vector nearOrigin;
	Vector nearU;
	Vector nearV;

	Vector farOrigin;
	Vector farNormal;
	Vector farU;
	Vector farV;
};

typedef struct _Camera * Camera;

Camera camCreate( Vector eye, Vector at, Vector up, double fovy, double nearp, double farp,
					int screenWidth, int screenHeight );

void camGetFarPlane( Camera camera, Vector *origin, Vector *normal, Vector *u, Vector *v );

Vector camGetEye( Camera camera );

Vector camGetRay( Camera camera, double x, double y );

int camGetScreenWidth( Camera camera );

int camGetScreenHeight( Camera camera );

void camDestroy( Camera camera );

void camResize(Camera camera, int screenWidth, int screenHeight);

void camSetEye( Camera camera, Vector eye );

void camSet( Camera camera, Vector eye, Vector up, Vector at );

#ifdef __cplusplus
}
#endif

#endif

