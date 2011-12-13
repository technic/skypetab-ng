/*
stwindowcontainer.cpp
Author:
     Nikita Tsukanov <keks9n@gmail.com>

Copyright (c) 2011 Nikita Tsukanov

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#define protected public
#include <QWidget>
#undef protected
#include <QApplication>
#include "stwindowcontainer.h"
#include <QResizeEvent>
#include <QTimer>
#include <QString>
#include "focusguard.h"
#include "x11.h"
#include <stdio.h>
#include <X11/Xlib.h>
namespace skypetab
{
STWindowContainer::STWindowContainer(QWidget *parent) :
    QWidget(parent)
{
	_embedded=0;
	_inputArea=0;
	_destroyed=false;
	setFocusPolicy(Qt::StrongFocus);
	setMinimumSize(300, 400);
	setAcceptDrops(true);
}

void STWindowContainer::setInputFocus()
{
}

QWidget* FindChild(QWidget *widget, const char *className)
{
	if(strcmp(className, widget->metaObject()->className())==0)
		return widget;
	QObject*o;
	foreach(o, widget->children())
	{
		if(o->isWidgetType())
		{
			QWidget *ret=FindChild(qobject_cast<QWidget*>(o), className);
			if(ret!=0)
				return ret;
		}
	}
	return 0;
}

void STWindowContainer::embedWindow(QWidget *widgetToEmbed)
{
	_embedded=widgetToEmbed;
	XReparentWindow(X11::XDisplay, _embedded->winId(), winId(),0,0);
	X11::Flush();
	_embedded->show();

	_embeddedAt=time(0);
	connect(widgetToEmbed, SIGNAL(destroyed(QObject*)), this, SLOT(onDestroyed(QObject*)));
	QTimer::singleShot(40, this, SLOT(delayedEmbed()));

	FocusGuard::addGuardedWidget(widgetToEmbed);

	_inputArea=FindChild(_embedded, "ChatInput");
}

void STWindowContainer::delayedEmbed()
{
	XReparentWindow(X11::XDisplay, _embedded->winId(), winId(),0,0);
	_embedded->show();
	_embedded->resize(size());

}

void STWindowContainer::resizeEvent(QResizeEvent *event)
{
	if((_embedded==0)||(_destroyed))
		return;
	_embedded->resize(event->size());
	_embedded->show();
}

QString STWindowContainer::getWindowTitle()
{
	if((_embedded==0)||(_destroyed))
		return "DESTROYED";
	return X11::GetWindowName(_embedded->winId());
}

QString STWindowContainer::getShortWindowTitle(int maxSize)
{
	QString title=getWindowTitle();
	if(title.length()>maxSize)
	{
		title=title.left(maxSize-3).append("...");
	}
	return title;
}

bool STWindowContainer::isDestroyed()
{
	return _destroyed;
}

bool STWindowContainer::isAvailable()
{
	if(_destroyed)
		return false;
	if(time(0)-2<_embeddedAt)
		return true;
	return _embedded->isVisible();
}

QWidget *STWindowContainer::widget()
{
	return _embedded;
}

QWidget *STWindowContainer::inputArea()
{
	return _inputArea;
}

void STWindowContainer::focusInEvent(QFocusEvent *)
{

	if((_embedded==0)||(_destroyed))
		return;
	XSetInputFocus(X11::XDisplay, _embedded->winId(), RevertToNone, CurrentTime);
}

void STWindowContainer::dragEnterEvent(QDragEnterEvent *ev)
{
	ev->acceptProposedAction();
}


void STWindowContainer::dropEvent(QDropEvent *ev)
{
	_inputArea->dropEvent(ev);
}

void STWindowContainer::onDestroyed(QObject *)
{
	_destroyed=true;
}



}
