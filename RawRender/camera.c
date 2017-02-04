#include "camera.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define M_PI	3.14159265358979323846

Camera camCreate( Vector eye, Vector at, Vector up, double fovy, double nearp, double farp,
					int screenWidth, int screenHeight )
{
	Camera camera = (struct _Camera *)malloc( sizeof(struct _Camera) );

	camera->eye = eye;
	camera->at = at;
	camera->up = up;
	camera->fovy = fovy;
	camera->nearp = nearp;
	camera->farp = farp;

	camera->zAxis = algUnit( algSub( eye, at ) );
	camera->xAxis = algUnit( algCross( up, camera->zAxis ) );
	camera->yAxis = algCross( camera->zAxis, camera->xAxis );

	camResize( camera, screenWidth, screenHeight );

	return camera;
}

void camGetFarPlane( Camera camera, Vector *origin, Vector *normal, Vector *u, Vector *v )
{
	*origin = camera->farOrigin;
	*normal = camera->farNormal;
	*u = camera->farU;
	*v = camera->farV;	
}

Vector camGetEye( Camera camera )
{
	return camera->eye;
}

Vector camGetRay( Camera camera, double x, double y )
{
	Vector u = algScale( ( x / camera->screenWidth ), camera->nearU );
	Vector v = algScale( ( y / camera->screenHeight ), camera->nearV );
	Vector point = algAdd( algAdd( camera->nearOrigin, u ), v );

	return algUnit( algSub( point, camera->eye ) );
}

int camGetScreenWidth( Camera camera )
{
	return (int)camera->screenWidth;
}

int camGetScreenHeight( Camera camera )
{
	return (int)camera->screenHeight;
}


void camDestroy( Camera camera )
{
	free( camera );
}

void camSetEye( Camera camera, Vector eye )
{
	camera->eye = eye;

	camera->zAxis = algUnit( algSub( eye, camera->at ) );
	camera->xAxis = algUnit( algCross( camera->up, camera->zAxis ) );
	camera->yAxis = algCross( camera->zAxis, camera->xAxis );

	camResize( camera, ( int ) camera->screenWidth, ( int ) camera->screenHeight );
}

void camSet( Camera camera, Vector eye, Vector up, Vector at )
{
	camera->eye = eye;
	camera->at = at;
	camera->up = up;

	camera->zAxis = algUnit( algSub( eye, camera->at ) );
	camera->xAxis = algUnit( algCross( camera->up, camera->zAxis ) );
	camera->yAxis = algCross( camera->zAxis, camera->xAxis );

	camResize( camera, ( int ) camera->screenWidth, ( int ) camera->screenHeight );
}

/************************************************************************/
/* Definição das Funções Privadas                                       */
/************************************************************************/
void camResize( Camera camera, int screenWidth, int screenHeight )
{
	double sx, sy, sz;
  
	camera->screenWidth = screenWidth;
	camera->screenHeight = screenHeight;

	/* Calcula a origem do near plane */
	sz = camera->nearp;
	sy = ( sz * tan( ( M_PI * camera->fovy ) / ( 2.0 * 180.0 ) ) );
	sx = ( ( sy * screenWidth ) / screenHeight );
	camera->nearOrigin = algLinComb( 4,
								1.0, camera->eye,
								-sz, camera->zAxis,
								-sy, camera->yAxis,
								-sx, camera->xAxis );

	/* Calcula os eixos (u,v) do near plane */
	camera->nearU = algScale( ( 2 * sx ), camera->xAxis );
	camera->nearV = algScale( ( 2 * sy ), camera->yAxis );

	/* Calcula a origem do far plane */
	sz *= ( camera->farp / camera->nearp );
	sy *= ( camera->farp / camera->nearp );
	sx *= ( camera->farp / camera->nearp );
	camera->farOrigin = algLinComb( 4,
								1.0, camera->eye,
								-sz, camera->zAxis,
								-sy, camera->yAxis,
								-sx, camera->xAxis );

	/* Calcula os eixos (u,v) do far plane */
	camera->farU = algScale( ( 2 * sx ), camera->xAxis );
	camera->farV = algScale( ( 2 * sy ), camera->yAxis );
	camera->farNormal = algUnit( algCross( camera->farU, camera->farV ) );
}
