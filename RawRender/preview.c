#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <gl\glut.h>
#include <gl\gl.h>
#include <gl\glu.h>

#include "preview.h"
#include "color.h"
#include "algebra.h"


/*- Contexto do Programa: -------------------------------------------------*/
Scene scene;         /* cena corrente */
Camera camera;

void renderScene( void ) ;
void initCamera( ) ;

static int width, height = -1 ; /* alrgura e altura corrente */
static float angle = 0.0f, angleV = 0.0f, angleZ = 0.0f, deltaAngle = 0.0f, deltaAngleV = 0.0f, deltaAngleZ = 0.0f ;
static float deltaMoveX = 0.0f, deltaMoveY = 0.0f, deltaMoveZ = 0.0f ;

static Vector initialUp  = { 0.0f, 0.0f, 1.0f, 1.0f };
static Vector initialAt  = { 0.0f, 1.0f, 0.0f, 1.0f };
static Vector initialEye = { 0.0f, 0.0f, 1.0f, 1.0f };

static void centerCam()
{
   if ( !scene ) return ;
   
   camSet( camera, initialEye, initialUp, initialAt ) ;
   initCamera() ;
}

void setInitialCameraArgs(Vector up, Vector at, Vector eye)
{
	initialUp = up;
	initialAt = at;
	initialEye = eye;
}

static void orientMe()
{
   Matrix rotation, rotationV, rotationZ ;
   Vector newAt ;
   Vector newDir, oldDir ;//, oldDirXZ, oldDirYZ, oldDirXY ;
   Vector newUp ;//,oldUpXZ, oldUpYZ, oldUpXY ;
   
   if ( !scene ) return ;

   rotation  = algMatrixRotate(  angle, camera->yAxis.x, camera->yAxis.y, camera->yAxis.z ) ;
   oldDir = algSub( camera->at, camera->eye ) ;
   newDir = algTransf( rotation, oldDir ) ;
   newAt = algAdd( camera->eye, newDir ) ;
   newUp = algTransf( rotation, camera->up ) ;
   camSet( camera, camera->eye, newUp, newAt ) ;

   rotationV = algMatrixRotate( angleV, camera->xAxis.x, camera->xAxis.y, camera->xAxis.z ) ;
   oldDir = algSub( camera->at, camera->eye ) ;
   newDir = algTransf( rotationV, oldDir ) ;
   newAt = algAdd( camera->eye, newDir ) ;
   newUp = algTransf( rotationV, camera->up ) ;
   camSet( camera, camera->eye, newUp, newAt ) ;

   rotationZ = algMatrixRotate( angleZ, camera->zAxis.x, camera->zAxis.y, camera->zAxis.z ) ;
   oldDir = algSub( camera->at, camera->eye ) ;
   newDir = algTransf( rotationZ, oldDir ) ;
   newAt = algAdd( camera->eye, newDir ) ;
   newUp = algTransf( rotationZ, camera->up ) ;
   camSet( camera, camera->eye, newUp, newAt ) ;

   /**/

   /*/ Girar para a esquerda e direita
   rotation  = algMatrixRotate(  angle, camera->yAxis.x, camera->yAxis.y, camera->yAxis.z ) ;
   oldDir = algSub( camera->at, camera->eye ) ;
   oldDirXZ = algAdd( algProj( oldDir, camera->xAxis ), algProj( oldDir, camera->zAxis ) ) ;
   oldUpXZ = algAdd( algProj( camera->up, camera->xAxis ), algProj( camera->up, camera->zAxis ) ) ;
   
   newDir = algAdd( algTransf( rotation, oldDirXZ ), algSub( oldDir, oldDirXZ ) ) ;
   newAt = algAdd( camera->eye, newDir ) ;
   newUp = algAdd( algTransf( rotation, oldUpXZ ), algSub( camera->up, oldUpXZ ) ) ;
   camSet( camera, camera->eye, newUp, newAt ) ;

   // Inclinar para cima e para baixo
   rotationV = algMatrixRotate( angleV, camera->xAxis.x, camera->xAxis.y, camera->xAxis.z ) ;
   oldDir = algSub( camera->at, camera->eye ) ;
   oldDirYZ = algAdd( algProj( oldDir, camera->yAxis ), algProj( oldDir, camera->zAxis ) ) ;
   oldUpYZ = algAdd( algProj( camera->up, camera->yAxis ), algProj( camera->up, camera->zAxis ) ) ;

   newDir = algAdd( algTransf( rotationV, oldDirYZ ), algSub( oldDir, oldDirYZ ) ) ;
   newAt = algAdd( camera->eye, newDir ) ;
   newUp = algAdd( algTransf( rotationV, oldUpYZ ), algSub( camera->up, oldUpYZ ) ) ;
   camSet( camera, camera->eye, newUp, newAt ) ;

   // Inclinar para esquerda e para direita
   rotationZ = algMatrixRotate( angleZ, camera->zAxis.x, camera->zAxis.y, camera->zAxis.z ) ;
   oldDir = algSub( camera->at, camera->eye ) ;
   oldDirXY = algAdd( algProj( oldDir, camera->xAxis ), algProj( oldDir, camera->yAxis ) ) ;
   oldUpXY = algAdd( algProj( camera->up, camera->xAxis ), algProj( camera->up, camera->yAxis ) ) ;

   newDir = algAdd( algTransf( rotationZ, oldDirXY ), algSub( oldDir, oldDirXY ) ) ;
   newAt = algAdd( camera->eye, newDir ) ;
   newUp = algAdd( algTransf( rotationZ, oldUpXY ), algSub( camera->up, oldUpXY ) ) ;
   camSet( camera, camera->eye, newUp, newAt ) ;*/

   initCamera() ;
}

static void moveMeFlat(float x, float y, float z)
{
   Vector newEye, newAt ;

   if ( !scene ) return ;

   newEye.x = camera->eye.x + camera->xAxis.x * x + camera->yAxis.x * y + camera->zAxis.x * z ;
   newEye.y = camera->eye.y + camera->xAxis.y * x + camera->yAxis.y * y + camera->zAxis.y * z ;
   newEye.z = camera->eye.z + camera->xAxis.z * x + camera->yAxis.z * y + camera->zAxis.z * z ;

   newAt.x = camera->at.x + camera->xAxis.x * x + camera->yAxis.x * y + camera->zAxis.x * z ;
   newAt.y = camera->at.y + camera->xAxis.y * x + camera->yAxis.y * y + camera->zAxis.y * z ;
   newAt.z = camera->at.z + camera->xAxis.z * x + camera->yAxis.z * y + camera->zAxis.z * z ;

   camSet( camera, newEye, camera->up, newAt ) ;
   initCamera() ;
}

static void setLighting()
{
	int i;
	Color ambientColor;
	GLfloat sceneAmbientLight[4]; // luz ambiente da cena (nao depende de nenhuma fonte de luz)

	ambientColor = sceGetAmbientLight(scene);
	sceneAmbientLight[0] = (GLfloat) ambientColor.red;
	sceneAmbientLight[1] = (GLfloat) ambientColor.green;
	sceneAmbientLight[2] = (GLfloat) ambientColor.blue;
	sceneAmbientLight[3] = (GLfloat) 1.0f;
	
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, sceneAmbientLight);

	for ( i = 0 ; i < scene->lightCount ; i++ )
	{
		Light light;
		GLfloat ambientLight[4] = {0.0f, 0.0f, 0.0f, 1.0f}; // preto pois a luz ambiente eh da cena e nao de cada fonte de luz
		GLfloat lightColor[4]; // usada para as componentes difusa e especular
		GLfloat lightPosition[4];

		glPushMatrix() ;
	    
		light = scene->lights[i] ;
		lightColor[0] = (GLfloat) light->color.red;
		lightColor[1] = (GLfloat) light->color.green;
		lightColor[2] = (GLfloat) light->color.blue;
		lightColor[3] = (GLfloat) 1.0f;

		lightPosition[0] = (GLfloat) light->position.x;
		lightPosition[1] = (GLfloat) light->position.y;
		lightPosition[2] = (GLfloat) light->position.z;
		lightPosition[3] = (GLfloat) light->position.w;

		glLightfv(GL_LIGHT0 + i, GL_AMBIENT, ambientLight);
		glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, lightColor);
		glLightfv(GL_LIGHT0 + i, GL_SPECULAR, lightColor);
		glLightfv(GL_LIGHT0 + i, GL_POSITION, lightPosition);

		glEnable(GL_LIGHT0 + i);
	    
		glPopMatrix() ;
	}
}

void initCamera( )
{
	int i;
	float ratio ;

	if ( !scene ) return ;

	ratio = 1.0f * camera->screenWidth / camera->screenHeight ;
	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Set the clipping volume
	//gluPerspective(camera->fovy,ratio,camera->nearp,camera->farp);
	gluPerspective(camera->fovy,ratio,camera->nearp,100000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(camera->eye.x, camera->eye.y, camera->eye.z, 
				camera->at.x,camera->at.y,camera->at.z,
				camera->up.x, camera->up.y, camera->up.z ) ;

	// Habilita iluminacao
	glEnable(GL_LIGHTING);

	// Habilita color tracking (chamadas a glColor afetam a cor dos objetos)
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// Desabilita luzes
	for (i = 0; i < 8; ++i) {
		glDisable(GL_LIGHT0 + i);
	}
}

void renderScene()
{
   int i ;
   Object object ;
   Material material ;
   Sphere * sphere ;
   Triangle * triangle ;
   Box * box ;

   if ( !scene ) return ;

   if ( deltaMoveX || deltaMoveY || deltaMoveZ )
      moveMeFlat( deltaMoveX, deltaMoveY, deltaMoveZ ) ;
   if ( deltaAngle || deltaAngleV || deltaAngleZ )
   {
      angle = deltaAngle ;
      angleV = deltaAngleV ;
      angleZ = deltaAngleZ ;
      orientMe();
   }
   
   glClearColor( scene->bgColor.red, scene->bgColor.green, scene->bgColor.blue, 1.0f ) ;
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   setLighting();
   
   for ( i = 0 ; i < scene->objectCount ; i++ )
   {
      GLfloat specular[4];
      glPushMatrix() ;
      object = scene->objects[i] ;
      material = scene->materials[object->material] ;
      
	  glColor4f( material->diffuseColor.red, material->diffuseColor.green, material->diffuseColor.blue, 1.0f ) ;
	  
	  specular[0] = (GLfloat) material->specularColor.red;
	  specular[1] = (GLfloat) material->specularColor.green;
	  specular[2] = (GLfloat) material->specularColor.blue;
	  specular[3] = (GLfloat) 1.0f;
	  glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	  glMateriali(GL_FRONT, GL_SHININESS, 64);

      switch( object->type )
      {
         case TYPE_TRIANGLE:
            triangle = ( Triangle * ) object->data ;
            glBegin( GL_TRIANGLES );
               glVertex3f( triangle->v0.x, triangle->v0.y, triangle->v0.z ) ;
               glVertex3f( triangle->v1.x, triangle->v1.y, triangle->v1.z ) ;
               glVertex3f( triangle->v2.x, triangle->v2.y, triangle->v2.z ) ;
            glEnd() ;
            break ;
         case TYPE_SPHERE:
            sphere = ( Sphere * ) object->data ;
            glTranslatef( sphere->center.x, sphere->center.y, sphere->center.z ) ;
            glutSolidSphere( sphere->radius, 20, 20);
            break ;
         case TYPE_BOX:
            box = ( Box * ) object->data ;
	         glBegin(GL_QUADS); // Frontal
			   glNormal3f( 0.0f, 0.0f, -1.0f );
		       glVertex3f( box->bottomLeft.x, box->bottomLeft.y, box->bottomLeft.z ) ;
		       glVertex3f( box->topRight.x,   box->bottomLeft.y, box->bottomLeft.z ) ;
               glVertex3f( box->topRight.x,   box->topRight.y,   box->bottomLeft.z ) ;
               glVertex3f( box->bottomLeft.x, box->topRight.y,   box->bottomLeft.z ) ;
	         glEnd();
	         glBegin(GL_QUADS); // Superior
			   glNormal3f( 0.0f, 1.0f, 0.0f );
		       glVertex3f( box->bottomLeft.x, box->topRight.y, box->bottomLeft.z ) ;
		       glVertex3f( box->topRight.x,   box->topRight.y, box->bottomLeft.z ) ;
               glVertex3f( box->topRight.x,   box->topRight.y, box->topRight.z ) ;
               glVertex3f( box->bottomLeft.x, box->topRight.y, box->topRight.z ) ;
	         glEnd();
	         glBegin(GL_QUADS); // Esquerda
			   glNormal3f( -1.0f, 0.0f, 0.0f );
               glVertex3f( box->bottomLeft.x, box->topRight.y,   box->topRight.z ) ;
               glVertex3f( box->bottomLeft.x, box->bottomLeft.y, box->topRight.z ) ;
               glVertex3f( box->bottomLeft.x, box->bottomLeft.y, box->bottomLeft.z ) ;
               glVertex3f( box->bottomLeft.x, box->topRight.y,   box->bottomLeft.z ) ;
	         glEnd();
	         glBegin(GL_QUADS); // Direita
			   glNormal3f( 1.0f, 0.0f, 0.0f );
               glVertex3f( box->topRight.x, box->bottomLeft.y, box->bottomLeft.z ) ;
               glVertex3f( box->topRight.x, box->topRight.y,   box->bottomLeft.z ) ;
               glVertex3f( box->topRight.x, box->topRight.y,   box->topRight.z ) ;
               glVertex3f( box->topRight.x, box->bottomLeft.y, box->topRight.z ) ;
	         glEnd();
	         glBegin(GL_QUADS); // Traseiro
			   glNormal3f( 0.0f, 0.0f, 1.0f );
               glVertex3f( box->bottomLeft.x, box->bottomLeft.y, box->topRight.z ) ;
               glVertex3f( box->topRight.x,   box->bottomLeft.y, box->topRight.z ) ;
               glVertex3f( box->topRight.x,   box->topRight.y,   box->topRight.z ) ;
               glVertex3f( box->bottomLeft.x, box->topRight.y,   box->topRight.z ) ;
	         glEnd();
	         glBegin(GL_QUADS); // Inferior
			   glNormal3f( 0.0f, -1.0f, 0.0f );
               glVertex3f( box->bottomLeft.x, box->bottomLeft.y, box->bottomLeft.z ) ;
               glVertex3f( box->bottomLeft.x, box->bottomLeft.y, box->topRight.z ) ;
               glVertex3f( box->topRight.x,   box->bottomLeft.y, box->topRight.z ) ;
               glVertex3f( box->topRight.x,   box->bottomLeft.y, box->bottomLeft.z ) ;
	         glEnd();
            break ;
         default:
            break ;
      }
      glPopMatrix() ;
   }

   return ;
}

void pressKey(int key, int x, int y)
{
	switch (key)
   {
		case GLUT_KEY_F1: 
			//deltaMoveX = -0.1f;
			deltaMoveX = -1.0f;
			break;
		case GLUT_KEY_F2: 
			//deltaMoveX = 0.1f;
			deltaMoveX = 1.0f;
			break;
		case GLUT_KEY_PAGE_UP: 
			//deltaMoveY = 0.1f;
			deltaMoveY = 1.0f;
			break;
		case GLUT_KEY_PAGE_DOWN: 
			//deltaMoveY = -0.1f;
			deltaMoveY = -1.0f;
			break;
		case GLUT_KEY_F5: 
			centerCam();
			break;
		case GLUT_KEY_HOME: 
			//deltaMoveZ = -0.1f;
			deltaMoveZ = -1.0f;
			break;
		case GLUT_KEY_END: 
			//deltaMoveZ = 0.1f;
			deltaMoveZ = 1.0f;
			break;
		case GLUT_KEY_LEFT: 
			//deltaAngle = 0.1f;
			deltaAngle = 1.0f;
			break;
		case GLUT_KEY_RIGHT: 
			//deltaAngle = -0.1f;
			deltaAngle = -1.0f;
			break;
		case GLUT_KEY_UP: 
			//deltaAngleV = 0.1f;
			deltaAngleV = 1.0f;
			break;
		case GLUT_KEY_DOWN: 
			//deltaAngleV = -0.1f;
			deltaAngleV = -1.0f;
			break;
		case GLUT_KEY_F9: 
			//deltaAngleZ = 0.1f;
			deltaAngleZ = 1.0f;
			break;
		case GLUT_KEY_F10: 
			//deltaAngleZ = -0.1f;
			deltaAngleZ = -1.0f;
			break;
	}

}

void releaseKey(int key, int x, int y)
{
	switch (key)
   {
		case GLUT_KEY_LEFT: 
		case GLUT_KEY_RIGHT: 
			deltaAngle = 0.0f;
			break;
		case GLUT_KEY_UP: 
		case GLUT_KEY_DOWN: 
			deltaAngleV = 0.0f;
			break;
		case GLUT_KEY_F9: 
		case GLUT_KEY_F10: 
			deltaAngleZ = 0.0f;
			break;
		case GLUT_KEY_F1: 
		case GLUT_KEY_F2: 
			deltaMoveX = 0.0f;
			break;
		case GLUT_KEY_PAGE_UP: 
		case GLUT_KEY_PAGE_DOWN: 
			deltaMoveY = 0.0f;
			break;
		case GLUT_KEY_HOME: 
		case GLUT_KEY_END: 
			deltaMoveZ = 0.0f;
			break;
	}
}
