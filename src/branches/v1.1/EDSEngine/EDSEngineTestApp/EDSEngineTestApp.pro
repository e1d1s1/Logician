TEMPLATE = app
CONFIG += console
CONFIG -= qt

DEFINES += EDSENGINETESTAPP \
        USE_LIBXML \
        USE_JAVASCRIPT \
        USE_PYTHON \
        POSIX

SOURCES += \
    stdafx.cpp \
    EDSEngineTestApp.cpp

HEADERS += \
    stdafx.h


INCLUDEPATH += $$PWD/../ \
               /usr/include/libxml2 \
               /usr/include/js \
               /usr/include/python2.7

DEPENDPATH += $$PWD/../bin/Debug \
              /usr/lib/i386-linux-gnu

unix:!symbian: LIBS += -L$$PWD/../ -lEDSEngine

INCLUDEPATH += $$PWD/../
DEPENDPATH += $$PWD/../
