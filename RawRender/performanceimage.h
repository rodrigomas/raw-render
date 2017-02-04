#ifndef _PERFORMANCE_IMAGE_H_
#define _PERFORMANCE_IMAGE_H_

#include <stdio.h>
#include "algebra.h"

struct _PerfomanceImageData_
{
	int RayCnt;
	int CpuTime;
};

typedef struct _PerfomanceImageData_ PerfomanceImageData;

struct _PerfomanceImage_
{
	int width;
	int height;
	int length;
	PerfomanceImageData* data;
	Vector* velocity;
};

typedef struct _PerfomanceImage_* PerfomanceImage;

PerfomanceImage piCreate(int width, int height, PerfomanceImageData defaultValue, int velocity);

PerfomanceImage piLoad(char* filename);

void piSave(PerfomanceImage img, char* filename);

PerfomanceImageData *piGetData(PerfomanceImage img, int x, int y);

Vector piGetDataV(PerfomanceImage img, int x, int y);

void piSetData(PerfomanceImage img, int x, int y, PerfomanceImageData *value);

void piSetDataV(PerfomanceImage img, int x, int y, Vector *v);

void piDestroy(PerfomanceImage pi);

PerfomanceImage piEstimateFromSequency(PerfomanceImage *pArray, int pCnt);

#endif