#include <math.h>
#include <float.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/timeb.h>

#include "raytracing.h"
#include "color.h"
#include "algebra.h"


#define MAX( a, b ) ( ( (a) > (b) ) ? (a) : (b) )
#define MIN( a, b ) ( ( (a) < (b) ) ? (a) : (b) )

const int MAX_DEPTH = 10;

int curDepth = 2;


static Color shade( Scene scene, Vector eye, Vector ray, Object object, Vector point,
					Vector normal, int depth, int x, int y, int TopLevel);

static double getNearestObject( Scene scene, Vector eye, Vector ray, Object *object);

static int isInShadow( Scene scene, Vector point, Vector lightLocation);


/* Flags usadas para determinar quais componentes de iluminacao serao usadas */
int flagAmbiente = 1;
int flagDifusa = 1;
int flagEspecular = 1;
int flagSombra = 1;
int flagRefracao = 1;

Color rayTraceRec(Scene scene, Vector eye, Vector ray, int depth, int x, int y);


Color rayTraceRec(Scene scene, Vector eye, Vector ray, int depth, int x, int y)
{
	Object object;
	double distance;

	Vector point;
	Vector normal;

	scene->perfImage->data[y * scene->perfImage->width + x].RayCnt++;

	distance = getNearestObject(scene, eye, ray, &object);

	if (distance == DBL_MAX)
	{
		return sceGetBackgroundColor(scene, eye, ray);
	}

	point = algAdd(eye, algScale(distance, ray));

	normal = objNormalAt(object, point);

	Color c = shade(scene, eye, ray, object, point, normal, depth, x, y, 0);

	return c;
}

Color rayTrace( Scene scene, int depth, int x, int y)
{
	Object object;
	double distance;

	Vector point;
	Vector normal;	

	struct _timeb timebuffer;

	_ftime(&timebuffer);

	unsigned long int t0 = timebuffer.time * 1000 + timebuffer.millitm;
	unsigned long int t1 = t0;

	Color c = colorCreate3b(0,0,0);
	
	scene->perfImage->data[y * scene->perfImage->width + x].RayCnt = 0;

	if (scene->subSampling == 0)
		return c;


	sceSetMap(scene, MAP_COLOR, (y * scene->perfImage->width + x), 0, 0, 0, 0);
	sceSetMap(scene, MAP_DEPTH, (y * scene->perfImage->width + x), 0, 0, 0, 0);
	sceSetMap(scene, MAP_NORMAL, (y * scene->perfImage->width + x), 0, 0, 0, 0);
	sceSetMap(scene, MAP_SHADOW, (y * scene->perfImage->width + x), 0, 0, 0, 0);
	sceSetMap(scene, MAP_REFLECTION, (y * scene->perfImage->width + x), 0, 0, 0, 0);
	sceSetMap(scene, MAP_REFRACTION, (y * scene->perfImage->width + x), 0, 0, 0, 0);
	sceSetMap(scene, MAP_DIFFUSE, (y * scene->perfImage->width + x), 0, 0, 0, 0);
	sceSetMap(scene, MAP_SPECULAR, (y * scene->perfImage->width + x), 0, 0, 0, 0);
	sceSetMap(scene, MAP_GI, (y * scene->perfImage->width + x), 0, 0, 0, 0);

	double dx;
	double dy;

	Vector ray;
	Vector eye = scene->camera->eye;

	double JITTER_RADIUS_X = 1.0 / scene->camera->screenWidth;
	double JITTER_RADIUS_Y = 1.0 / scene->camera->screenHeight;

	double angle = 0;
	double maxradius = sqrt(JITTER_RADIUS_X*JITTER_RADIUS_X + JITTER_RADIUS_Y*JITTER_RADIUS_Y);

	double radius = maxradius;

	//srand(t0);

	int hits = 0;

	int nsamples = (scene->useGI == 1) ? scene->samples : 1;

	for (register int i = 0; i < scene->subSampling; i++)
	{
		scene->perfImage->data[y * scene->perfImage->width + x].RayCnt++;

		radius = maxradius * (rand() / (double)RAND_MAX);
		angle = 2 * M_PI * (rand() / (double)RAND_MAX);

		dx = radius * cos(angle);
		dy = radius * sin(angle);

		ray = camGetRay(scene->camera, x + dx, y + dy);

		distance = getNearestObject(scene, eye, ray, &object);

		if (distance == DBL_MAX)
		{
			Color cb = sceGetBackgroundColor(scene, eye, ray);
			
			c = colorAddition(c, cb);

			continue;
		}

		hits++;

		point = algAdd(eye, algScale(distance, ray));

		normal = objNormalAt(object, point);

		Color c0 = shade(scene, eye, ray, object, point, normal, depth, x, y, 1);

		c = colorAddition(c, c0);

		sceIncMap(scene, MAP_COLOR, (y * scene->perfImage->width + x), c0.red, c0.green, c0.blue, 0); sceSetMapW(scene, MAP_COLOR, (y * scene->perfImage->width + x), 1);
		sceIncMap(scene, MAP_DEPTH, (y * scene->perfImage->width + x), point.x, point.y, point.z, 0); sceSetMapW(scene, MAP_DEPTH, (y * scene->perfImage->width + x), 1);
		sceIncMap(scene, MAP_NORMAL, (y * scene->perfImage->width + x), normal.x, normal.y, normal.z, 0); sceSetMapW(scene, MAP_NORMAL, (y * scene->perfImage->width + x), 1);
	}

	if (hits > 0)
	{
		sceIncMap(scene, MAP_COLOR, (y * scene->perfImage->width + x), c.red, c.green, c.blue, 0); sceSetMapW(scene, MAP_COLOR, (y * scene->perfImage->width + x), 1);

		sceSetMapDiv(scene, MAP_COLOR, (y * scene->perfImage->width + x), hits);
		sceSetMapDiv(scene, MAP_DEPTH, (y * scene->perfImage->width + x), hits);
		sceSetMapDiv(scene, MAP_NORMAL, (y * scene->perfImage->width + x), hits);
		sceSetMapDiv(scene, MAP_SHADOW, (y * scene->perfImage->width + x), nsamples * hits);
		sceSetMapDiv(scene, MAP_REFLECTION, (y * scene->perfImage->width + x), nsamples * hits);
		sceSetMapDiv(scene, MAP_REFRACTION, (y * scene->perfImage->width + x), nsamples * hits);
		sceSetMapDiv(scene, MAP_DIFFUSE, (y * scene->perfImage->width + x), nsamples * hits);
		sceSetMapDiv(scene, MAP_SPECULAR, (y * scene->perfImage->width + x), nsamples * hits);
		sceSetMapDiv(scene, MAP_GI, (y * scene->perfImage->width + x), nsamples * hits);
	}

	_ftime(&timebuffer);

	t1 = timebuffer.time * 1000 + timebuffer.millitm;

	scene->perfImage->data[y * scene->perfImage->width + x].CpuTime = t1 - t0;

	return c;
}

static Color shade( Scene scene, Vector eye, Vector ray, Object object, Vector point,
					Vector normal, int depth, int x, int y, int TopLevel)
{
	int i;
	int numLuzes;
	Vector reverseRay, direcaoRayRefletida;
	Material material;
	double reflectionFactor, specularExponent, opacity;
	Color ambient, diffuse, specular, displacement, refract, reflect, color;	

	color = colorCreate3b(0, 0, 0);

	Vector uv = objTextureCoordinateAt(object, point);

	reverseRay = algMinus( ray );

	material = sceGetMaterial(scene,objGetMaterial(object));

	if (material->L != NULL)
	{
		return color;
	}	
	
	double angleX = 0;
	double angleY = 0;

	double radius = GI_RADIUS;

	reflectionFactor = matGetReflectionFactor( material, uv );

	specularExponent = matGetSpecularExponent( material );

	opacity = matGetOpacity(material, uv);

	ambient = sceGetAmbientLight( scene );

	diffuse = matGetDiffuse(material, uv);

	specular = matGetSpecular(material, uv);

	normal = matGetNormal(material, normal, uv);
	
	point = matGetDisplacement(material, point, normal, uv);	

	/* Reflexao de ray (r) em torno da normal (n) (usada para o calculo da componente especular) */
	/* rr = 2*(r.n)*n - r (normalizado) */	
	
	/* Começa com a cor ambiente */
	if (flagAmbiente) {
		color.red = diffuse.red * ambient.red;
		color.green = diffuse.green * ambient.green;
		color.blue = diffuse.blue * ambient.blue;

		if (TopLevel == 1)
		{
			sceIncMap(scene, MAP_DIFFUSE, (y * scene->perfImage->width + x), color.red, color.green, color.blue, 0);			
		}

	} else {
		color.red = 0.0;
		color.green = 0.0;
		color.blue = 0.0;
	}

	numLuzes = sceGetLightCount(scene);

	int nsamples = (scene->useGI == 1) ? scene->samples : 1;

	for (register int i = 0; i < nsamples; i++)
	{
		scene->perfImage->data[y * scene->perfImage->width + x].RayCnt++;
	
		radius = GI_RADIUS * (rand() / (double)RAND_MAX);
		angleX = 2 * M_PI * (rand() / (double)RAND_MAX);
		angleY = M_PI * (rand() / (double)RAND_MAX);

		double dx = radius * cos(angleX);
		double dz = radius * sin(angleX);
		double dy = radius * sin(angleY);

		Vector offset = algVector(dx, dy, dz, 1);

		Vector nnormal = algAdd(normal, offset);

		direcaoRayRefletida = algSub(algScale(2.0*algDot(reverseRay, nnormal), nnormal), reverseRay);

		//local light
		for (i = 0; i < numLuzes; ++i)
		{
			scene->perfImage->data[y * scene->perfImage->width + x].RayCnt++;

			Light light = sceGetLight(scene, i);
			Color lightColor = lightGetColor(light);
			Vector lightPosition = lightGetPosition(light);

			Color aux;

			Vector direcaoLuz;
			double cosTeta; 
			double cosAlfaExpN;

			/* So nao processo se a flag de sombra estiver ligada e o objeto estiver na sombra */
			if (!(flagSombra && isInShadow(scene, point, lightPosition)))
			{
				/* Direcao do raio que vem da fonte de luz (normalizada) */
				direcaoLuz = algUnit(algSub(lightPosition, point));

				if (flagDifusa) {
					/* Cosseno de teta (os dois vetores ja estao normalizados) */
					cosTeta = algDot(normal, direcaoLuz);
					/* Se teta for maior que 90, nao ha iluminacao difusa (nao faz sentido subtrair cor) */
					if (cosTeta < 0.0) {
						cosTeta = 0.0;
					}
				}
				else {
					cosTeta = 0.0;
				}

				if (flagEspecular) {
					/* Cosseno de alfa */
					double cosAlfa = algDot(direcaoRayRefletida, direcaoLuz);
					if (cosAlfa > 0.0) {
						/* Cosseno de alfa elevado a n */
						cosAlfaExpN = pow(cosAlfa, specularExponent);
					}
					else {
						cosAlfaExpN = 0.0;
					}
				}
				else {
					cosAlfaExpN = 0.0;
				}

				aux.red = lightColor.red * (diffuse.red * cosTeta + specular.red * cosAlfaExpN);
				aux.green = lightColor.green * (diffuse.green * cosTeta + specular.green * cosAlfaExpN);
				aux.blue = lightColor.blue * (diffuse.blue * cosTeta + specular.blue * cosAlfaExpN);

				color.red += aux.red;
				color.green += aux.green;
				color.blue += aux.blue;

				if (TopLevel == 1)
				{
					sceIncMap(scene, MAP_DIFFUSE, (y * scene->perfImage->width + x), lightColor.red * (diffuse.red * cosTeta), lightColor.green * (diffuse.green * cosTeta), lightColor.blue * (diffuse.blue * cosTeta), 0);
					sceSetMapW(scene, MAP_DIFFUSE, (y * scene->perfImage->width + x), 1);

					sceIncMap(scene, MAP_SPECULAR, (y * scene->perfImage->width + x), lightColor.red * (specular.red * cosAlfaExpN), lightColor.green * (specular.green * cosAlfaExpN), lightColor.blue * (specular.blue * cosAlfaExpN), 0);
					sceSetMapW(scene, MAP_SPECULAR, (y * scene->perfImage->width + x), 1);
				}
			}
			else if (TopLevel == 1)
			{
				sceIncMap(scene, MAP_SHADOW, (y * scene->perfImage->width + x), 1.0f / numLuzes, 1.0f / numLuzes, 1.0f / numLuzes, 0);
				sceSetMapW(scene, MAP_SHADOW, (y * scene->perfImage->width + x), 1);
			}
		}

		color.red *= 1.0f / numLuzes;
		color.green *= 1.0f / numLuzes;
		color.blue *= 1.0f / numLuzes;

		// GI
		//++depth;


		//////////////////////

		// Reflexao
		if (depth < curDepth && reflectionFactor > 0.0)
		{
			Color refletida = rayTraceRec(scene, point, direcaoRayRefletida, depth + 1, x, y);

			color.red += reflectionFactor * refletida.red;
			color.green += reflectionFactor * refletida.green;
			color.blue += reflectionFactor * refletida.blue;

			if (TopLevel == 1)
			{
				sceIncMap(scene, MAP_REFLECTION, (y * scene->perfImage->width + x), refletida.red * reflectionFactor, refletida.green * reflectionFactor, refletida.blue * reflectionFactor, 0);
				sceSetMapW(scene, MAP_REFLECTION, (y * scene->perfImage->width + x), 1);
			}
		}

		// Refracao
		if (depth < curDepth && opacity < 1.0 && flagRefracao)
		{
			Vector rayTang;
			Vector direcaoRayRefratada;
			Color refratada;
			double seno;
			double cosseno;
			double dotRayNormal;

			dotRayNormal = algDot(reverseRay, nnormal);
			/* rayTang = componente tangencial do raio */
			/* rt = (r.n)*n - r */
			rayTang = algSub(algScale(dotRayNormal, nnormal), reverseRay);

			if (dotRayNormal > 0.0) { // Raio entrando no objeto
				seno = 1.0 / matGetRefractionIndex(material, uv) * algNorm(rayTang);
			}
			else { // Raio saindo do objeto
				seno = matGetRefractionIndex(material, uv) / 1.0 * algNorm(rayTang);
			}
			cosseno = sqrt(1.0 - seno*seno);

			direcaoRayRefratada = algSub(algScale(seno, algUnit(rayTang)), algScale(cosseno, nnormal));

			refratada = rayTraceRec(scene, point, direcaoRayRefratada, depth + 1, x, y);

			color.red += (1.0 - opacity) * refratada.red;
			color.green += (1.0 - opacity) * refratada.green;
			color.blue += (1.0 - opacity) * refratada.blue;

			if (TopLevel == 1)
			{
				sceIncMap(scene, MAP_REFRACTION, (y * scene->perfImage->width + x), refratada.red * reflectionFactor, refratada.green * reflectionFactor, refratada.blue * reflectionFactor, 0);
				sceSetMapW(scene, MAP_REFRACTION, (y * scene->perfImage->width + x), 1);
			}
		}
	}

	color.red *= 1.0f / nsamples;
	color.green *= 1.0f / nsamples;
	color.blue *= 1.0f / nsamples;

	//////////////////////

	///* Trunca cores */
	//color.red = MIN(1.0, color.red);
	//color.green = MIN(1.0, color.green);
	//color.blue = MIN(1.0, color.blue);

	return color;
}

static double getNearestObject( Scene scene, Vector eye, Vector ray, Object *object)
{
#ifdef _NO_OCTREE_
	int i;
	int objectCount = sceGetObjectCount( scene );

	double closest = DBL_MAX;

	/* Para cada objeto na cena */
	for( i = 0; i < objectCount; ++i )
	{
		Object currentObject = sceGetObject( scene, i );
		double distance = objIntercept( currentObject, eye, ray );
		
		if( distance > EPSILON && distance < closest )
		{
			closest = distance;
			*object = currentObject;
		}
	}

	return closest;
#else	
	Ray r = rayCreate(eye, ray);
	Vector normal;

	double closest = DBL_MAX;

	Object currentObject = octreeGetFirstIntersection(scene->octree, r, &closest, &normal, NULL);

	object = currentObject;

	return closest;
#endif
}

static int isInShadow( Scene scene, Vector point, Vector lightLocation)
{
#ifdef _NO_OCTREE_
	int i;
	int objectCount = sceGetObjectCount( scene );
	Vector rayToLight = algSub(lightLocation, point);

	/* maxDistance = distância de point até lightLocation */
	double maxDistance = algNorm(rayToLight);

	rayToLight = algUnit(rayToLight);

	/* Para cada objeto na cena */
	for( i = 0; i < objectCount; ++i )
	{
		double distance = objIntercept( sceGetObject( scene, i ), point, rayToLight );
		
		if( distance > 0.0 && distance < maxDistance )
		{
			return 1;
		}
	}

	return 0;
#else
	Vector rayToLight = algSub(lightLocation, point);	
	Vector normal;

	double maxDistance = algNorm(rayToLight);

	rayToLight = algUnit(rayToLight);

	double distance = DBL_MAX;

	Ray r = rayCreate(point, rayToLight);

	Object currentObject = octreeGetFirstIntersection(scene->octree, r, &distance, &normal, NULL);

	if (distance > 0.0 && distance < maxDistance)
	{
		return 1;
	}

	return 0;
#endif
}

