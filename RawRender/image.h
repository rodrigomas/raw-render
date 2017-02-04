#ifndef IMAGE_H
#define IMAGE_H

#include "color.h"

#ifdef __cplusplus
extern "C" {
#endif

struct Image_imp 
{
  unsigned short width;
  unsigned short height; 
  unsigned char  *buf;              
};  

typedef struct Image_imp * Image;

Image   imageCreate (int w, int h);

void    imageDestroy (Image image);

void imageGetDimensions(Image image, int *w, int *h);

void imageSetPixel(Image image, int x, int y, Color color);

Color imageGetPixel(Image image, int x, int y);

Image imageResize(Image img0, int w1, int h1);

Image imageLoad(char *filename);

int imageWriteTGA(char *filename, Image image);

#ifdef __cplusplus
}
#endif

#endif
