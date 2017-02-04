#ifndef _MAINUI_H_
#define _MAINUI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>

#ifdef _WIN32
#include <windows.h>         
#endif

#include <stdio.h>
#include <time.h>

	
#include <iup.h>                  
#include <iupgl.h>                
#include <gl/gl.h>             
#include <gl/glu.h>              
#include <gl/glut.h>              


#include <sys/timeb.h>


#include "image.h"               
#include "color.h"
#include "algebra.h"
#include "raytracing.h"
#include "logmanager.h"
#include "quadtree.h"

#define MAX( a, b ) ( ( (a) > (b) ) ? (a) : (b) )
#define MIN( a, b ) ( ( (a) < (b) ) ? (a) : (b) )

#define BLOCK_SIZE 32

struct _mainui
{
	Scene scene;

	QuadTree tree;

	int width;
	int height;

	Image image;	

	Vector eye;

	Camera camera;

	Ihandle *canvas;

	Ihandle *label; 

	Ihandle *passcombo;

	Ihandle *quadlevel;

	Ihandle *timelabel;

	Ihandle *progresscheck;

	Ihandle *progressgauge;

	Ihandle *messageinformer;

	Ihandle *dialog;

	pthread_t idx_renderthread;

	int numCPU;

	pthread_t *threads;

	pthread_mutex_t lock;

	struct Rectangle *blocks;

	int blocksCnt;

	int blocksCurr;

	int hasUI;

	int cancel;

	int showProgress;

	int quadViewLevel;

	int passId;

	int quadMaxLevel;

	double subdivThreshold;

	struct _timeb timebuffer;

	Ihandle* timer;
};

typedef struct _mainui * MainUI;

static MainUI _MainUI;

MainUI muiGetUI(void);

char * muiGetFileName(void);

int muiHandleResize(Ihandle *self, int width, int height);

int muiHandleRepaint(Ihandle *self);

char * muiGetNewFileName(void);

int muiHandleSaveClick(Ihandle *self);

int muiHandleIdle(void);

int muiHandleKeyPress(Ihandle *self, int c, int press);

int muiHandleLoadClick(void);

int muiHandleCancelClick(void);

int muiHandlePerformanceClick(void);

int muiHandleAboutClick(void);

int muiHandleSpinQuadTreeClick(Ihandle *ih, int pos);

int muiHandleQuadTreeValueClick(Ihandle *ih);

int muiHandleTimer(Ihandle *ih);

int muiHandlePassSelectItem(Ihandle *ih, char *text, int item, int state);

int muiHandleLevelChange(Ihandle *ih, int c, char *new_value);

int muiHandleProgressToogle(Ihandle* ih, int state);

MainUI muiInitMainUI(int argc, char **argv, int StartUI);

void * muiEngineWorker(void * arg);

void *muiEngineSubdivision(void*arg);

int muiEngineRender(char* scene, char* output);

int muiStart(void);

int muiClose(void);

#ifdef __cplusplus
}
#endif

#endif


