#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <GL/glew.h>
#include <GL/glxew.h>
#include <GL/glx.h>
#include <GL/gl.h>
#include <GL/glu.h>
#define MAXLENGTH 900000
int m[20][901000];
int max[20];
int min[20];
int l;

static int dblBuf[] =  {GLX_RGBA, GLX_RED_SIZE, 1, GLX_GREEN_SIZE, 1, GLX_BLUE_SIZE, 1, GLX_DEPTH_SIZE, 12, GLX_DOUBLEBUFFER, None};

int developmassive()
{
	l = 0;
	FILE *sora;
	sora = fopen("./chota.txt","r");
	if (sora == NULL)
	{
		printf("fucking error");
		exit(0);
	}
	memset(max,0,sizeof(max));
	memset(max,0,sizeof(min));
	while (fscanf(sora,"%d %d %d %d %d %d %d %d %d %d %d %d %d\n", &m[1][l],&m[2][l],&m[3][l],&m[4][l],&m[5][l],&m[6][l],&m[7][l],&m[8][l],&m[9][l],&m[10][l],&m[11][l],&m[12][l],&m[13][l])!=EOF)
	{
		for(int i=1;i!=12;i++)
		{
			if (min[i]==0) min[i]=m[i][l];
			if (max[i]==0) max[i]=m[i][l];
			if (m[i][l]<min[i]) min[i]=m[i][l];
			if (m[i][l]>max[i]) max[i]=m[i][l];
		}
		if (l++ > MAXLENGTH)
			break;
	}
	printf("length is %d\n",l);
	for(int i=1;i!=12;i++)
		printf("%d: min %d, max %d, span %d\n",i,min[i],max[i],max[i]-min[i]);
	return 0;
}

int main(int argc, char **argv)
{
	float xspan = 0.0;
	int xwidth, yheight;
	double xscale = 1.0;
	int x = 0;
	int y = 0;
	char buf[100];
	Display *dpy;
	Window win;
	XVisualInfo *vi;
	Colormap cmap;
	XSetWindowAttributes swa;
	GLXContext cx;
	XEvent event;
	Bool needRedraw = False, recalcModelView = True;
	dpy = XOpenDisplay(NULL);
	vi = glXChooseVisual(dpy, DefaultScreen(dpy), dblBuf);
	cx = glXCreateContext(dpy, vi, None, True);
	cmap = XCreateColormap(dpy, RootWindow(dpy, vi->screen), vi->visual, AllocNone);
	swa.colormap = cmap;
	swa.border_pixel = 0;
	swa.event_mask = ExposureMask | ButtonPressMask | StructureNotifyMask;
	win = XCreateWindow(dpy, RootWindow(dpy, vi->screen), 0, 0, 300, 300, 0, vi->depth, InputOutput, vi->visual, CWBorderPixel | CWColormap | CWEventMask, &swa);
	XSetStandardProperties(dpy, win, "glxsimple", "glxsimple", None, argv, argc, NULL);
	glXMakeCurrent(dpy, win, cx);
	XSelectInput(dpy, win, ExposureMask | KeyPressMask | ButtonPressMask | StructureNotifyMask);
	XMapWindow(dpy, win);
	Atom WM_DELETE_WINDOW = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(dpy, win, &WM_DELETE_WINDOW, 1);
	GLenum err = glewInit();
	if (GLEW_OK != err)
		printf("Error: %s\n", glewGetErrorString(err));
	err = glXSwapIntervalMESA(1);
	if (err == GLX_BAD_CONTEXT)
		printf("bad context\n");
	//printf("swap interval is %d\n", glXGetSwapIntervalMESA());
	developmassive();
	while (1)
	{
		XNextEvent(dpy, &event);
		switch (event.type)
		{
      		case ConfigureNotify:
			xwidth = event.xconfigure.width;
			yheight = event.xconfigure.height;
			printf("width %d, height %d\n",xwidth,yheight);
			printf("scale by x is %f\n",(double)xwidth/l);
			glViewport(0, 0, event.xconfigure.width, event.xconfigure.height);
			glLoadIdentity();
			gluOrtho2D(0.0,(GLdouble)event.xconfigure.width,0.0,(GLdouble)event.xconfigure.height);
			break;
		case KeyPress:
			XLookupString(&event.xkey,buf,100,NULL,NULL);
			//printf("keypress %s\n",buf);
			switch(XLookupKeysym (&event.xkey, 0))
			{
			/*case XK_1:
				xscale = 1.0;
				break;
			case XK_2:
				xscale = (double)xwidth/l;
				break;*/
			case XK_2:
				if (xscale/2.0>(double)xwidth/l)
					xscale = xscale/2.0;
				else
					xscale = (double)xwidth/l;
				break;
			case XK_1:
				xscale = xscale*2.0;
				break;
			case XK_q:
				XCloseDisplay(dpy);
				exit(0);
				break;
			case XK_Left:
				//xspan++;
				if ((   (l-xspan)   *xscale)     > xwidth)
					xspan=xspan+(1.0/xscale);
				//glTranslatef(-10.0,0.0,0.0);
				break;
			case XK_Right:
				//xspan--;
				if (xspan-(1.0/xscale)>=0)
					xspan=xspan-(1.0/xscale);
				//glTranslatef(10.0,0.0,0.0);
				break;
			case XK_Up:
				//y++;
				glTranslatef(0.0,10.0,0.0);
				break;
			case XK_Down:
				//y--;
				glTranslatef(0.0,-10.0,0.0);
				break;
			}
			//break;
		case Expose:
			glClear(GL_COLOR_BUFFER_BIT);
			//scale matrix
			glPushMatrix();
			//glScalef(0.5,1.0,1.0);
			//glScalef((float)xwidth/l,1.0,1.0);
			glScalef(xscale,1.0,1.0);
			//glLoadIdentity();
			//glRectf(x,y,x+10,y+10);
			glBegin(GL_LINE_STRIP);
			//draw screen width
			//for(int i=0;i<xwidth;i++)
			//draw screen width by current scale
			for(int i=0;i<xwidth/xscale;i++)
			//draw everything
			//for(int i=0;i<l;i++)
			{
				glVertex2i(i,m[5][i+(int)xspan]);
				if (i+xspan>l)
					break;
			}
			glEnd();
			//glRectf(10,10,20,20);
			//glRectf(0.5,0.5,0.7,0.7);
			glPopMatrix();
			//glEnd();
			glXSwapBuffers(dpy, win);
			//glFlush();          
			break;
		case ClientMessage:
			XCloseDisplay(dpy);
			exit(0);
			break;
		case ButtonPress:
			break;
		}
	}
	return 0;
}
