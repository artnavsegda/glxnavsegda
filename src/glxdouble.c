#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <GL/glew.h>
#include <GL/glxew.h>
#include <GL/glx.h>
#include <GL/gl.h>
#include "font.h"

int main(int argc, char *argv[])
{
	int x = 10;
	int y = 10;
	char buf[100];
	static GLfloat spin = 0.0;
	int dblBuf[] =  {GLX_RGBA, GLX_RED_SIZE, 1, GLX_GREEN_SIZE, 1, GLX_BLUE_SIZE, 1, GLX_DEPTH_SIZE, 12, GLX_DOUBLEBUFFER, None};
	XSetWindowAttributes swa;
	XEvent event;
	Display *dpy = XOpenDisplay(NULL);
	XVisualInfo *vi = glXChooseVisual(dpy, DefaultScreen(dpy), dblBuf);
	GLXContext cx = glXCreateContext(dpy, vi, None, True);
	Colormap cmap = XCreateColormap(dpy, RootWindow(dpy, vi->screen), vi->visual, AllocNone);
	swa.colormap = cmap;
	swa.border_pixel = 0;
	swa.event_mask = StructureNotifyMask;
	Window win = XCreateWindow(dpy, RootWindow(dpy, vi->screen), 0, 0, 300, 300, 0, vi->depth, InputOutput, vi->visual, CWBorderPixel | CWColormap | CWEventMask, &swa);
	XSetStandardProperties(dpy, win, "glxsimple", "glxsimple", None, argv, argc, NULL);
	glXMakeCurrent(dpy, win, cx);
	XSelectInput(dpy, win, KeyPressMask | StructureNotifyMask);
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
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	GLuint fontOffset = glGenLists(128);
	for (GLuint i = 32; i < 127; i++)
	{
		glNewList(i+fontOffset, GL_COMPILE);
		glBitmap(8,13,0.0,2.0,10.0,0.0,font[i-32]);
		glEndList();
	}
	glListBase(fontOffset);
	while (1)
	{
		while (XPending(dpy))
		{
			XNextEvent(dpy, &event);
			switch (event.type)
			{
	      		case ConfigureNotify:
				glViewport(0, 0, event.xconfigure.width, event.xconfigure.height);
				glLoadIdentity();
				//gluOrtho2D(0.0,(GLdouble)event.xconfigure.width,0.0,(GLdouble)event.xconfigure.height);
				gluOrtho2D((GLdouble)event.xconfigure.width/-2,(GLdouble)event.xconfigure.width/2,(GLdouble)event.xconfigure.height/-2,(GLdouble)event.xconfigure.height/2);
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
					glTranslatef(-1.0,0.0,0.0);
					break;
				case XK_Right:
					x--;
					glTranslatef(1.0,0.0,0.0);
					break;
				case XK_Up:
					y++;
					glTranslatef(0.0,1.0,0.0);
					break;
				case XK_Down:
					y--;
					glTranslatef(0.0,-1.0,0.0);
					break;
				}
				break;
			case ClientMessage:
				XCloseDisplay(dpy);
				exit(0);
				break;
			}
		}
		spin = spin + 1.0;
		if (spin > 360.0)
			spin = spin - 360.0;

		glClear(GL_COLOR_BUFFER_BIT);
		//glRotatef(1.0,0.0,0.0,1.0);
		glPushMatrix();
		//glLoadIdentity();
		glRotatef(spin,0.0,0.0,1.0);
		/*glRasterPos2f(-0.5,-0.5);
		glCallLists(3, GL_UNSIGNED_BYTE,(GLubyte *)"one");
		glRasterPos2f(-0.5,0.5);
		glCallLists(3, GL_UNSIGNED_BYTE,(GLubyte *)"day");
		glRasterPos2f(0.5,0.5);
		glCallLists(7, GL_UNSIGNED_BYTE,(GLubyte *)"it goes");
		glRasterPos2f(0.5,-0.5);
		glCallLists(4, GL_UNSIGNED_BYTE,(GLubyte *)"away");*/

		//glRectf(-0.5,0.5,0.5,-0.5);
		glRectf(-75,75,75,-75);
		/*glBegin(GL_LINE_LOOP);
			glVertex2f(-0.5,-0.5);
			glVertex2f(-0.5,0.5);
			glVertex2f(0.5,0.5);
			glVertex2f(0.5,-0.5);
		glEnd();*/
		glPopMatrix();

		glXSwapBuffers(dpy, win);
		glFlush();          
	}
	return 0;
}
