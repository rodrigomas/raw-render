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
	// Para cálculo de interseção com AABB
	Vector idir;

	// Classificação da direção do raio
	// P = Positivo (Plus)
	// O = Zero
	// M = Negativo (Minus)
	int classification;

	// Para método rápido de teste de AABB
	// Inclinação do Raio Projetado
	float ibyj, jbyi, kbyj, jbyk, ibyk, kbyi;

	// Valores pré-computado em cima das inclinações
	float c_xy, c_xz, c_yx, c_yz, c_zx, c_zy;

	// Identificador
	int id;

	// Posição de Origem
	Vector pos;

	// Direção
	Vector dir;

};

typedef struct _Ray * Ray;

// Construtor
Ray rayCreate(Vector p, Vector d);

// Classifica raio e precomputa valores necessários para comparação
// rápida com AABB.
// Adaptado do artigo de Martin Eisemann disponível em
// http://jgt.akpeters.com/papers/EisemannEtAl07/
void rayClassify(Ray r);

// Verifica interseção entre Raio e AABB pro comparação de slopes.
// Adaptado do artigo de Martin Eisemann disponível em
// http://jgt.akpeters.com/papers/EisemannEtAl07/
BOOL rayIntersects(Ray r, AABB b);

// Testa se dois raios são iguais comparando direção e posição
// componente a componente
BOOL rayEquals(Ray r, Ray d);

#ifdef __cplusplus
}
#endif

#endif