#ifndef   _ALGEBRA_H_
#define   _ALGEBRA_H_

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define real double

#ifndef BOOL
#define BOOL int
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#define NULL 0
#endif

typedef struct
{
   real x;
   real y;
   real z;
   real w;
}
Vector;


typedef struct 
{
   real w;
   real x;
   real y;
   real z;
}
Quat;

typedef struct 
{
   real m[16];
}
Matrix;


Vector algVector( real x, real y, real z, real w );

real algGetX(Vector vector);

real algGetY(Vector vector);

real algGetZ(Vector vector);

real algGetW(Vector vector);

Vector algCartesian( Vector vector );

Vector algAdd( Vector v1, Vector v2 );

Vector algScale( real scalar, Vector vector );

Vector algSub( Vector v1, Vector v2 );

Vector algMinus( Vector vector );

real algNorm( Vector vector );

Vector algUnit( Vector vector );

real algDot( Vector v1, Vector v2 );

real algDot4( Vector v1, Vector v2 );

Vector algProj( Vector ofVector, Vector ontoVector );

Vector algCross( Vector v1, Vector v2 );

Vector algReflect( Vector ofVector, Vector aroundVector );

/**
 *   Combinação linear de N vetores.
 *
 *   O primeiro parâmetro indica N, o número de vetores sendo combinados. Os parâmetros
 *   seguintes são sempre pares [real scalar, Vector vector]. O vetor resultante é a
 *   soma de todos os vetores (scalar * vector) fornecidos, e normalizado (w = 1).
 *
 *   Exemplo: para obter o vetor 2 * v1 + 4 * v2 + 8 * v3, use:
 *      vector = algLinComb( 3, 2.0, v1, 4.0, v2, 8.0, v3 );
 *
 *   @warning Sempre passe os escalares como real explicitamente (1.0 ao invés de 1).
 */
Vector algLinComb( int count, ... );

Matrix algMatrix4x4( real a11, real a12, real a13, real a14, 
                     real a21, real a22, real a23, real a24,
                     real a31, real a32, real a33, real a34,
                     real a41, real a42, real a43, real a44);

real algGetElem(Matrix m, int i, int j);
 
real * algGetOGLMatrix(Matrix *m);

Vector algTransf( Matrix matrix, Vector vector );

Matrix algMatrixIdent(void);

Matrix algMatrixTransl(real tx, real ty, real tz);

Matrix algMatrixScale(real scalex, real scaley, real scalez);

Matrix algMatrixRotate(real angle, real ex, real ey,  real ez);

Matrix algMatrixAdd( Matrix m1, Matrix m2 );

Matrix algMatrixScalarProduct( real scale, Matrix m );

Matrix algMult( Matrix m1, Matrix m2 );

real algDet( Matrix matrix );

Matrix algInv( Matrix matrix);

/**
 * Resolve o sistema Mx=b.
 */
Vector algSolve( Matrix matrix, Vector b );

Matrix algTransp( Matrix m );

Quat algQuat(real w, real x,  real y,  real z); 

Quat  algQuatAdd(Quat q1, Quat q2);

Quat  algQuatSub(Quat q1, Quat q2);

Quat  algQuatScale(real s, Quat q1);

Quat  algQuatMult(Quat q1, Quat q2);

real algQuatDot(Quat q1, Quat q2);

real algQuatNorm(Quat q1);

real algQuatEuclidianNorm(Quat q1);

Quat  algQuatUnit(Quat q1);

Quat  algQuatConj(Quat q1);

Quat  algQuatInv(Quat q1);

/**
 * SLERP (Spherical Linear Interpolation). 
 */
Quat  algQuatSLERP(Quat q1, Quat q2, real u);

/**
 *  Quaternio que representa uma rotacao de "angle" (em radianos) em torno do eixo "axis"
 *  que e' definido por um vetor que pode nao ser unitario.
 */
Quat algQuatFromRot(real angle, Vector axis);

/**
 *  Angulo de rotacao representado em um quaternio.
 */
real algQuatRotAngle(Quat q);

/**
 *  Eixo de rotacao representado em um quaternio.
 */
Vector algQuatRotAxis(Quat q);

/**
 *  Quaternio correspondente a uma matriz de rotacao.
 */
Quat algQuatFromMatrix(Matrix m);

/**
 *  Matriz de rotacao representada por um quaternio.
 */
Matrix algQuatToMatrix(Quat q);

void algVectorPrint(char *text, Vector v);

void algMatrixPrint(char *text, Matrix m);

void algQuatPrint(char *text, Quat v);

#undef real

#ifdef __cplusplus
}
#endif

#endif
