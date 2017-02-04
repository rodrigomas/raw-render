#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "color.h"
#include "algebra.h"

#ifdef __cplusplus
extern "C" {
#endif

struct _Light
{
	Vector position;
	Color color;
};

typedef struct _Light * Light;

Light lightCreate( Vector position, Color color );

Vector lightGetPosition( Light light );

Color lightGetColor( Light light );

void lightDestroy( Light light );

void lightSetPosition( Light light, Vector position );

void lightSetColor( Light light, Color color );

#ifdef __cplusplus
}
#endif

#endif

