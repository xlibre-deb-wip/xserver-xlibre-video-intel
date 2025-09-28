#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "test.h"

const char *test_target_name(enum target target)
{
	switch (target) {
	default:
	case ROOT: return "root";
	case CHILD: return "child";
	case PIXMAP: return "pixmap";
	}
}

void test_target_create_render(struct test_display *dpy,
			       enum target target,
			       struct test_target *tt)
{
	XSetWindowAttributes attr;
	XGCValues gcv;

	tt->dpy = dpy;
	tt->target = target;

	tt->draw = dpy->root;
	tt->format = dpy->format;
	tt->width = dpy->width;
	tt->height = dpy->height;
	tt->depth = dpy->depth;

	switch (target) {
	case ROOT:
		break;

	case CHILD:
		attr.override_redirect = 1;
		tt->width /= 4;
		tt->height /= 4;
		tt->draw = XCreateWindow(dpy->dpy, tt->draw,
					 dpy->width/2, dpy->height/2,
					 tt->width, tt->height,
					 0, tt->format->depth,
					 InputOutput,
					 DefaultVisual(dpy->dpy,
						       DefaultScreen(dpy->dpy)),
					 CWOverrideRedirect, &attr);
		XMapWindow(dpy->dpy, tt->draw);
		break;

	case PIXMAP:
		tt->format = XRenderFindStandardFormat(dpy->dpy, PictStandardARGB32);
		tt->draw = XCreatePixmap(dpy->dpy, tt->draw,
					 dpy->width, dpy->height,
					 tt->format->depth);
		tt->depth = 32;
		break;
	}

	tt->picture =
		XRenderCreatePicture(dpy->dpy, tt->draw, tt->format, 0, NULL);

	gcv.graphics_exposures = 0;
	tt->gc = XCreateGC(dpy->dpy, tt->draw, GCGraphicsExposures, &gcv);
}

void test_target_destroy_render(struct test_display *dpy,
				struct test_target *tt)
{
	XRenderFreePicture(dpy->dpy, tt->picture);
	switch (tt->target) {
	case ROOT:
		break;
	case CHILD:
		XDestroyWindow(dpy->dpy, tt->draw);
		break;
	case PIXMAP:
		XFreePixmap(dpy->dpy, tt->draw);
		break;
	}
}
