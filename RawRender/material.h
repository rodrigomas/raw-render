#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "color.h"
#include "image.h"
#include "algebra.h"

#ifdef __cplusplus
extern "C" {
#endif
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#ifdef __cplusplus
	}
#endif

struct _Material
{
	Color diffuseColor;

	Image diffuseTexture;

	Color specularColor;

	Image specularTexture;

	Image normalTexture;

	Image displacementTexture;

	Image reflectionTexture;

	Image refractionTexture;

	Image opacityTexture;

	double specularExponent;

	double reflectionFactor;

	double refractionFactor;

	double opacityFactor;

	lua_State *L;
};

typedef struct _Material * Material;


Material matCreate( Image texture, Color diffuseColor, 
					Color specularColor, double specularExponent,
					double reflectionFactor, double refractionFactor, double opacityFactor );

Color matGetDiffuse( Material material, Vector textureCoordinate );

Color matGetSpecular( Material material, Vector textureCoordinate);

Vector matGetNormal(Material material, Vector normal, Vector textureCoordinate);

Vector matGetDisplacement(Material material, Vector pos, Vector normal, Vector textureCoordinate);

double matGetSpecularExponent( Material material );

double matGetReflectionFactor( Material material, Vector textureCoordinate);

double matGetRefractionIndex( Material material, Vector textureCoordinate);

double matGetOpacity( Material material, Vector textureCoordinate);

void matDestroy( Material material );

#ifdef __cplusplus
}
#endif

#endif

