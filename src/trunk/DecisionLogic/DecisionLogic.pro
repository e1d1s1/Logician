TEMPLATE = app
CONFIG += console
CONFIG -= qt

DEFINES += POSIX \
           __UNIX__ \
           __WXGTK__

INCLUDEPATH += /usr/include/libxml2 \
               /usr/include/wx-2.8 \
               /usr/lib/x86_64-linux-gnu/wx/include/gtk2-unicode-release-2.8 \
               /usr/lib/i386-linux-gnu/wx/include/gtk2-unicode-release-2.8

SOURCES += \
    WorkerClass.cpp \
    utilities.cpp \
    stdafx.cpp \
    ProjectManager.cpp \
    MDIChild.cpp \
    LogicTable.cpp \
    GUIClass.cpp \
    FindReplaceDialog.cpp \
    EditorPrefs.cpp \
    DecisionLogic.cpp \
    CodeControl.cpp

HEADERS += \
    WorkerClass.h \
    utilities.h \
    StringTable.h \
    stdafx.h \
    resource.h \
    ProjectManager.h \
    OpenLogicTable.h \
    MDIChild.h \
    LogicTable.h \
    GUIClass.h \
    FindReplaceDialog.h \
    EditorPrefs.h \
    DecisionLogic.h \
    DebugOptions.h \
    DataSet.h \
    CompileOptions.h \
    CodeEditor.h \
    CodeControl.h

DEPENDPATH += /usr/lib \
              /usr/lib/i386-linux-gnu \
              /usr/lib/x86_64-linux-gnu

unix:!symbian: LIBS += -lxml2
unix:!symbian: LIBS += -lstdc++
unix:!symbian: LIBS += -lwx_baseu-2.8
unix:!symbian: LIBS += -lwx_gtk2u_adv-2.8
unix:!symbian: LIBS += -lwx_gtk2u_html-2.8
unix:!symbian: LIBS += -lwx_baseu_xml-2.8
unix:!symbian: LIBS += -lwx_gtk2u_stc-2.8
unix:!symbian: LIBS += -lwx_gtk2u_core-2.8
unix:!symbian: LIBS += -lwx_baseu_net-2.8
unix:!symbian: LIBS += -lboost_system
unix:!symbian: LIBS += -lboost_iostreams
unix:!symbian: LIBS += -lboost_filesystem

