#include "light.h"
#include <string.h>
#include <stdlib.h>

Light lightCreate( Vector position, Color color )
{
	Light light = (struct _Light *)malloc( sizeof(struct _Light) );

	light->position = position;
	light->color = color;

	return light;
}

Vector lightGetPosition( Light light )
{
	return light->position;
}

Color lightGetColor( Light light )
{
	return light->color;
}

void lightDestroy( Light light )
{
	free( light );
}

void lightSetPosition( Light light, Vector position )
{
	light->position = position;
}

void lightSetColor( Light light, Color color )
{
	light->color = color;
}

