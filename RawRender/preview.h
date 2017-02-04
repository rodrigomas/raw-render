#ifndef _TRAB4GL_H_
#define _TRAB4GL_H_

#include "scene.h"

extern Scene scene;         /* cena corrente */
extern Camera camera;

void initCamera( ) ;

void renderScene() ;

void pressKey(int key, int x, int y) ;

void releaseKey(int key, int x, int y) ;

void setInitialCameraArgs(Vector up, Vector at, Vector eye);

#endif
