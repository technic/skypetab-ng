#-------------------------------------------------
#
# Project created by QtCreator 2012-11-01T15:54:46
#
#-------------------------------------------------

QT       += core gui

LIBS+= -lX11

TARGET = skypetab-prelaunch
TEMPLATE = app


SOURCES += main.cpp \
    ../x11.cpp

HEADERS  +=

unix:
{
   target.path = /usr/bin
   INSTALLS += target
}
