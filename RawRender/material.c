#include "material.h"
#include <string.h>
#include <stdlib.h>


Material matCreate( Image texture, Color diffuseColor, 
					Color specularColor, double specularExponent,
					double reflectionFactor, double refractionFactor, double opacityFactor )
{
	Material material = (struct _Material *)malloc( sizeof(struct _Material) );

	memset(material, 0, sizeof(struct _Material));
	
	material->diffuseColor = diffuseColor;
	material->specularColor = specularColor;
	material->specularExponent = specularExponent;
	material->reflectionFactor = reflectionFactor;
	material->refractionFactor = refractionFactor;
	material->opacityFactor = opacityFactor;
	
	//material->L = lua_open();   /* opens Lua */
	//luaL_openlibs(material->L);
	//luaL_loadbuffer
	//lua_pcall

	return material;
}

Color matGetDiffuse( Material material, Vector textureCoordinate )
{
	int x;
	int y;
	int width;
	int height;

	if( material->diffuseTexture == NULL )
	{
		return material->diffuseColor;
	}

	imageGetDimensions( material->diffuseTexture, &width, &height );
	
	x = ( (int)( textureCoordinate.x * ( width - 1 ) ) % width );
	y = ( (int)( textureCoordinate.y * ( height - 1 ) ) % height );

	return imageGetPixel( material->diffuseTexture, x, y );
}

Color matGetSpecular( Material material, Vector textureCoordinate)
{
	int x;
	int y;
	int width;
	int height;

	if (material->specularTexture == NULL)
	{
		return material->specularColor;
	}

	imageGetDimensions(material->specularTexture, &width, &height);

	x = ((int)(textureCoordinate.x * (width - 1)) % width);
	y = ((int)(textureCoordinate.y * (height - 1)) % height);

	return imageGetPixel(material->specularTexture, x, y);
}

Vector matGetNormal(Material material, Vector normal, Vector textureCoordinate)
{
	int x;
	int y;
	int width;
	int height;

	if (material->normalTexture == NULL)
	{
		return normal;
	}

	imageGetDimensions(material->normalTexture, &width, &height);

	x = ((int)(textureCoordinate.x * (width - 1)) % width);
	y = ((int)(textureCoordinate.y * (height - 1)) % height);

	Color c = imageGetPixel(material->normalTexture, x, y);

	Vector nnormal = algVector(c.red, c.green, c.blue, 1);

	Vector nx = algCross(normal, algVector(1, 0, 0, 1));
	Vector nz = algCross(normal, nx);

	Matrix m = algMatrix4x4(nx.x, nx.y, nx.z, 0, normal.x, normal.y, normal.z, 0, nz.x, nz.y, nz.z, 0, 0, 0, 0, 1);

	nnormal = algTransf(m, nnormal);

	return nnormal;
}

Vector matGetDisplacement(Material material, Vector pos, Vector normal, Vector textureCoordinate)
{
	int x;
	int y;
	int width;
	int height;

	if (material->displacementTexture == NULL)
	{
		return pos;
	}

	imageGetDimensions(material->displacementTexture, &width, &height);

	x = ((int)(textureCoordinate.x * (width - 1)) % width);
	y = ((int)(textureCoordinate.y * (height - 1)) % height);

	Color c = imageGetPixel(material->displacementTexture, x, y);

	Vector ppos = algAdd(pos, algVector(c.red, c.green, c.blue, 1));

	return ppos;
}

double matGetSpecularExponent( Material material )
{
    return material->specularExponent;
}

double matGetReflectionFactor( Material material, Vector textureCoordinate)
{
	int x;
	int y;
	int width;
	int height;

	if (material->reflectionTexture == NULL)
	{
		return material->reflectionFactor;
	}

	imageGetDimensions(material->reflectionTexture, &width, &height);

	x = ((int)(textureCoordinate.x * (width - 1)) % width);
	y = ((int)(textureCoordinate.y * (height - 1)) % height);

	Color c = imageGetPixel(material->reflectionTexture, x, y);

	double v = 0.21 * c.red + 0.72 * c.green + 0.07 * c.blue;

    return v;
}

double matGetRefractionIndex( Material material, Vector textureCoordinate)
{
	int x;
	int y;
	int width;
	int height;

	if (material->refractionTexture == NULL)
	{
		return material->refractionFactor;
	}

	imageGetDimensions(material->refractionTexture, &width, &height);

	x = ((int)(textureCoordinate.x * (width - 1)) % width);
	y = ((int)(textureCoordinate.y * (height - 1)) % height);

	Color c = imageGetPixel(material->refractionTexture, x, y);

	double v = 0.21 * c.red + 0.72 * c.green + 0.07 * c.blue;

	return v;
}

double matGetOpacity( Material material, Vector textureCoordinate)
{
	return material->opacityFactor;
}

void matDestroy( Material material )
{
	
	if (material->diffuseTexture != NULL)
	{
		imageDestroy(material->diffuseTexture);
	}

	if (material->specularTexture != NULL)
	{
		imageDestroy(material->specularTexture);
	}

	if (material->normalTexture != NULL)
	{
		imageDestroy(material->normalTexture);
	}

	if (material->displacementTexture != NULL)
	{
		imageDestroy(material->displacementTexture);
	}

	if (material->reflectionTexture != NULL)
	{
		imageDestroy(material->reflectionTexture);
	}

	if (material->refractionTexture != NULL)
	{
		imageDestroy(material->refractionTexture);
	}

	if (material->normalTexture != NULL)
	{
		imageDestroy(material->normalTexture);
	}

	if (material->opacityTexture != NULL)
	{
		imageDestroy(material->opacityTexture);
	}

	free( material );
}

