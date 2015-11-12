#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <GL/glew.h>
#include <GL/glxew.h>
#include <GL/glx.h>
#include <GL/gl.h>
#include <GL/glu.h>

static int dblBuf[] =  {GLX_RGBA, GLX_RED_SIZE, 1, GLX_GREEN_SIZE, 1, GLX_BLUE_SIZE, 1, GLX_DEPTH_SIZE, 12, GLX_DOUBLEBUFFER, None};

int main(int argc, char **argv)
{
	int x = 10;
	int y = 10;
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
	while (1)
	{
		XNextEvent(dpy, &event);
		switch (event.type)
		{
      		case ConfigureNotify:
			glViewport(0, 0, event.xconfigure.width, event.xconfigure.height);
			glLoadIdentity();
			gluOrtho2D(0.0,(GLdouble)event.xconfigure.width,0.0,(GLdouble)event.xconfigure.height);
			break;
		case KeyPress:
			XLookupString(&event.xkey,buf,100,NULL,NULL);
			//printf("keypress %s\n",buf);
			switch(XLookupKeysym (&event.xkey, 0))
			{
			case XK_q:
				XCloseDisplay(dpy);
				exit(0);
				break;
			case XK_Left:
				x++;
				break;
			case XK_Right:
				x--;
				break;
			case XK_Up:
				y++;
				break;
			case XK_Down:
				y--;
				break;
			}
			/*if (XK_q == XLookupKeysym (&event.xkey, 0))
			{
				XCloseDisplay(dpy);
				exit(0);
			}*/
			//break;
		case Expose:
			glClear(GL_COLOR_BUFFER_BIT);
			glRectf(x,y,x+10,y+10);
			glEnd();
			glXSwapBuffers(dpy, win);
			glFlush();          
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
