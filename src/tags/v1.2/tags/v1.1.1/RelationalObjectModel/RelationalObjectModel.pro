#-------------------------------------------------
#
# Project created by QtCreator 2012-07-08T02:29:47
#
#-------------------------------------------------

QT       -= core gui

TARGET = RelationalObjectModel
TEMPLATE = lib

DEFINES += RELATIONALOBJECTMODEL_LIBRARY \
           USE_LIBXML \
           POSIX

INCLUDEPATH += /usr/include/libxml2 \
               ../EDSEngine

SOURCES += \
    utilities.cpp \
    stdafx.cpp \
    ROMNode.cpp \
    ROMDictionary.cpp \
    LinearEngine.cpp

HEADERS += \
    utilities.h \
    stdafx.h \
    ROMNode.h \
    ROMDictionaryAttribute.h \
    ROMDictionary.h \
    LinearEngine.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE3F70F56
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = RelationalObjectModel.dll
    addFiles.path = !:/sys/bin
    DEPLOYMENT += addFiles
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

unix:!symbian: LIBS += -lxml2

unix:!symbian: LIBS += -lxslt

symbian: LIBS += -lEDSEngine
else:unix:!macx: LIBS += -L../EDSEngine/ -lEDSEngine

INCLUDEPATH += ../EDSEngine
DEPENDPATH += ../EDSEngine
