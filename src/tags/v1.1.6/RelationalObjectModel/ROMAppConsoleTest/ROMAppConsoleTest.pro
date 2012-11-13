TEMPLATE = app
CONFIG += console
CONFIG -= qt

DEFINES += USE_LIBXML \
           POSIX

SOURCES += \
    stdafx.cpp \
    ROMAppConsoleTest.cpp

HEADERS += \
    stdafx.h


unix:!symbian: LIBS += -lxml2

unix:!symbian: LIBS += -lxslt

symbian: LIBS += -lEDSEngine
else:unix:!macx: LIBS += -L../../EDSEngine/ -lEDSEngine

INCLUDEPATH += ../../EDSEngine \
               /usr/include/libxml2
DEPENDPATH += ../../EDSEngine

symbian: LIBS += -lRelationalObjectModel
else:unix:!macx: LIBS += -L../ -lRelationalObjectModel

INCLUDEPATH += ../
DEPENDPATH += ../
