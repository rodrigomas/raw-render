#include "object.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MIN( a, b ) ( ( a < b ) ? a : b )

#ifndef EPSILON
#define EPSILON	1.0e-3
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Object objCreateSphere( int material, const Vector center, double radius )
{
	Object object;
	Sphere *sphere;

	object = (struct _Object *)malloc( sizeof(struct _Object) );
	sphere = (Sphere *)malloc( sizeof(Sphere) );

	sphere->center = center;
	sphere->radius = radius;

	object->type = TYPE_SPHERE;
	object->material = material;
	object->data = sphere;

	object->boundingBox = aabbCreate(algVector(center.x - 2.0 * radius * sqrt(2.0), center.y - 2.0 * radius * sqrt(2.0), center.z - 2.0 * radius * sqrt(2.0), 1.0),
									 algVector(center.x + 2.0 * radius * sqrt(2.0), center.y + 2.0 * radius * sqrt(2.0), center.z + 2.0 * radius * sqrt(2.0), 1.0));

	return object;
}

Object objCreateTriangleNormal(int material, const Vector v0, const Vector v1, const Vector v2,
	const Vector tex0, const Vector tex1, const Vector tex2,
	const Vector n0, const Vector n1, const Vector n2)
{
	Object object;
	Triangle *triangle;

	object = (struct _Object *)malloc(sizeof(struct _Object));
	triangle = (Triangle *)malloc(sizeof(Triangle));

	triangle->v0 = v0;
	triangle->v1 = v1;
	triangle->v2 = v2;

	triangle->tex0 = tex0;
	triangle->tex1 = tex1;
	triangle->tex2 = tex2;

	triangle->n0 = n0;
	triangle->n1 = n1;
	triangle->n2 = n2;

	object->type = TYPE_TRIANGLE;
	object->material = material;
	object->data = triangle;

	Vector vmin;
	Vector vmax;

	vmin.x = min(v0.x, min(v1.x, v2.x));
	vmin.y = min(v0.y, min(v1.y, v2.y));
	vmin.z = min(v0.z, min(v1.z, v2.z));

	vmax.x = max(v0.x, max(v1.x, v2.x));
	vmax.y = max(v0.y, max(v1.y, v2.y));
	vmax.z = max(v0.z, max(v1.z, v2.z));


	object->boundingBox = aabbCreate(vmin, vmax);

	return object;
}

Object objCreateTriangle( int material, const Vector v0, const Vector v1, const Vector v2, 
						                const Vector tex0, const Vector tex1, const Vector tex2)
{
	Object object;
	Triangle *triangle;

	object = (struct _Object *)malloc( sizeof(struct _Object) );
	triangle = (Triangle *)malloc( sizeof(Triangle) );

	triangle->v0 = v0;
	triangle->v1 = v1;
	triangle->v2 = v2;

	triangle->tex0 = tex0;
	triangle->tex1 = tex1;
	triangle->tex2 = tex2;

	object->type = TYPE_TRIANGLE;
	object->material = material;
	object->data = triangle;

	Vector vmin;
	Vector vmax;

	vmin.x = min(v0.x, min(v1.x, v2.x));
	vmin.y = min(v0.y, min(v1.y, v2.y));
	vmin.z = min(v0.z, min(v1.z, v2.z));

	vmax.x = max(v0.x, max(v1.x, v2.x));
	vmax.y = max(v0.y, max(v1.y, v2.y));
	vmax.z = max(v0.z, max(v1.z, v2.z));


	object->boundingBox = aabbCreate(vmin,vmax);

	return object;
}


Object objCreateBox( int material, const Vector bottomLeft, const Vector topRight )
{
	Object object;
	Box *box;

	object = (struct _Object *)malloc( sizeof(struct _Object) );
	box = (Box *)malloc( sizeof(Box) );

	box->bottomLeft = bottomLeft;
	box->topRight = topRight;

	object->type = TYPE_BOX;
	object->material = material;
	object->data = box;

	object->boundingBox = aabbCreate(bottomLeft,topRight);


	return object;
}

AABB objGetBoundingBox(Object object)
{
	return object->boundingBox;
}

double objIntercept( Object object, Vector eye, Vector ray )
{
	switch( object->type )
	{
	case TYPE_SPHERE:
		{
			Sphere *s = (Sphere *)object->data;

			double a, b, c, delta;
			double distance = -1.0;

			Vector fromSphereToEye;

			fromSphereToEye = algSub( eye, s->center );

			a = algDot( ray, ray );
			b = ( 2.0 * algDot( ray, fromSphereToEye ) );
			c = ( algDot( fromSphereToEye, fromSphereToEye ) - ( s->radius * s->radius ) );

			delta = ( ( b * b ) - ( 4 * a * c ) );

			if( fabs( delta ) <= EPSILON )
			{
				distance = ( -b / (2 * a ) );
			}
			else if( delta > EPSILON )
			{
				double root = sqrt( delta );
				distance = MIN( ( ( -b + root ) / ( 2 * a ) ), ( ( -b - root ) / ( 2.0 * a ) )  );
			}

			return distance;
		}

	case TYPE_TRIANGLE:
		{
			Triangle *t = (Triangle *)object->data;


			double dividend, divisor;
			double distance = -1.0;

			Vector v0ToV1 = algSub( t->v1, t->v0 );
			Vector v1ToV2 = algSub( t->v2, t->v1 );
			Vector normal = algCross( v0ToV1, v1ToV2 );
			Vector eyeToV0 = algSub( t->v0, eye );

			dividend = algDot( eyeToV0, normal );
			divisor = algDot( ray, normal );

			if( divisor <= -EPSILON )
			{
				distance = ( dividend / divisor );
			}

			if( distance >= 1.0 )
			{
				double a0, a1, a2;

				Vector v2ToV0 = algSub( t->v0, t->v2 );
				Vector p = algAdd( eye, algScale( distance, ray ) );
				Vector n0 = algCross( v0ToV1, algSub( p, t->v0 ) );
				Vector n1 = algCross( v1ToV2, algSub( p, t->v1 ) );
				Vector n2 = algCross( v2ToV0, algSub( p, t->v2 ) );

				normal = algUnit(normal);
				a0 = ( 0.5 * algDot( normal, n0 ) );
				a1 = ( 0.5 * algDot( normal, n1 ) );
				a2 = ( 0.5 * algDot( normal, n2 ) );

				if ( (a0>0) && (a1>0) && (a2>0) ) 
					return distance;
				else
					return -1.0;
			}

			return distance;
		}

	case TYPE_BOX:
		{
			Box *box = (Box *)object->data;

			double xmin = box->bottomLeft.x;
		    double ymin = box->bottomLeft.y;
		    double zmin = box->bottomLeft.z;
		    double xmax = box->topRight.x;
		    double ymax = box->topRight.y;
		    double zmax = box->topRight.z;
			
			double x, y, z;
			double distance = -1.0;

			if( ray.x > EPSILON || -ray.x > EPSILON )
			{
				if( ray.x > 0 )
				{
					x = xmin;
					distance = ( ( xmin - eye.x ) / ray.x );
				}
				else
				{
					x = xmax;
					distance = ( ( xmax - eye.x ) / ray.x );
				}

				if( distance > EPSILON )
				{
					y = ( eye.y + ( distance * ray.y ) ); 
					z = ( eye.z + ( distance * ray.z ) ); 
                    if( ( y >= ymin ) && ( y <= ymax ) && ( z >= zmin ) && ( z <= zmax ) )
                        return distance;
				}
			}

			if( ray.y > EPSILON || -ray.y > EPSILON )
			{
				if( ray.y > 0 )
				{
					y = ymin;
					distance = ( ( ymin - eye.y ) / ray.y );
				}
				else
				{
					y = ymax;
					distance = ( ( ymax - eye.y ) / ray.y );
				}
				
				if( distance > EPSILON )
				{
					x = ( eye.x + ( distance * ray.x ) ); 
					z = ( eye.z + ( distance * ray.z ) ); 
				    if( ( x >= xmin ) && ( x <= xmax ) && ( z >= zmin ) && ( z <= zmax ) )
					    return distance;
				}

			}

			if( ray.z > EPSILON || -ray.z > EPSILON )
			{
				if( ray.z > 0 )
				{
					z = zmin;
					distance = ( (zmin - eye.z ) / ray.z );
				}
				else
				{
					z = zmax;
					distance = ( ( zmax - eye.z ) / ray.z );
				}

				if( distance > EPSILON )
				{
					x = ( eye.x + ( distance * ray.x ) ); 
					y = ( eye.y + ( distance * ray.y ) ); 
				    if( ( x >= xmin ) && ( x <= xmax ) && ( y >= ymin ) && ( y <= ymax ) )	
						return distance;
				}
			}

			return -1.0;
		}
	
	default:
		
		return -1.0;
	}
}


Vector objNormalAt( Object object, Vector point )
{
	if( object->type == TYPE_SPHERE )
	{
		Sphere *sphere = (Sphere *)object->data;

		return algScale( ( 1.0 / sphere->radius ),
					algSub( point, sphere->center ) );
	}
	else if ( object->type == TYPE_TRIANGLE )
	{
		Triangle *triangle = (Triangle *)object->data;

		return algCross( algSub( triangle->v1, triangle->v0 ),
					algSub( triangle->v2, triangle->v0 ) );
	}
	else if ( object->type == TYPE_BOX )
	{
		Box *box = (Box *)object->data;

		if( fabs( point.x - box->bottomLeft.x ) < EPSILON )
		{
			return algVector( -1, 0, 0, 1  );
		}
		else if( fabs( point.x - box->topRight.x ) < EPSILON )
		{
			return algVector( 1, 0, 0, 1 );
		}
		else if( fabs( point.y - box->bottomLeft.y ) < EPSILON )
		{
			return algVector( 0, -1, 0, 1 );
		}
		else if( fabs( point.y - box->topRight.y ) < EPSILON )
		{
			return algVector( 0, 1, 0, 1 );
		}
		else if( fabs( point.z - box->bottomLeft.z ) < EPSILON )
		{
			return algVector( 0, 0, -1, 1 );
		}
		else if( fabs( point.z - box->topRight.z ) < EPSILON )
		{
			return algVector( 0, 0, 1, 1 );
		}
		else
		{
			return algVector( 0, 0, 0, 1 );
		}
	} 
	else
	{
	
		return algVector( 0, 0, 0, 1 );
	}
}

Vector objTextureCoordinateAt( Object object, Vector point )
{
	if( object->type == TYPE_SPHERE )
	{
		Vector normal = objNormalAt( object, point );
		double phi = atan2( normal.y, normal.x );
		double theta = atan2( sqrt( ( normal.x * normal.x ) + ( normal.y * normal.y ) ), normal.z );

		return algVector( ( 3 * ( 1 + phi / M_PI ) ), ( 3 * ( 1 - theta / M_PI ) ), 0, 1 );
	} 
	else if( object->type == TYPE_TRIANGLE )
	{
		Triangle *triangle = (Triangle *)object->data;
		Vector t;
		double d0 = algNorm(algSub(point, triangle->v0));
		double d1 = algNorm(algSub(point, triangle->v1));
		double d2 = algNorm(algSub(point, triangle->v2));

		if ((d0 + d1 + d2) > 0)
		{
			t = algAdd(algScale(d0, triangle->tex0), algAdd(algScale(d1, triangle->tex1), algScale(d2, triangle->tex2)));
			t = algScale(1.0 / (d0 + d1 + d2), t);
		}
		else
		{
			t = triangle->tex0;
		}

		return t;
	} 
	else if( object->type == TYPE_BOX )
	{
		Box *box = (Box *)object->data;

		double xmin = box->bottomLeft.x;
		double ymin = box->bottomLeft.y;
		double zmin = box->bottomLeft.z;
		double xmax = box->topRight.x;
		double ymax = box->topRight.y;
		double zmax = box->topRight.z;

		if( ( fabs( point.x - xmin ) < EPSILON ) || ( fabs( point.x - xmax ) < EPSILON ) )
		{
			return algVector( ( ( point.y - ymin ) / ( ymax - ymin ) ), ( ( point.z - zmin ) / ( zmax - zmin ) ), 0, 1 );
		}
		else if( ( fabs( point.y - ymin ) < EPSILON ) || ( fabs( point.y - ymax ) < EPSILON ) )
		{
			return algVector( ( ( point.z - zmin ) / ( zmax - zmin ) ), ( ( point.x - xmin ) / ( xmax - xmin ) ), 0, 1 );
		}
		else if( ( fabs( point.z - zmin ) < EPSILON ) || ( fabs( point.z - zmax ) < EPSILON ) )
		{
			return algVector( ( ( point.x - xmin ) / ( xmax - xmin ) ), ( ( point.y - ymin ) / ( ymax - ymin ) ), 0, 1 );
		}
	} 


	return algVector( 0, 0, 0, 1 );	
}

int objGetMaterial( Object object )
{
	return object->material;
}

void objDestroy( Object object )
{
	aabbDestroy(object->boundingBox);
	free( object );
}

