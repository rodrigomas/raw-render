#ifndef _RAY_H_
#define _RAY_H_

#include "algebra.h"
#include "aabb.h"

#ifdef __cplusplus
extern "C" {
#endif

enum Classification {
	MMM, MMP, MPM, MPP,
	PMM, PMP, PPM, PPP,
	POO, MOO, OPO, OMO,
	OOP, OOM, OMM, OMP,
	OPM, OPP, MOM, MOP,
	POM, POP, MMO, MPO,
	PMO, PPO
};

struct _Ray
{
	// Para c�lculo de interse��o com AABB
	Vector idir;

	// Classifica��o da dire��o do raio
	// P = Positivo (Plus)
	// O = Zero
	// M = Negativo (Minus)
	int classification;

	// Para m�todo r�pido de teste de AABB
	// Inclina��o do Raio Projetado
	float ibyj, jbyi, kbyj, jbyk, ibyk, kbyi;

	// Valores pr�-computado em cima das inclina��es
	float c_xy, c_xz, c_yx, c_yz, c_zx, c_zy;

	// Identificador
	int id;

	// Posi��o de Origem
	Vector pos;

	// Dire��o
	Vector dir;

};

typedef struct _Ray * Ray;

// Construtor
Ray rayCreate(Vector p, Vector d);

// Classifica raio e precomputa valores necess�rios para compara��o
// r�pida com AABB.
// Adaptado do artigo de Martin Eisemann dispon�vel em
// http://jgt.akpeters.com/papers/EisemannEtAl07/
void rayClassify(Ray r);

// Verifica interse��o entre Raio e AABB pro compara��o de slopes.
// Adaptado do artigo de Martin Eisemann dispon�vel em
// http://jgt.akpeters.com/papers/EisemannEtAl07/
BOOL rayIntersects(Ray r, AABB b);

// Testa se dois raios s�o iguais comparando dire��o e posi��o
// componente a componente
BOOL rayEquals(Ray r, Ray d);

#ifdef __cplusplus
}
#endif

#endif