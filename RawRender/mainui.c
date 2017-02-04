#include "mainui.h"

extern MainUI _MainUI = NULL;


MainUI muiGetUI(void)
{
	return _MainUI;
}

char * muiGetFileName(void)
{
	Ihandle* getfile = IupFileDlg();
	char* filename = NULL;

	IupSetAttribute(getfile, IUP_TITLE, "Load Scene Frame File");
	IupSetAttribute(getfile, IUP_DIALOGTYPE, IUP_OPEN);
	IupSetAttribute(getfile, IUP_FILTER, "*.rt4");
	IupSetAttribute(getfile, IUP_FILTERINFO, "Scene Frame File (*.rt4)");
	IupPopup(getfile, IUP_CENTER, IUP_CENTER);

	filename = IupGetAttribute(getfile, IUP_VALUE);
	return filename;
}

int muiHandleResize(Ihandle *self, int width, int height)
{
	IupGLMakeCurrent(self);
							
	glViewport(0, 0, width, height);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
					
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLsizei)(width), 0.0, (GLsizei)(height));

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	muiGetUI()->width = width;
	muiGetUI()->height = height;

	if(muiGetUI()->camera != NULL)
		camResize(muiGetUI()->camera, width, height);

	return IUP_DEFAULT; 
}

int muiHandleRepaint(Ihandle *self)
{
	int w, h;
	int x, y;
	Color rgb;

	if (muiGetUI()->image == NULL) {
		
		int i, j;
		
		Color pixel = { 0.0, 0.0, 0.0 };

		muiGetUI()->image = imageCreate(muiGetUI()->width, muiGetUI()->height);

		for (i = 0; i < muiGetUI()->width; ++i) 
		{
			for (j = 0; j < muiGetUI()->height; ++j) 
			{
				imageSetPixel(muiGetUI()->image, i, j, pixel);				
			}
		}
	}

	imageGetDimensions(muiGetUI()->image, &w, &h);
	IupGLMakeCurrent(self);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glBegin(GL_POINTS);
	for (y = 0; y<h; y++) {
		for (x = 0; x<w; x++) {

			switch (muiGetUI()->passId)
			{
				case 1:
				case 2:
				case 3:
				case 4:
				case 5:
				case 6:
				case 7:
				case 8:
				case 9:
				{
					Vector *v = &muiGetUI()->scene->Maps[muiGetUI()->passId - 1][y * muiGetUI()->scene->imgWidth + x];
					glColor3f((float)v->x, (float)v->y, (float)v->z);
					glVertex2i(x, y);
				}
				break;

				case 0:
				default:
				{
					rgb = imageGetPixel(muiGetUI()->image, x, y);

					glColor3f((float)rgb.red, (float)rgb.green, (float)rgb.blue);
					glVertex2i(x, y);
				}
				break;
			}			
		}
	}
	glEnd();

	glFlush();
	IupGLSwapBuffers(self);

	return IUP_DEFAULT; /* retorna o controle para o gerenciador de eventos */
}

char * muiGetNewFileName(void)
{
	Ihandle* getfile = IupFileDlg();
	char* filename = NULL;

	IupSetAttribute(getfile, IUP_TITLE, "Save file");
	IupSetAttribute(getfile, IUP_DIALOGTYPE, IUP_SAVE);
	IupSetAttribute(getfile, IUP_FILTER, "*.tga");
	IupSetAttribute(getfile, IUP_FILTERINFO, "Image File (*.tga)");
	IupPopup(getfile, IUP_CENTER, IUP_CENTER);  /* o posicionamento nao esta sendo respeitado no Windows */

	filename = IupGetAttribute(getfile, IUP_VALUE);
	return filename;
}

int muiHandleSaveClick(Ihandle *self)
{
	char* filename = muiGetNewFileName();  /* chama o dialogo de abertura de arquivo */
	if (filename == NULL) return 0;

	imageWriteTGA(filename, muiGetUI()->image);

	sceSaveOutput(muiGetUI()->scene, filename);

	return IUP_DEFAULT;
}

int muiHandleIdle(void)
{
	muiHandleRepaint(muiGetUI()->canvas);
	return IUP_DEFAULT;
}

int muiHandleKeyPress(Ihandle *self, int c, int press)
{
	IupGLMakeCurrent(self);

	return IUP_DEFAULT;
}

int muiHandleLoadClick(void)
{
	char* filename = muiGetFileName();  /* chama o dialogo de abertura de arquivo */
	char buffer[256];
	char* slash;

	if (filename == NULL) return 0;

	/* Le a cena especificada */
	if (muiGetUI()->scene != NULL) {
		sceDestroy(muiGetUI()->scene);
	}
	muiGetUI()->scene = sceLoad(filename);
	if (muiGetUI()->scene == NULL) return IUP_DEFAULT;

	muiGetUI()->camera = sceGetCamera(muiGetUI()->scene);
	muiGetUI()->eye = camGetEye(muiGetUI()->camera);

	//setInitialCameraArgs(camera->up, camera->at, camera->eye);

	muiGetUI()->width = camGetScreenWidth(muiGetUI()->camera);
	muiGetUI()->height = camGetScreenHeight(muiGetUI()->camera);

	if (muiGetUI()->image) imageDestroy(muiGetUI()->image);
	muiGetUI()->image = imageCreate(muiGetUI()->width, muiGetUI()->height);

	IupGLMakeCurrent(muiGetUI()->canvas);

	IupSetFunction(IUP_IDLE_ACTION, (Icallback)muiHandleIdle);

	sprintf(buffer, "%3dx%3d", muiGetUI()->width, muiGetUI()->height);
	IupSetAttribute(muiGetUI()->canvas, IUP_RASTERSIZE, buffer);

	slash = strrchr(filename, '\\');
	++slash;
	sprintf(buffer, "Ray Tracer (%s)", slash);
	IupSetAttribute(muiGetUI()->dialog, "TITLE", buffer);

	return IUP_DEFAULT;
}

MainUI muiInitMainUI(int argc, char **argv, int StartUI)
{
	Ihandle *statusbar, *box;

	Ihandle *toolbar, *load, *save, *cancel, *perf, *about;

	if (_MainUI != NULL) return _MainUI;

	_MainUI = (struct _mainui*)malloc(sizeof(struct _mainui));

	memset(_MainUI, 0, sizeof(struct _mainui));	

	_MainUI->hasUI = StartUI;
	_MainUI->cancel = 0;
	_MainUI->quadViewLevel = 1;
	_MainUI->passId = 0;
	_MainUI->showProgress = 1;
	_MainUI->tree = NULL;
	_MainUI->quadMaxLevel = 7;
	_MainUI->subdivThreshold = 200;

	if (StartUI == 1)
	{
		IupOpen(&argc, &argv);
		IupGLCanvasOpen();
		IconLibOpen();
		IupImageLibOpen();

		load = IupButton("", "OnLoadClick");
		IupSetAttribute(load, "TIP", "Carrega uma imagem.");
		IupSetAttribute(load, "IMAGE", "IUP_FileOpen");
		IupSetAttribute(load, "FLAT", "YES");
		IupSetFunction("OnLoadClick", (Icallback)muiHandleLoadClick);

		save = IupButton("", "OnSaveClick");
		IupSetAttribute(save, "FLAT", "YES");
		IupSetAttribute(save, "TIP", "Salva no formato TIF.");
		IupSetAttribute(save, "IMAGE", "IUP_FileSave");
		IupSetFunction("OnSaveClick", (Icallback)muiHandleSaveClick);

		cancel = IupButton("", "OnCancelClick");
		IupSetAttribute(cancel, "FLAT", "YES");
		IupSetAttribute(cancel, "IMAGE", "IUP_ActionCancel");
		IupSetFunction("OnCancelClick", (Icallback)muiHandleCancelClick);

		perf = IupButton("", "OnPerformanceClick");
		IupSetAttribute(perf, "FLAT", "YES");
		IupSetAttribute(perf, "IMAGE", "IUP_FileProperties");
		IupSetFunction("OnPerformanceClick", (Icallback)muiHandlePerformanceClick);

		about = IupButton("", "OnAboutClick");
		IupSetAttribute(about, "FLAT", "YES");
		IupSetAttribute(about, "IMAGE", "IUP_MessageInfo");
		IupSetFunction("OnAboutClick", (Icallback)muiHandleAboutClick);

		muiGetUI()->passcombo = IupList("OnPassClick");		
		IupSetFunction("OnPassClick", (Icallback)muiHandlePassSelectItem);

		IupSetAttributes(muiGetUI()->passcombo, "1=Color, 2=Depth, 3=Normal, 4=Shadow, 5=Reflection, 6=Refraction, 7=Diffuse, 8=Specular, 9=GI,""DROPDOWN=YES, VISIBLEITEMS=3, VALUE=1");

		muiGetUI()->quadlevel = IupText("OnLevelChange");
		IupSetFunction("OnLevelChange", (Icallback)muiHandleLevelChange);

		IupSetAttribute(muiGetUI()->quadlevel, "SPIN_CB", "OnSpinChange");
		IupSetFunction("OnSpinChange", (Icallback)muiHandleSpinQuadTreeClick);

		IupSetAttribute(muiGetUI()->quadlevel, "VALUECHANGED_CB", "OnValueSpinChange");
		IupSetFunction("OnValueSpinChange", (Icallback)muiHandleQuadTreeValueClick);

		IupSetAttribute(muiGetUI()->quadlevel, "FILTER", "NUMBER");
		IupSetAttribute(muiGetUI()->quadlevel, "SPIN", "YES");	
		IupSetAttribute(muiGetUI()->quadlevel, "SPINMAX", "7");
		IupSetAttribute(muiGetUI()->quadlevel, "SPINMIN", "1");
		IupSetAttribute(muiGetUI()->quadlevel, "SPINVALUE", "1");

		muiGetUI()->timelabel = IupLabel("0.0 s");

		muiGetUI()->timer = IupTimer();
		IupSetAttribute(muiGetUI()->timer, "TIME", "100");
		IupSetAttribute(muiGetUI()->timer, "ACTION_CB", "OnTimer");
		IupSetFunction("OnTimer", (Icallback)muiHandleTimer);

		muiGetUI()->progressgauge = IupProgressBar();				

		IupSetAttribute(muiGetUI()->progressgauge, "VALUE", "0.0");
		IupSetAttribute(muiGetUI()->progressgauge, "ORIENTATION ", "HORIZONTAL");

		muiGetUI()->progresscheck = IupToggle("Show Progress", "OnToogleProgress");
		IupSetAttribute(muiGetUI()->progresscheck, "VALUE", "YES");		
		IupSetFunction("OnToogleProgress", (Icallback)muiHandleProgressToogle);

		toolbar = IupHbox(
			load,
			save,
			cancel,
			IupLabel(" "),
			IupSetAttributes(IupLabel(NULL), "SEPARATOR=VERTICAL"),
			IupLabel(" "),
			muiGetUI()->passcombo,
			IupLabel(" "),
			about,
			IupLabel(" Level View: "),
			muiGetUI()->quadlevel,
			IupFill(),
			IupLabel(" Time: "),
			muiGetUI()->timelabel,
			IupLabel(" "),
			muiGetUI()->progressgauge,
			IupLabel(" "),
			muiGetUI()->progresscheck,			
			perf,
			IupLabel("   "),
			NULL);

		IupSetAttribute(toolbar, "ALIGNMENT", "ACENTER");		
		IupSetAttribute(toolbar, "GAP", "2");
		IupSetAttribute(toolbar, "EXPAND", "HORIZONTAL");

		muiGetUI()->messageinformer = IupMultiLine(NULL);
		IupSetAttribute(muiGetUI()->messageinformer, "READONLY", "YES");
		IupSetAttribute(muiGetUI()->messageinformer, "EXPAND", "HORIZONTAL");
		IupSetAttribute(muiGetUI()->messageinformer, IUP_RASTERSIZE, "1280x100");

		muiGetUI()->canvas = IupGLCanvas("OnRepaint");
		IupSetAttribute(muiGetUI()->canvas, IUP_BUFFER, IUP_DOUBLE);
		IupSetAttribute(muiGetUI()->canvas, IUP_RASTERSIZE, "1280x720");
		IupSetAttribute(muiGetUI()->canvas, "RESIZE_CB", "OnResize");
		IupSetAttribute(muiGetUI()->canvas, "KEYPRESS_CB", "OnKeyPress");

		IupSetFunction("OnKeyPress", (Icallback)muiHandleKeyPress);
		IupSetFunction("OnRepaint", (Icallback)muiHandleRepaint);

		IupSetFunction("OnSaveClick", (Icallback)muiHandleSaveClick);
		IupSetFunction("OnResize", (Icallback)muiHandleResize);

		IupSetFunction(IUP_IDLE_ACTION, (Icallback)muiHandleIdle);		

		muiGetUI()->label = IupLabel(" Raw Render ");
		IupSetAttribute(muiGetUI()->label, "EXPAND", "HORIZONTAL");
		IupSetAttribute(muiGetUI()->label, IUP_RASTERSIZE, "500x30");
		//IupSetAttribute(muiGetUI()->label, "FONT", "COURIER_NORMAL_10");
		statusbar = muiGetUI()->label;

		box = IupVbox(
			toolbar,
			IupSetAttributes(muiGetUI()->canvas, "GAP=1"),
			muiGetUI()->messageinformer,
			statusbar,
			NULL);

		muiGetUI()->dialog = IupDialog(box);
		IupSetAttribute(muiGetUI()->dialog, "ICON", "MAINICON");
		//IupSetAttribute(muiGetUI()->dialog, "TRAY", "YES");		
		//IupSetAttribute(muiGetUI()->dialog, "TRAYIMAGE", "MAINICON");		
		IupSetAttribute(muiGetUI()->dialog, "CLOSE_CB", "OnExit");
		IupSetAttribute(muiGetUI()->dialog, "TITLE", "Raw Render - Ray Tracer");


		IupShowXY(muiGetUI()->dialog, IUP_CENTER, IUP_CENTER);
	}

	return IUP_DEFAULT;
}

int muiStart(void)
{
	if(muiGetUI()->hasUI)
		IupMainLoop();
}

int muiClose(void)
{
	if (muiGetUI()->hasUI)
		IupClose();

	if (pthread_join(muiGetUI()->idx_renderthread, NULL))
	{

		fprintf(stderr, "Error joining thread\n");
		return NULL;
	}
}

int muiHandleTimer(Ihandle *ih)
{
	char buffer[MAX_PATH];

	if (!muiGetUI()->hasUI)
	{
		return IUP_DEFAULT;
	}

	if (muiGetUI()->scene->rendered == 0)
	{
		struct _timeb buff;
		_ftime(&buff);		
		
		sprintf(buffer, "%.2f s", (buff.time * 1000 + buff.millitm - muiGetUI()->timebuffer.time * 1000 - muiGetUI()->timebuffer.millitm) / 1000.0);

		IupSetAttribute(muiGetUI()->timelabel, "VALUE", buffer);
	}
	else
	{
		double p = (double)muiGetUI()->blocksCurr / (double)muiGetUI()->blocksCnt;
		sprintf(buffer, "%.2f", p);

		IupSetAttribute(muiGetUI()->progressgauge, "VALUE", buffer);

		IupSetAttribute(muiGetUI()->label, "VALUE", "Rendered");
	}
}

void * muiEngineWorker(void * arg)
{
	int p = *((int*)arg);

	int block = 0;

	while (muiGetUI()->blocksCurr < muiGetUI()->blocksCnt)
	{
		if (muiGetUI()->cancel == 1)
		{
			break;
		}

		pthread_mutex_lock(&muiGetUI()->lock);
		block = muiGetUI()->blocksCurr++;
		pthread_mutex_unlock(&muiGetUI()->lock);

		if (block >= muiGetUI()->blocksCnt)
			break;

		printf("\nThread %d: Block Id (%d) Started\n", p, block);

		struct Rectangle *b = &muiGetUI()->blocks[block];

		for (register int y = b->ymin; y < b->ymax; ++y)
		{
			for (register int x = b->xmin; x < b->xmax; ++x)
			{
				Color pixel = { 0.0, 0.0, 0.0 };
				Vector ray;
				Color color;				

				color = rayTrace(muiGetUI()->scene, 0, x, y);

				pixel = colorAddition(pixel, color);

				imageSetPixel(muiGetUI()->image, x, y, pixel);
			}
		}

		printf("\nThread %d: Block Id (%d) Ended\n", p, block);
	}

	return NULL;
}

int QuadTreeSubdivisionRule(void* data, int xmin, int xmax, int ymin, int ymax)
{
	PerfomanceImage img = (PerfomanceImage)data;

	double  val = 0;

	for (int y = ymin; y < ymax; ++y)
	{
		for (int x = xmin; x < xmax; ++x)
		{
			PerfomanceImageData *pimg = piGetData(img, x, y);

			val += pimg->CpuTime;
		}
	}

	return (val > muiGetUI()->subdivThreshold);
}

void *muiEngineSubdivision(void*arg)
{
	int nw, nh, nt, nft;	

	if (muiGetUI()->scene->perfImage != NULL)
	{
		nw = muiGetUI()->width / BLOCK_SIZE;
		nh = muiGetUI()->height / BLOCK_SIZE;

		nt = nw * nh + ((muiGetUI()->width % BLOCK_SIZE > 0) ? 1 : 0) * nh + ((muiGetUI()->height % BLOCK_SIZE > 0) ? 1 : 0) * nw;

		muiGetUI()->blocksCnt = nt;
		muiGetUI()->blocksCurr = 0;
		muiGetUI()->blocks = (struct Rectangle*)malloc(nt * sizeof(struct Rectangle));

		//quadrados
		int idx = 0;

		for (int y = 0; y < nh; ++y)
		{
			for (int x = 0; x < nw; ++x)
			{
				muiGetUI()->blocks[idx].xmin = x * BLOCK_SIZE;
				muiGetUI()->blocks[idx].xmax = muiGetUI()->blocks[idx].xmin + BLOCK_SIZE;

				muiGetUI()->blocks[idx].ymin = y * BLOCK_SIZE;
				muiGetUI()->blocks[idx].ymax = muiGetUI()->blocks[idx].ymin + BLOCK_SIZE;

				idx++;
			}
		}

		for (int x = 0; x < nw; ++x)
		{
			muiGetUI()->blocks[idx].xmin = x * BLOCK_SIZE;
			muiGetUI()->blocks[idx].xmax = muiGetUI()->blocks[idx].xmin + BLOCK_SIZE;

			muiGetUI()->blocks[idx].ymin = nh * BLOCK_SIZE;
			muiGetUI()->blocks[idx].ymax = muiGetUI()->height;

			idx++;
		}

		//if (StartUI == 1)
		//{

		for (int y = 0; y < nh; ++y)
		{
			muiGetUI()->blocks[idx].xmin = nw * BLOCK_SIZE;
			muiGetUI()->blocks[idx].xmax = muiGetUI()->width;

			muiGetUI()->blocks[idx].ymin = y * BLOCK_SIZE;
			muiGetUI()->blocks[idx].ymax = muiGetUI()->blocks[idx].ymin + BLOCK_SIZE;

			idx++;
		}
	}
	else
	{
		muiGetUI()->tree = qudCreate(0, muiGetUI()->width, 0, muiGetUI()->height);

		qudComputeSubdividion(muiGetUI()->tree, QuadTreeSubdivisionRule, muiGetUI()->scene->perfImage, 0, muiGetUI()->quadMaxLevel);

		int cnt = qudGetLeafSize(muiGetUI()->tree);

		muiGetUI()->blocksCnt = cnt;
		muiGetUI()->blocksCurr = 0;
		muiGetUI()->blocks = (struct Rectangle*)malloc(cnt * sizeof(struct Rectangle));

		qudFillLeafs(muiGetUI()->tree, muiGetUI()->blocks, cnt);
	}

	if (pthread_mutex_init(&muiGetUI()->lock, NULL) != 0)
	{
		printf("\n mutex init failed\n");
		return NULL;
	}

	muiGetUI()->threads = (pthread_t*)malloc(sizeof(pthread_t));
	int *th = (int*)malloc(sizeof(int));

	for (register int i = 0; i < muiGetUI()->numCPU; i++)
	{
		th[i] = i;
		if (pthread_create(&muiGetUI()->threads[i], NULL, muiEngineWorker, &(th[i])))
		{
			fprintf(stderr, "Error creating thread\n");
			return NULL;
		}
	}

	for (register int i = 0; i < muiGetUI()->numCPU; i++)
	{
		if (pthread_join(muiGetUI()->threads[i], NULL)) 
		{

			fprintf(stderr, "Error joining thread\n");
			return NULL;
		}
	}

	//SAVE
	sceSaveOutput(muiGetUI()->scene, NULL);

	pthread_mutex_destroy(&muiGetUI()->lock);

	muiGetUI()->scene->rendered = 1;
	
	free(th);
	free(muiGetUI()->threads);
	free(muiGetUI()->blocks);
	muiGetUI()->blocksCnt = 0;
	muiGetUI()->cancel = 0;
}

int muiEngineRender(char* scenefile, char* output)
{
	char* filename = scenefile;
	char buffer[256];
	char* slash;

#ifdef _WIN32
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);

	muiGetUI()->numCPU = sysinfo.dwNumberOfProcessors;
#else
	numCPU = sysconf(_SC_NPROCESSORS_ONLN);
#endif

	if (filename == NULL) return 0;

	if (muiGetUI()->scene != NULL)
	{
		sceDestroy(muiGetUI()->scene);
	}

	muiGetUI()->scene = sceLoad(filename);

	if (muiGetUI()->scene == NULL) return IUP_DEFAULT;

	muiGetUI()->camera = sceGetCamera(muiGetUI()->scene);

	muiGetUI()->eye = camGetEye(muiGetUI()->camera);

	//setInitialCameraArgs(camera->up, camera->at, camera->eye);

	muiGetUI()->width = camGetScreenWidth(muiGetUI()->camera);
	muiGetUI()->height = camGetScreenHeight(muiGetUI()->camera);

	if (muiGetUI()->image) imageDestroy(muiGetUI()->image);
	muiGetUI()->image = imageCreate(muiGetUI()->width, muiGetUI()->height);	

	_ftime(&muiGetUI()->timebuffer);
	
	IupSetAttribute(muiGetUI()->timer, "RUN", "YES");

	if (pthread_create(&muiGetUI()->idx_renderthread, NULL, muiEngineSubdivision, NULL))
	{
		fprintf(stderr, "Error creating thread\n");
		return IUP_ERROR;
	}

	if (!muiGetUI()->hasUI)
	{
		if (pthread_join(muiGetUI()->idx_renderthread, NULL)) 
		{
			fprintf(stderr, "Error joining thread\n");
			return NULL;
		}
	}
}

int muiHandleCancelClick(void)
{
	muiGetUI()->cancel = 1;

	return IUP_DEFAULT;
}

int muiHandlePerformanceClick(void)
{
	IupMessagef("Performace", "TODO");

	return IUP_DEFAULT;
}

int muiHandleAboutClick(void)
{
	IupMessagef("About", "Developed by: Rodrigo Marques");

	return IUP_DEFAULT;
}

int muiHandlePassSelectItem(Ihandle *ih, char *text, int item, int state)
{
	muiGetUI()->passId = item;

	return IUP_DEFAULT;
}

int muiHandleLevelChange(Ihandle *ih, int c, char *new_value)
{
	sscanf(new_value, "%d", &muiGetUI()->quadViewLevel);

	return IUP_DEFAULT;
}

int muiHandleProgressToogle(Ihandle* ih, int state)
{
	muiGetUI()->showProgress = state;

	return IUP_DEFAULT;
}

int muiHandleSpinQuadTreeClick(Ihandle *ih, int pos)
{
	char *s = IupGetAttribute(muiGetUI()->quadlevel, "VALUE");
	
	sscanf(s, "%d", &muiGetUI()->quadViewLevel);

	return IUP_DEFAULT;
}

int muiHandleQuadTreeValueClick(Ihandle *ih)
{
	char *s = IupGetAttribute(muiGetUI()->quadlevel, "VALUE");

	sscanf(s, "%d", &muiGetUI()->quadViewLevel);

	return IUP_DEFAULT;
}