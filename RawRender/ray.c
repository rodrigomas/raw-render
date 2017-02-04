#include "ray.h"

// Construtor
Ray rayCreate(Vector p, Vector d)
{
	Ray r = (struct _Ray *)malloc(sizeof(struct _Ray));

	r->pos = p;
	r->dir = d;
}

void rayDestroy(Ray r)
{
	free(r);
}

// Classifica raio e precomputa valores necessários para comparação
// rápida com AABB.
// Adaptado do artigo de Martin Eisemann disponível em
// http://jgt.akpeters.com/papers/EisemannEtAl07/
void rayClassify(Ray r)
{
	// Inversa da Direção
	r->idir.x = 1.0f / r->dir.x;
	r->idir.y = 1.0f / r->dir.y;
	r->idir.z = 1.0f / r->dir.z;

	// Slopes projetados
	r->ibyj = r->dir.x * r->idir.y;
	r->jbyi = r->dir.y * r->idir.x;
	r->jbyk = r->dir.y * r->idir.z;
	r->kbyj = r->dir.z * r->idir.y;
	r->ibyk = r->dir.x * r->idir.z;
	r->kbyi = r->dir.z * r->idir.x;

	// Pré-Processamento
	r->c_xy = r->pos.y - r->jbyi * r->pos.x;
	r->c_xz = r->pos.z - r->kbyi * r->pos.x;
	r->c_yx = r->pos.x - r->ibyj * r->pos.y;
	r->c_yz = r->pos.z - r->kbyj * r->pos.y;
	r->c_zx = r->pos.x - r->ibyk * r->pos.z;
	r->c_zy = r->pos.y - r->jbyk * r->pos.z;

	// Classificando pela Direção
	if (r->dir.x < 0) {
		if (r->dir.y < 0) {
			if (r->dir.z < 0)
				r->classification = MMM;
			else if (r->dir.z > 0)
				r->classification = MMP;
			else
				r->classification = MMO;
		}
		else {
			if (r->dir.z < 0) {
				r->classification = MPM;
				if (r->dir.y == 0)
					r->classification = MOM;
			}
			else {
				if ((r->dir.y == 0) && (r->dir.z == 0))
					r->classification = MOO;
				else if (r->dir.z == 0)
					r->classification = MPO;
				else if (r->dir.y == 0)
					r->classification = MOP;
				else
					r->classification = MPP;
			}
		}
	}
	else {
		if (r->dir.y < 0) {
			if (r->dir.z < 0) {
				r->classification = PMM;
				if (r->dir.x == 0)
					r->classification = OMM;
			}
			else {
				if ((r->dir.x == 0) && (r->dir.x == 0))
					r->classification = OMO;
				else if (r->dir.z == 0)
					r->classification = PMO;
				else if (r->dir.x == 0)
					r->classification = OMP;
				else
					r->classification = PMP;
			}
		}
		else {
			if (r->dir.z < 0) {
				if ((r->dir.x == 0) && (r->dir.y == 0))
					r->classification = OOM;
				else if (r->dir.x == 0)
					r->classification = OPM;
				else if (r->dir.y == 0)
					r->classification = POM;
				else
					r->classification = PPM;
			}
			else {
				if (r->dir.x == 0) {
					if (r->dir.y == 0)
						r->classification = OOP;
					else if (r->dir.z == 0)
						r->classification = OPO;
					else
						r->classification = OPP;
				}
				else {
					if ((r->dir.y == 0) && (r->dir.z == 0))
						r->classification = POO;
					else if (r->dir.y == 0)
						r->classification = POP;
					else if (r->dir.z == 0)
						r->classification = PPO;
					else
						r->classification = PPP;
				}
			}
		}
	}
}

// Verifica interseção entre Raio e AABB pro comparação de slopes.
// Adaptado do artigo de Martin Eisemann disponível em
// http://jgt.akpeters.com/papers/EisemannEtAl07/
BOOL rayIntersects(Ray r, AABB b)
{
	switch (r->classification) {
	case MMM:
		if ((r->pos.x < b->pMin.x) || (r->pos.y < b->pMin.y) || (r->pos.z < b->pMin.z)
			|| (r->jbyi * b->pMin.x - b->pMax.y + r->c_xy > 0)
			|| (r->ibyj * b->pMin.y - b->pMax.x + r->c_yx > 0)
			|| (r->jbyk * b->pMin.z - b->pMax.y + r->c_zy > 0)
			|| (r->kbyj * b->pMin.y - b->pMax.z + r->c_yz > 0)
			|| (r->kbyi * b->pMin.x - b->pMax.z + r->c_xz > 0)
			|| (r->ibyk * b->pMin.z - b->pMax.x + r->c_zx > 0))
			return FALSE;
		return TRUE;

	case MMP:
		if ((r->pos.x < b->pMin.x) || (r->pos.y < b->pMin.y) || (r->pos.z > b->pMax.z)
			|| (r->jbyi * b->pMin.x - b->pMax.y + r->c_xy > 0)
			|| (r->ibyj * b->pMin.y - b->pMax.x + r->c_yx > 0)
			|| (r->jbyk * b->pMax.z - b->pMax.y + r->c_zy > 0)
			|| (r->kbyj * b->pMin.y - b->pMin.z + r->c_yz < 0)
			|| (r->kbyi * b->pMin.x - b->pMin.z + r->c_xz < 0)
			|| (r->ibyk * b->pMax.z - b->pMax.x + r->c_zx > 0))
			return FALSE;
		return TRUE;

	case MPM:
		if ((r->pos.x < b->pMin.x) || (r->pos.y > b->pMax.y) || (r->pos.z < b->pMin.z)
			|| (r->jbyi * b->pMin.x - b->pMin.y + r->c_xy < 0)
			|| (r->ibyj * b->pMax.y - b->pMax.x + r->c_yx > 0)
			|| (r->jbyk * b->pMin.z - b->pMin.y + r->c_zy < 0)
			|| (r->kbyj * b->pMax.y - b->pMax.z + r->c_yz > 0)
			|| (r->kbyi * b->pMin.x - b->pMax.z + r->c_xz > 0)
			|| (r->ibyk * b->pMin.z - b->pMax.x + r->c_zx > 0))
			return FALSE;
		return TRUE;

	case MPP:
		if ((r->pos.x < b->pMin.x) || (r->pos.y > b->pMax.y) || (r->pos.z > b->pMax.z)
			|| (r->jbyi * b->pMin.x - b->pMin.y + r->c_xy < 0)
			|| (r->ibyj * b->pMax.y - b->pMax.x + r->c_yx > 0)
			|| (r->jbyk * b->pMax.z - b->pMin.y + r->c_zy < 0)
			|| (r->kbyj * b->pMax.y - b->pMin.z + r->c_yz < 0)
			|| (r->kbyi * b->pMin.x - b->pMin.z + r->c_xz < 0)
			|| (r->ibyk * b->pMax.z - b->pMax.x + r->c_zx > 0))
			return FALSE;
		return TRUE;

	case PMM:
		if ((r->pos.x > b->pMax.x) || (r->pos.y < b->pMin.y) || (r->pos.z < b->pMin.z)
			|| (r->jbyi * b->pMax.x - b->pMax.y + r->c_xy > 0)
			|| (r->ibyj * b->pMin.y - b->pMin.x + r->c_yx < 0)
			|| (r->jbyk * b->pMin.z - b->pMax.y + r->c_zy > 0)
			|| (r->kbyj * b->pMin.y - b->pMax.z + r->c_yz > 0)
			|| (r->kbyi * b->pMax.x - b->pMax.z + r->c_xz > 0)
			|| (r->ibyk * b->pMin.z - b->pMin.x + r->c_zx < 0))
			return FALSE;
		return TRUE;

	case PMP:

		if ((r->pos.x > b->pMax.x) || (r->pos.y < b->pMin.y) || (r->pos.z > b->pMax.z)
			|| (r->jbyi * b->pMax.x - b->pMax.y + r->c_xy > 0)
			|| (r->ibyj * b->pMin.y - b->pMin.x + r->c_yx < 0)
			|| (r->jbyk * b->pMax.z - b->pMax.y + r->c_zy > 0)
			|| (r->kbyj * b->pMin.y - b->pMin.z + r->c_yz < 0)
			|| (r->kbyi * b->pMax.x - b->pMin.z + r->c_xz < 0)
			|| (r->ibyk * b->pMax.z - b->pMin.x + r->c_zx < 0))
			return FALSE;
		return TRUE;

	case PPM:
		if ((r->pos.x > b->pMax.x) || (r->pos.y > b->pMax.y) || (r->pos.z < b->pMin.z)
			|| (r->jbyi * b->pMax.x - b->pMin.y + r->c_xy < 0)
			|| (r->ibyj * b->pMax.y - b->pMin.x + r->c_yx < 0)
			|| (r->jbyk * b->pMin.z - b->pMin.y + r->c_zy < 0)
			|| (r->kbyj * b->pMax.y - b->pMax.z + r->c_yz > 0)
			|| (r->kbyi * b->pMax.x - b->pMax.z + r->c_xz > 0)
			|| (r->ibyk * b->pMin.z - b->pMin.x + r->c_zx < 0))
			return FALSE;
		return TRUE;

	case PPP:
		if ((r->pos.x > b->pMax.x) || (r->pos.y > b->pMax.y) || (r->pos.z > b->pMax.z)
			|| (r->jbyi * b->pMax.x - b->pMin.y + r->c_xy < 0)
			|| (r->ibyj * b->pMax.y - b->pMin.x + r->c_yx < 0)
			|| (r->jbyk * b->pMax.z - b->pMin.y + r->c_zy < 0)
			|| (r->kbyj * b->pMax.y - b->pMin.z + r->c_yz < 0)
			|| (r->kbyi * b->pMax.x - b->pMin.z + r->c_xz < 0)
			|| (r->ibyk * b->pMax.z - b->pMin.x + r->c_zx < 0))
			return FALSE;
		return TRUE;

	case OMM:
		if ((r->pos.x < b->pMin.x) || (r->pos.x > b->pMax.x)
			|| (r->pos.y < b->pMin.y) || (r->pos.z < b->pMin.z)
			|| (r->jbyk * b->pMin.z - b->pMax.y + r->c_zy > 0)
			|| (r->kbyj * b->pMin.y - b->pMax.z + r->c_yz > 0))
			return FALSE;
		return TRUE;

	case OMP:
		if ((r->pos.x < b->pMin.x) || (r->pos.x > b->pMax.x)
			|| (r->pos.y < b->pMin.y) || (r->pos.z > b->pMax.z)
			|| (r->jbyk * b->pMax.z - b->pMax.y + r->c_zy > 0)
			|| (r->kbyj * b->pMin.y - b->pMin.z + r->c_yz < 0))
			return FALSE;
		return TRUE;

	case OPM:
		if ((r->pos.x < b->pMin.x) || (r->pos.x > b->pMax.x)
			|| (r->pos.y > b->pMax.y) || (r->pos.z < b->pMin.z)
			|| (r->jbyk * b->pMin.z - b->pMin.y + r->c_zy < 0)
			|| (r->kbyj * b->pMax.y - b->pMax.z + r->c_yz > 0))
			return FALSE;
		return TRUE;

	case OPP:
		if ((r->pos.x < b->pMin.x) || (r->pos.x > b->pMax.x)
			|| (r->pos.y > b->pMax.y) || (r->pos.z > b->pMax.z)
			|| (r->jbyk * b->pMax.z - b->pMin.y + r->c_zy < 0)
			|| (r->kbyj * b->pMax.y - b->pMin.z + r->c_yz < 0))
			return FALSE;
		return TRUE;

	case MOM:
		if ((r->pos.y < b->pMin.y) || (r->pos.y > b->pMax.y)
			|| (r->pos.x < b->pMin.x) || (r->pos.z < b->pMin.z)
			|| (r->kbyi * b->pMin.x - b->pMax.z + r->c_xz > 0)
			|| (r->ibyk * b->pMin.z - b->pMax.x + r->c_zx > 0))
			return FALSE;
		return TRUE;

	case MOP:
		if ((r->pos.y < b->pMin.y) || (r->pos.y > b->pMax.y)
			|| (r->pos.x < b->pMin.x) || (r->pos.z > b->pMax.z)
			|| (r->kbyi * b->pMin.x - b->pMin.z + r->c_xz < 0)
			|| (r->ibyk * b->pMax.z - b->pMax.x + r->c_zx > 0))
			return FALSE;
		return TRUE;

	case POM:
		if ((r->pos.y < b->pMin.y) || (r->pos.y > b->pMax.y)
			|| (r->pos.x > b->pMax.x) || (r->pos.z < b->pMin.z)
			|| (r->kbyi * b->pMax.x - b->pMax.z + r->c_xz > 0)
			|| (r->ibyk * b->pMin.z - b->pMin.x + r->c_zx < 0))
			return FALSE;
		return TRUE;

	case POP:
		if ((r->pos.y < b->pMin.y) || (r->pos.y > b->pMax.y)
			|| (r->pos.x > b->pMax.x) || (r->pos.z > b->pMax.z)
			|| (r->kbyi * b->pMax.x - b->pMin.z + r->c_xz < 0)
			|| (r->ibyk * b->pMax.z - b->pMin.x + r->c_zx < 0))
			return FALSE;
		return TRUE;

	case MMO:
		if ((r->pos.z < b->pMin.z) || (r->pos.z > b->pMax.z)
			|| (r->pos.x < b->pMin.x) || (r->pos.y < b->pMin.y)
			|| (r->jbyi * b->pMin.x - b->pMax.y + r->c_xy > 0)
			|| (r->ibyj * b->pMin.y - b->pMax.x + r->c_yx > 0))
			return FALSE;
		return TRUE;

	case MPO:
		if ((r->pos.z < b->pMin.z) || (r->pos.z > b->pMax.z)
			|| (r->pos.x < b->pMin.x) || (r->pos.y > b->pMax.y)
			|| (r->jbyi * b->pMin.x - b->pMin.y + r->c_xy < 0)
			|| (r->ibyj * b->pMax.y - b->pMax.x + r->c_yx > 0))
			return FALSE;
		return TRUE;

	case PMO:

		if ((r->pos.z < b->pMin.z) || (r->pos.z > b->pMax.z)
			|| (r->pos.x > b->pMax.x) || (r->pos.y < b->pMin.y)
			|| (r->jbyi * b->pMax.x - b->pMax.y + r->c_xy > 0)
			|| (r->ibyj * b->pMin.y - b->pMin.x + r->c_yx < 0))
			return FALSE;
		return TRUE;

	case PPO:
		if ((r->pos.z < b->pMin.z) || (r->pos.z > b->pMax.z)
			|| (r->pos.x > b->pMax.x) || (r->pos.y > b->pMax.y)
			|| (r->jbyi * b->pMax.x - b->pMin.y + r->c_xy < 0)
			|| (r->ibyj * b->pMax.y - b->pMin.x + r->c_yx < 0))
			return FALSE;
		return TRUE;

	case MOO:
		if ((r->pos.x < b->pMin.x)
			|| (r->pos.y < b->pMin.y) || (r->pos.y > b->pMax.y)
			|| (r->pos.z < b->pMin.z) || (r->pos.z > b->pMax.z))
			return FALSE;
		return TRUE;

	case POO:
		if ((r->pos.x > b->pMax.x)
			|| (r->pos.y < b->pMin.y) || (r->pos.y > b->pMax.y)
			|| (r->pos.z < b->pMin.z) || (r->pos.z > b->pMax.z))
			return FALSE;
		return TRUE;

	case OMO:
		if ((r->pos.y < b->pMin.y)
			|| (r->pos.x < b->pMin.x) || (r->pos.x > b->pMax.x)
			|| (r->pos.z < b->pMin.z) || (r->pos.z > b->pMax.z))
			return FALSE;

	case OPO:
		if ((r->pos.y > b->pMax.y)
			|| (r->pos.x < b->pMin.x) || (r->pos.x > b->pMax.x)
			|| (r->pos.z < b->pMin.z) || (r->pos.z > b->pMax.z))
			return FALSE;

	case OOM:
		if ((r->pos.z < b->pMin.z)
			|| (r->pos.x < b->pMin.x) || (r->pos.x > b->pMax.x)
			|| (r->pos.y < b->pMin.y) || (r->pos.y > b->pMax.y))
			return FALSE;

	case OOP:
		if ((r->pos.z > b->pMax.z)
			|| (r->pos.x < b->pMin.x) || (r->pos.x > b->pMax.x)
			|| (r->pos.y < b->pMin.y) || (r->pos.y > b->pMax.y))
			return FALSE;
		return TRUE;
	}

	return FALSE;
}

// Testa se dois raios são iguais comparando direção e posição
// componente a componente
BOOL rayEquals(Ray r, Ray d)
{
	if (r->dir.x != d->dir.x || r->dir.y != d->dir.y || r->dir.z != d->dir.z)
		return FALSE;

	if (r->dir.x != d->dir.x || r->dir.y != d->dir.y || r->dir.z != d->dir.z)
		return FALSE;

	return TRUE;
}