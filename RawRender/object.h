#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "color.h"
#include "algebra.h"
#include "material.h"
#include "aabb.h"

#ifdef __cplusplus
extern "C" {
#endif

enum
{
	TYPE_UNKNOWN,
	TYPE_SPHERE,
	TYPE_TRIANGLE,
	TYPE_BOX
};

struct _Object
{

	int type;

	int material;

	void *data;

	AABB boundingBox;
};

typedef struct
{
	
	Vector center;
	  
	double radius;
}
Sphere;

typedef struct
{	

	Vector v0;

	Vector v1;

	Vector v2;
	
	Vector tex0; 
	Vector tex1; 
	Vector tex2; 

	Vector n0; 
	Vector n1; 
	Vector n2; 
}
Triangle;

typedef struct
{	
	Vector bottomLeft;	

	Vector topRight;
}
Box;

typedef struct _Object * Object;


Object objCreateSphere( int material, const Vector center, double radius );

Object objCreateTriangle( int material, const Vector v0, const Vector v1, const Vector v2, 
						                const Vector tex0, const Vector tex1, const Vector tex2 );

Object objCreateTriangleNormal(int material, const Vector v0, const Vector v1, const Vector v2,
									   const Vector tex0, const Vector tex1, const Vector tex2,
									   const Vector n0, const Vector n1, const Vector n2);

Object objCreateBox( int material, const Vector bottomLeft, const Vector topRight );

double objIntercept( Object object, Vector eye, Vector ray );

Vector objNormalAt( Object object, Vector point );

Vector objTextureCoordinateAt( Object object, Vector point );

int objGetMaterial( Object object );

AABB objGetBoundingBox(Object object);

void objDestroy( Object object );

#ifdef __cplusplus
}
#endif

#endif
