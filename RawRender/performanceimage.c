#include "performanceimage.h"

void piDestroy(PerfomanceImage pi)
{
	free(pi->data);
	if (pi->velocity != NULL)
	{
		free(pi->velocity);
	}
	free(pi);
}

PerfomanceImage piCreate(int width, int height, PerfomanceImageData defaultValue, int velocity)
{
	PerfomanceImage pi;	

	pi = (struct _PerfomanceImage_*)malloc(sizeof(struct _PerfomanceImage_));

	pi->data = (PerfomanceImageData*)malloc(width * height * sizeof(PerfomanceImageData*));
	pi->width = width;
	pi->height = height;
	pi->length = width * height;

	if (velocity == 1)
	{
		pi->velocity = (Vector*)malloc(pi->width * pi->height * sizeof(Vector));
		memset(pi->velocity, 0, pi->width * pi->height * sizeof(Vector));
	}
	else
	{
		pi->velocity = NULL;
	}

	for (register int i = 0; i < pi->length; i++)
	{
		memcpy(&pi->data[i], &defaultValue, sizeof(PerfomanceImageData));
	}

	return pi;
}

PerfomanceImage piLoad(char* filename)
{
	PerfomanceImage pi = NULL;

	int w, h;

	FILE *fp = fopen(filename, "rb");

	if (fp == NULL)
		return NULL;

	fread(&w, sizeof(int), 1, fp);
	fread(&h, sizeof(int), 1, fp);

	if (w <= 0 || h <= 0)
	{
		fclose(fp);
		return NULL;
	}

	PerfomanceImageData d;

	memset(&d, 0, sizeof(PerfomanceImageData));

	pi = piCreate(w, h, d, 0);

	fread(pi->data, sizeof(PerfomanceImageData), pi->length, fp);

	if (!feof(fp))
	{
		pi->velocity = (Vector*)malloc(pi->width * pi->height * sizeof(Vector));
		fread(pi->velocity, sizeof(Vector), pi->length, fp);
	}

	fclose(fp);

	return pi;
}

void piSave(PerfomanceImage img, char* filename)
{	
	if (img == NULL)
		return;

	FILE *fp = fopen(filename, "wb");

	if (fp == NULL)
		return NULL;

	fwrite(&img->width, sizeof(int), 1, fp);
	fwrite(&img->height, sizeof(int), 1, fp);

	fwrite(img->data, sizeof(PerfomanceImageData), img->length, fp);

	if (img->velocity != NULL)
	{
		fwrite(img->velocity, sizeof(Vector), img->length, fp);
	}

	fclose(fp);
}

PerfomanceImageData *piGetData(PerfomanceImage img, int x, int y)
{
	return &img->data[img->width * y + x];
}

void piSetData(PerfomanceImage img, int x, int y, PerfomanceImageData *value)
{
	memcpy(&img->data[img->width * y + x], value, sizeof(PerfomanceImageData));
}

void piSetDataV(PerfomanceImage img, int x, int y, Vector *v)
{
	img->velocity[img->width * y + x] = *v;
}

Vector piGetDataV(PerfomanceImage img, int x, int y)
{
	return img->velocity[img->width * y + x];
}

PerfomanceImage piEstimateFromSequency(PerfomanceImage *pArray, int pCnt)
{
	int hasVelocity = 1;
	PerfomanceImage pi = NULL;
	PerfomanceImage pib = NULL;

	PerfomanceImageData d;
	d.CpuTime = 0;
	d.RayCnt = 0;	

	for (register int i = 0; i < pCnt; i++)
	{
		if (pArray[i] != NULL)
		{
			pib = pArray[i];

			if (pArray[i]->velocity == NULL)
			{
				hasVelocity = 0;
			}			
		}
	}

	if (hasVelocity == 1)
	{

		pi = piCreate(pib->width, pib->height, d, 1);

		for (register int i = 0; i < pi->length; i++)
		{
			double cnt = 0;
			for (register int j = 0; j < pCnt; j++)
			{

			}
			//pi->velocity[i]
			//pi->data[i]
		}

		return pi;
	}
	else
	{
		/// Estimate
		return NULL;
	}
}