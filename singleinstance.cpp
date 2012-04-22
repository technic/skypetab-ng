#include "singleinstance.h"
#include "skypetab.h"
#include "mainwindow.h"
#include "x11.h"
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#define WIN_MSG "Activate"

namespace skypetab
{

WId FindXWindow(WId parent)
{
	WId root, *children;
	unsigned int nchildren;
	Atom WM_CLASS = XInternAtom(X11::XDisplay, "WM_CLASS",0);
	XQueryTree(X11::XDisplay, parent, &root, &parent,&children, &nchildren);
	WId ret=0;
	for(unsigned int c=0; c<nchildren; c++)
	{
		WId win=children[c];

		unsigned long bytes_after, len;
		unsigned char *value;
		int format;
		Atom actual_type;
		if(XGetWindowProperty(X11::XDisplay,win,WM_CLASS,0,1024,False,XA_STRING, &actual_type, &format,
						   &len,&bytes_after,&value)==Success)
		{
			char*val=new char[len+1];
			memcpy(val, value, len);
			val[len]=0;
			XFree(value);
			if(0==strcmp(val, "SkypeTab"))
			{
				ret=win;
				break;
			}
			ret=FindXWindow(win);
			if(ret!=0)
				break;
		}
	}
	XFree(children);
	return ret;
}

bool skypetab::SingleInstance::activatePreviousInstance()
{
	//Make sure that we have X11 display handle
	QWidget* w=new QWidget();
	w->winId();
	delete w;
	WId inst=FindXWindow(XRootWindow(X11::XDisplay,0));
	if(!inst)
		return false;
	XEvent ev;
	memset(&ev, 0, sizeof(ev));
    ev.xclient.type = ClientMessage;
    ev.xclient.window = inst;
    ev.xclient.message_type = XA_STRING;
    ev.xclient.format = 8;
	strcpy(ev.xclient.data.b, WIN_MSG);
	int ret=XSendEvent(X11::XDisplay, inst, 0, NoEventMask, &ev);
	printf("XSendEvent: %i", ret);
	XSync(X11::XDisplay, False);
	return true;
}

bool STabMainWindow::x11Event(XEvent *ev)
{
	if(ev->type==ClientMessage)
	{
		if(0==memcmp(ev->xclient.data.b, WIN_MSG, strlen(WIN_MSG)))
		{
			SkypeTab::_instance->mainWindow->show();
			SkypeTab::_instance->mainWindow->activateWindow();
		}
	}
	return QWidget::x11Event(ev);
}
}
