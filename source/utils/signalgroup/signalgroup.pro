TEMPLATE    = lib
CONFIG      += staticlib
QT          -= gui
QT          += network

SOURCES     += signalgroup.cpp
SOURCES     += connection/connection.cpp

HEADERS     += signalgroup.h
HEADERS     += connection/connection.h

INCLUDEPATH += ../../../externals/include/ \

LIBS        += -L$$PWD/../../../externals/lib/ -lqamqp

DESTDIR     = ../../../build/lib/
