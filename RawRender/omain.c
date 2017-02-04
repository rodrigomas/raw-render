/*  
 *	@file main.c 
 *
 *	@author 
 *			- Rodrigo Marques
 *
 *	@date
 *			Última Modificação:		10 de Janeiro de 2014
 *
 *	@version 2.0
 *
 */

#include <pthread.h>

#ifdef _WIN32
#include <windows.h>                /* inclui as definicoes do windows para o OpenGL */
#endif

#include <stdio.h>
#include <time.h>

/*- Inclusao das bibliotecas IUP e CD: ------------------------------------*/
#include <iup.h>                    /* interface das funcoes IUP */
#include <iupgl.h>                  /* interface da funcao do canvas do OpenGL no IUP */
#include <gl/gl.h>                  /* prototypes do OpenGL */
#include <gl/glu.h>                 /* prototypes do OpenGL */
#include <gl/glut.h>                /* prototypes do OpenGL */

/* Funcoes de tempo */
#include <sys/timeb.h>


#include "image.h"                  /* TAD para imagem */
#include "color.h"
#include "algebra.h"
#include "raytracing.h"

#define MAX( a, b ) ( ( (a) > (b) ) ? (a) : (b) )
#define MIN( a, b ) ( ( (a) < (b) ) ? (a) : (b) )

/* -- implemented in "iconlib.c" to load standard icon images into IUP */
void IconLibOpen(void);

/*- Contexto do Programa: -------------------------------------------------*/
Scene scene = NULL;  /* cena corrente */

int width,height=-1; /* largura e altura corrente */

Image image;         /* imagem que armazena o resultado até agora do algoritmo */
Vector eye;
Camera camera;
int yc = 0;

Ihandle *canvas;   /* ponteiro IUP dos canvas */
Ihandle *label;                /* ponteiro IUP do label para colocar mensagens para usuario */
Ihandle *dialog;    /* ponteiro IUP do dialogo para mudar o titulo */
pthread_t idx_renderthread;
int numCPU = 8;
pthread_t *threads = NULL;

/*- Funcoes auxiliares ------------*/

/* Dialogo de selecao de arquivo  */
static char * get_file_name( void )
{
  Ihandle* getfile = IupFileDlg();
  char* filename = NULL;

  IupSetAttribute(getfile, IUP_TITLE, "Abertura de arquivo"  );
  IupSetAttribute(getfile, IUP_DIALOGTYPE, IUP_OPEN);
  IupSetAttribute(getfile, IUP_FILTER, "*.rt4");
  IupSetAttribute(getfile, IUP_FILTERINFO, "Arquivo de cena (*.rt4)");
  IupPopup(getfile, IUP_CENTER, IUP_CENTER); 

  filename = IupGetAttribute(getfile, IUP_VALUE);
  return filename;
}


/*------------------------------------------*/
/* Callbacks do IUP.                        */
/*------------------------------------------*/


/* - Callback de mudanca de tamanho no canvas (mesma para ambos os canvas) */
int resize_cb(Ihandle *self, int width, int height)
{
 IupGLMakeCurrent(self);  /* torna o foco do OpenGL para este canvas */

 /* define a area do canvas que deve receber as primitivas do OpenGL */
 glViewport(0,0,width,height);

 /* transformacao de instanciacao dos objetos no sistema de coordenadas da camera */
 glMatrixMode(GL_MODELVIEW);
 glLoadIdentity();           /* identidade, ou seja nada */

 /* transformacao de projecao */
 glMatrixMode(GL_PROJECTION);
 glLoadIdentity();
 gluOrtho2D (0.0, (GLsizei)(width), 0.0, (GLsizei)(height));  /* ortografica no plano xy de [0,w]x[0,h] */
  
 glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
 glClear(GL_COLOR_BUFFER_BIT);

 return IUP_DEFAULT; /* retorna o controle para o gerenciador de eventos */
}

/* - Callback de repaint do canvas  */
int repaint_cb(Ihandle *self)
{
  int w,h;
  int x,y;
  Color rgb;

  if (image == NULL) {
	  int i, j;
	  Color pixel = {0.0, 0.0, 0.0};
	  image = imageCreate(500, 500);

	  for (i = 0; i < 500; ++i) {
		  for (j = 0; j < 500; ++j) {
			  imageSetPixel(image, i, j, pixel);
		  }
	  }
  }

  imageGetDimensions(image,&w,&h);
  IupGLMakeCurrent(self);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  glBegin(GL_POINTS);
    for (y=0;y<h;y++) {
      for (x=0; x<w; x++) {
         rgb=imageGetPixel(image, x, y);
         glColor3f((float)rgb.red,(float)rgb.green,(float)rgb.blue);
		 //glColor3f(1, 0, 0);
         glVertex2i(x,y);
       }
    }
  glEnd();
  
  glFlush();
  IupGLSwapBuffers(self);

  return IUP_DEFAULT; /* retorna o controle para o gerenciador de eventos */
} 


/* captura um nome para um arquivo */
static char * get_new_file_name( void )
{
  Ihandle* getfile = IupFileDlg();
  char* filename = NULL;

  IupSetAttribute(getfile, IUP_TITLE, "Salva arquivo"  );
  IupSetAttribute(getfile, IUP_DIALOGTYPE, IUP_SAVE);
  IupSetAttribute(getfile, IUP_FILTER, "*.tga");
  IupSetAttribute(getfile, IUP_FILTERINFO, "Arquivo de imagem (*.tga)");
  IupPopup(getfile, IUP_CENTER, IUP_CENTER);  /* o posicionamento nao esta sendo respeitado no Windows */

  filename = IupGetAttribute(getfile, IUP_VALUE);
  return filename;
}

/* salva a imagem gerada */
int save_cb(Ihandle *self)
{
  char* filename = get_new_file_name( );  /* chama o dialogo de abertura de arquivo */
  if (filename==NULL) return 0;
  imageWriteTGA(filename,image);
  return IUP_DEFAULT;
}

void drawImage( )
{
   int x;

   eye = camGetEye( camera );

   IupGLMakeCurrent(canvas);
   
   if (yc < height) {
	  glBegin(GL_POINTS);
      for( x = 0; x < width; ++x )
      {
         Color pixel = { 0.0, 0.0, 0.0 };
         Vector ray;				
         Color color;
         
         ray = camGetRay( camera, x, yc );

         /* Obtém a amostra. */
         color = rayTrace( scene, eye, ray, 0 );

         /* Adiciona a contribuição da amostra à cor final */
         pixel = colorAddition( pixel, color );

         imageSetPixel( image, x, yc, pixel );
         glColor3f((float)pixel.red,(float)pixel.green,(float)pixel.blue);
         glVertex2i(x,yc);
      }
      yc++;
	  glEnd();
   }
}

/* calcula uma linha da imagem a cada camada de idle */
int idle_cb(void)
{
   if (yc < height)
      drawImage( ) ;
     
   repaint_cb(canvas);

   return IUP_DEFAULT;
}

/* Trata os eventos de teclado */
int special_cb(Ihandle *self, int c, int press)
{
//   char texto[256];
   //char tecla = tolower(c);

   IupGLMakeCurrent( self ) ;
 
   return IUP_DEFAULT;
}


/* carrega uma nova cena */
int load_cb(void) 
{
  char* filename = get_file_name();  /* chama o dialogo de abertura de arquivo */
  char buffer[256];
  char* slash;

  if (filename==NULL) return 0;

  /* Le a cena especificada */
  if (scene != NULL) {
	  sceDestroy(scene);
  }
  scene = sceLoad( filename );
  if( scene == NULL ) return IUP_DEFAULT;

  camera = sceGetCamera( scene );
  eye = camGetEye( camera );

  //setInitialCameraArgs(camera->up, camera->at, camera->eye);

  width = camGetScreenWidth( camera );
  height = camGetScreenHeight( camera );
  yc=0;

  if (image) imageDestroy(image);
  image = imageCreate( width, height );
  
   sprintf(buffer, "Ambiente %c - Difusa %c - Especular %c - Sombra %c - Depth %d - Refracao %c",
      flagAmbiente?'S':'N', flagDifusa?'S':'N', flagEspecular?'S':'N',
	  flagSombra?'S':'N', curDepth, flagRefracao?'S':'N');
  IupSetfAttribute(label, "TITLE", buffer);

  IupGLMakeCurrent( canvas ) ;
  //glEnable ( GL_DEPTH_TEST ) ;
  //initCamera( ) ;
  IupSetFunction (IUP_IDLE_ACTION, (Icallback) idle_cb);
  sprintf(buffer,"%3dx%3d", width, height);
  IupSetAttribute(canvas,IUP_RASTERSIZE,buffer);

  slash = strrchr(filename, '\\');
  ++slash;
  sprintf(buffer, "Ray Tracer (%s)", slash);
  IupSetAttribute(dialog, "TITLE", buffer);
  return IUP_DEFAULT;
}

/*-------------------------------------------------------------------------*/
/* Incializa o programa.                                                   */
/*-------------------------------------------------------------------------*/

BOOL init(void)
{
  Ihandle *statusbar,  *box ;

  Ihandle *toolbar, *load, *save;

  /* creates the toolbar and its buttons */
  load = IupButton("", "load_cb");
  IupSetAttribute(load,"TIP","Carrega uma imagem.");
  IupSetAttribute(load,"IMAGE","icon_lib_open");
  IupSetFunction("load_cb", (Icallback)load_cb);

  save = IupButton("", "save_cb");
  IupSetAttribute(save,"TIP","Salva no formato GIF.");
  IupSetAttribute(save,"IMAGE","icon_lib_save");
  
  
  toolbar = IupHbox(
       load, 
       save,
	   IupFill(),
     NULL);

  IupSetAttribute(toolbar, "ALIGNMENT", "ACENTER");
 

  /* cria um canvas */
  canvas = IupGLCanvas("repaint_cb"); 
  IupSetAttribute(canvas, IUP_BUFFER, IUP_DOUBLE ) ;
  IupSetAttribute(canvas,IUP_RASTERSIZE,"500x500");
  IupSetAttribute(canvas, "RESIZE_CB", "resize_cb");
  IupSetAttribute(canvas, "KEYPRESS_CB", "special_cb");

  /* associa o evento de repaint a funccao repaint_cb */
  IupSetFunction("repaint_cb", (Icallback) repaint_cb);
  IupSetFunction("save_cb", (Icallback)save_cb);
  IupSetFunction("resize_cb", (Icallback) resize_cb);
  IupSetFunction (IUP_IDLE_ACTION, (Icallback) NULL);
  IupSetFunction("special_cb", (Icallback) special_cb);

  /* the status bar is just a label to put some usefull information in run time */
  label = IupLabel("status");
  IupSetAttribute(label, "EXPAND", "HORIZONTAL");
  IupSetAttribute(label, "FONT", "COURIER_NORMAL_10");
  statusbar = IupSetAttributes(IupHbox(
                IupFrame(IupHbox(label, NULL)), 
                NULL), "MARGIN=5x5");

  ///* cria o canvas GL */
  //canvasGL = IupGLCanvas( "repaintGL_cb" ) ; 
  //IupSetAttribute( canvasGL, "KEYPRESS_CB", "special_cb" ) ;
  //IupSetAttribute( canvasGL,IUP_RASTERSIZE,"500x500" ) ;
  //IupSetAttribute( canvasGL, IUP_BUFFER, IUP_DOUBLE ) ;
  //IupSetFunction("repaintGL_cb", (Icallback) repaintGL_cb);

  /* this is the most external box that puts together
     the toolbar, the two canvas and the status bar */
  box = IupVbox(
          toolbar,
          IupSetAttributes(canvas, "GAP=1"),
          statusbar, 
          NULL);

  /* create the dialog and set its attributes */
  dialog = IupDialog(box);
  IupSetAttribute(dialog, "CLOSE_CB", "app_exit_cb");
  IupSetAttribute(dialog, "TITLE", "Ray Tracer");


  IupShowXY(dialog, IUP_CENTER, IUP_CENTER);

  return TRUE;
}

pthread_mutex_t lock;

#define BLOCK_SIZE 32

struct ProcBlock
{
	int xmin, xmax, ymin, ymax;
};

struct ProcBlock *blocks = NULL;
int blocksCnt = 0;
int blocksCurr = 0;

void * renderWorker(void * arg)
{
	int p = *((int*)arg);

	int block = 0;

	while (blocksCurr < blocksCnt)
	{		
		pthread_mutex_lock(&lock);
		block = blocksCurr++;
		pthread_mutex_unlock(&lock);

		if (block >= blocksCnt)
			break;		

		printf("\nThread %d: Block Id (%d) Started\n", p, block);

		struct ProcBlock *b = &blocks[block];

		for (register int y = b->ymin; y < b->ymax; ++y)
		{
			for (register int x = b->xmin; x < b->xmax; ++x)
			{
				Color pixel = { 0.0, 0.0, 0.0 };
				Vector ray;
				Color color;

				ray = camGetRay(camera, x, y);

				color = rayTrace(scene, camera->eye, ray, 0);

				pixel = colorAddition(pixel, color);

				imageSetPixel(image, x, y, pixel);
			}
		}

		printf("\nThread %d: Block Id (%d) Ended\n", p, block);
	}

	return NULL;
}

void *renderMain(void*arg)
{	
	int nw, nh, nt, nft;
	
	nw = width / BLOCK_SIZE;
	nh = height / BLOCK_SIZE;

	nt = nw * nh + ((width % BLOCK_SIZE > 0) ? 1 : 0) * nh + ((height % BLOCK_SIZE > 0) ? 1 : 0) * nw;

	blocksCnt = nt;
	blocksCurr = 0;
	blocks = (struct ProcBlock*)malloc(sizeof(struct ProcBlock));	

	//quadrados
	int idx = 0;
	for (int y = 0; y < nh; ++y)
	{
		for (int x = 0; x < nw; ++x)
		{
			blocks[idx].xmin = x * BLOCK_SIZE;
			blocks[idx].xmax = blocks[idx].xmin + BLOCK_SIZE;

			blocks[idx].ymin = y * BLOCK_SIZE;
			blocks[idx].ymax = blocks[idx].ymin + BLOCK_SIZE;

			idx++;
		}
	}

	for (int x = 0; x < nw; ++x)
	{
		blocks[idx].xmin = x * BLOCK_SIZE;
		blocks[idx].xmax = blocks[idx].xmin + BLOCK_SIZE;

		blocks[idx].ymin = nh * BLOCK_SIZE;
		blocks[idx].ymax = height;

		idx++;
	}

	for (int y = 0; y < nh; ++y)
	{
		blocks[idx].xmin = nw * BLOCK_SIZE;
		blocks[idx].xmax = width;

		blocks[idx].ymin = y * BLOCK_SIZE;
		blocks[idx].ymax = blocks[idx].ymin + BLOCK_SIZE;

		idx++;
	}

	if (pthread_mutex_init(&lock, NULL) != 0)
	{
		printf("\n mutex init failed\n");
		return NULL;
	}

	threads = (pthread_t*)malloc(sizeof(pthread_t));
	int *th = (int*)malloc(sizeof(int));

	for (register int i = 0; i < numCPU; i++)
	{
		th[i] = i;
		if (pthread_create(&threads[i], NULL, renderWorker, &(th[i])))
		{
			fprintf(stderr, "Error creating thread\n");
			return NULL;
		}
	}

	for (register int i = 0; i < numCPU; i++)
	{
		if (pthread_join(threads[i], NULL)) {

			fprintf(stderr, "Error joining thread\n");
			return NULL;
		}
	}	

	pthread_mutex_destroy(&lock);

	free(th);
	free(threads);
	free(blocks);
	blocksCnt = 0;
}

int renderScene(char* scene, char* output)
{
	char* filename = scene;
	char buffer[256];
	char* slash;

#ifdef _WIN32
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);

	numCPU = sysinfo.dwNumberOfProcessors;
#else
	numCPU = sysconf(_SC_NPROCESSORS_ONLN);
#endif

	if (filename == NULL) return 0;

	if (scene != NULL) 
	{
		sceDestroy(scene);
	}

	scene = sceLoad(filename);
	
	if (scene == NULL) return IUP_DEFAULT;

	camera = sceGetCamera(scene);

	eye = camGetEye(camera);

	//setInitialCameraArgs(camera->up, camera->at, camera->eye);

	width = camGetScreenWidth(camera);
	height = camGetScreenHeight(camera);
	yc = 0;

	if (image) imageDestroy(image);
	image = imageCreate(width, height);
		
	if (pthread_create(&idx_renderthread, NULL, renderMain, NULL)) 
	{
		fprintf(stderr, "Error creating thread\n");
		return IUP_ERROR;
	}	
}

/*-------------------------------------------------------------------------*/
/* Rotina principal.                                                       */
/*-------------------------------------------------------------------------*/
void main(int argc, char** argv) 
{
	char* scenefile = NULL;	
	char* outputfile = NULL;
	BOOL verbose = FALSE;
	BOOL console = FALSE;

	for (register int i = 1; i < argc; i++)
	{
		if (strncmp("-s", argv[i], 2) == 0)
		{
			scenefile = argv[i + 1];
			i++;
		}
		else if (strncmp("-o", argv[i], 2) == 0)
		{
			outputfile = argv[i + 1];
			i++;

		} else if (strncmp("-verbose", argv[i], 8) == 0)
		{
			verbose = TRUE;
		} else if (strncmp("-console", argv[i], 8) == 0)
		{
			console = TRUE;
		}
	}

	if (scenefile != NULL)
	{
		 renderScene(scenefile, outputfile);
	}
	
	if (console == FALSE)
	{
		IupOpen(&argc, &argv);
		IupGLCanvasOpen();
		IconLibOpen();	
		IupImageLibOpen();
		
		if (init() == TRUE)
		{
			IupMainLoop();
		}
		
		IupClose();
	}	
}