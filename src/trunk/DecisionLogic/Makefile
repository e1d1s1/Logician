#############################################################################
# Makefile for building: DecisionLogic
# Generated by qmake (2.01a) (Qt 4.8.3) on: Sun Mar 10 15:11:52 2013
# Project:  DecisionLogic.pro
# Template: app
# Command: /usr/bin/qmake-qt4 -spec /usr/share/qt4/mkspecs/linux-g++-64 CONFIG+=debug CONFIG+=declarative_debug -o Makefile DecisionLogic.pro
#############################################################################

####### Compiler, tools and options

CC            = gcc
CXX           = g++
DEFINES       = -DQT_WEBKIT -DPOSIX -D__UNIX__ -D__WXGTK__
CFLAGS        = -m64 -pipe -g -Wall -W $(DEFINES)
CXXFLAGS      = -m64 -pipe -g -Wall -W $(DEFINES)
INCPATH       = -I/usr/share/qt4/mkspecs/linux-g++-64 -I. -I/usr/include/libxml2 -I/usr/include/wx-2.8 -I/usr/lib/x86_64-linux-gnu/wx/include/gtk2-unicode-release-2.8 -I/usr/lib/i386-linux-gnu/wx/include/gtk2-unicode-release-2.8
LINK          = g++
LFLAGS        = -m64
LIBS          = $(SUBLIBS)   -lxml2 -lstdc++ -lwx_baseu-2.8 -lwx_gtk2u_adv-2.8 -lwx_gtk2u_html-2.8 -lwx_baseu_xml-2.8 -lwx_gtk2u_stc-2.8 -lwx_gtk2u_core-2.8 -lwx_baseu_net-2.8 -lboost_system -lboost_iostreams -lboost_filesystem 
AR            = ar cqs
RANLIB        = 
QMAKE         = /usr/bin/qmake-qt4
TAR           = tar -cf
COMPRESS      = gzip -9f
COPY          = cp -f
SED           = sed
COPY_FILE     = $(COPY)
COPY_DIR      = $(COPY) -r
STRIP         = strip
INSTALL_FILE  = install -m 644 -p
INSTALL_DIR   = $(COPY_DIR)
INSTALL_PROGRAM = install -m 755 -p
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p

####### Output directory

OBJECTS_DIR   = ./

####### Files

SOURCES       = WorkerClass.cpp \
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
OBJECTS       = WorkerClass.o \
		utilities.o \
		stdafx.o \
		ProjectManager.o \
		MDIChild.o \
		LogicTable.o \
		GUIClass.o \
		FindReplaceDialog.o \
		EditorPrefs.o \
		DecisionLogic.o \
		CodeControl.o
DIST          = /usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/common/gcc-base.conf \
		/usr/share/qt4/mkspecs/common/gcc-base-unix.conf \
		/usr/share/qt4/mkspecs/common/g++-base.conf \
		/usr/share/qt4/mkspecs/common/g++-unix.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/modules/qt_webkit_version.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		/usr/share/qt4/mkspecs/features/debug.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/declarative_debug.prf \
		/usr/share/qt4/mkspecs/features/unix/gdb_dwarf_index.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf \
		/usr/share/qt4/mkspecs/features/yacc.prf \
		/usr/share/qt4/mkspecs/features/lex.prf \
		/usr/share/qt4/mkspecs/features/include_source_dir.prf \
		DecisionLogic.pro
QMAKE_TARGET  = DecisionLogic
DESTDIR       = 
TARGET        = DecisionLogic

first: all
####### Implicit rules

.SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o "$@" "$<"

####### Build rules

all: Makefile $(TARGET)

$(TARGET):  $(OBJECTS)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)
	{ test -n "$(DESTDIR)" && DESTDIR="$(DESTDIR)" || DESTDIR=.; } && test $$(gdb --version | sed -e 's,[^0-9]\+\([0-9]\)\.\([0-9]\).*,\1\2,;q') -gt 72 && gdb --nx --batch --quiet -ex 'set confirm off' -ex "save gdb-index $$DESTDIR" -ex quit '$(TARGET)' && test -f $(TARGET).gdb-index && objcopy --add-section '.gdb_index=$(TARGET).gdb-index' --set-section-flags '.gdb_index=readonly' '$(TARGET)' '$(TARGET)' && rm -f $(TARGET).gdb-index || true

Makefile: DecisionLogic.pro  /usr/share/qt4/mkspecs/linux-g++-64/qmake.conf /usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/common/gcc-base.conf \
		/usr/share/qt4/mkspecs/common/gcc-base-unix.conf \
		/usr/share/qt4/mkspecs/common/g++-base.conf \
		/usr/share/qt4/mkspecs/common/g++-unix.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/modules/qt_webkit_version.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		/usr/share/qt4/mkspecs/features/debug.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/declarative_debug.prf \
		/usr/share/qt4/mkspecs/features/unix/gdb_dwarf_index.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf \
		/usr/share/qt4/mkspecs/features/yacc.prf \
		/usr/share/qt4/mkspecs/features/lex.prf \
		/usr/share/qt4/mkspecs/features/include_source_dir.prf
	$(QMAKE) -spec /usr/share/qt4/mkspecs/linux-g++-64 CONFIG+=debug CONFIG+=declarative_debug -o Makefile DecisionLogic.pro
/usr/share/qt4/mkspecs/common/unix.conf:
/usr/share/qt4/mkspecs/common/linux.conf:
/usr/share/qt4/mkspecs/common/gcc-base.conf:
/usr/share/qt4/mkspecs/common/gcc-base-unix.conf:
/usr/share/qt4/mkspecs/common/g++-base.conf:
/usr/share/qt4/mkspecs/common/g++-unix.conf:
/usr/share/qt4/mkspecs/qconfig.pri:
/usr/share/qt4/mkspecs/modules/qt_webkit_version.pri:
/usr/share/qt4/mkspecs/features/qt_functions.prf:
/usr/share/qt4/mkspecs/features/qt_config.prf:
/usr/share/qt4/mkspecs/features/exclusive_builds.prf:
/usr/share/qt4/mkspecs/features/default_pre.prf:
/usr/share/qt4/mkspecs/features/debug.prf:
/usr/share/qt4/mkspecs/features/default_post.prf:
/usr/share/qt4/mkspecs/features/declarative_debug.prf:
/usr/share/qt4/mkspecs/features/unix/gdb_dwarf_index.prf:
/usr/share/qt4/mkspecs/features/warn_on.prf:
/usr/share/qt4/mkspecs/features/resources.prf:
/usr/share/qt4/mkspecs/features/uic.prf:
/usr/share/qt4/mkspecs/features/yacc.prf:
/usr/share/qt4/mkspecs/features/lex.prf:
/usr/share/qt4/mkspecs/features/include_source_dir.prf:
qmake:  FORCE
	@$(QMAKE) -spec /usr/share/qt4/mkspecs/linux-g++-64 CONFIG+=debug CONFIG+=declarative_debug -o Makefile DecisionLogic.pro

dist: 
	@$(CHK_DIR_EXISTS) .tmp/DecisionLogic1.0.0 || $(MKDIR) .tmp/DecisionLogic1.0.0 
	$(COPY_FILE) --parents $(SOURCES) $(DIST) .tmp/DecisionLogic1.0.0/ && (cd `dirname .tmp/DecisionLogic1.0.0` && $(TAR) DecisionLogic1.0.0.tar DecisionLogic1.0.0 && $(COMPRESS) DecisionLogic1.0.0.tar) && $(MOVE) `dirname .tmp/DecisionLogic1.0.0`/DecisionLogic1.0.0.tar.gz . && $(DEL_FILE) -r .tmp/DecisionLogic1.0.0


clean:compiler_clean 
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(TARGET) 
	-$(DEL_FILE) Makefile


check: first

compiler_rcc_make_all:
compiler_rcc_clean:
compiler_uic_make_all:
compiler_uic_clean:
compiler_image_collection_make_all: qmake_image_collection.cpp
compiler_image_collection_clean:
	-$(DEL_FILE) qmake_image_collection.cpp
compiler_yacc_decl_make_all:
compiler_yacc_decl_clean:
compiler_yacc_impl_make_all:
compiler_yacc_impl_clean:
compiler_lex_make_all:
compiler_lex_clean:
compiler_clean: 

####### Compile

WorkerClass.o: WorkerClass.cpp stdafx.h \
		WorkerClass.h \
		ProjectManager.h \
		StringTable.h \
		DataSet.h \
		LogicTable.h \
		MDIChild.h \
		OpenLogicTable.h \
		CodeEditor.h \
		CodeControl.h \
		EditorPrefs.h \
		utilities.h \
		DebugOptions.h \
		CompileOptions.h \
		GUIClass.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o WorkerClass.o WorkerClass.cpp

utilities.o: utilities.cpp stdafx.h \
		utilities.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o utilities.o utilities.cpp

stdafx.o: stdafx.cpp stdafx.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o stdafx.o stdafx.cpp

ProjectManager.o: ProjectManager.cpp stdafx.h \
		ProjectManager.h \
		StringTable.h \
		DataSet.h \
		utilities.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o ProjectManager.o ProjectManager.cpp

MDIChild.o: MDIChild.cpp stdafx.h \
		MDIChild.h \
		ProjectManager.h \
		StringTable.h \
		DataSet.h \
		LogicTable.h \
		OpenLogicTable.h \
		CodeEditor.h \
		CodeControl.h \
		EditorPrefs.h \
		utilities.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o MDIChild.o MDIChild.cpp

LogicTable.o: LogicTable.cpp stdafx.h \
		LogicTable.h \
		DataSet.h \
		StringTable.h \
		utilities.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o LogicTable.o LogicTable.cpp

GUIClass.o: GUIClass.cpp stdafx.h \
		GUIClass.h \
		LogicTable.h \
		DataSet.h \
		StringTable.h \
		OpenLogicTable.h \
		ProjectManager.h \
		DebugOptions.h \
		utilities.h \
		MDIChild.h \
		CodeEditor.h \
		CodeControl.h \
		EditorPrefs.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o GUIClass.o GUIClass.cpp

FindReplaceDialog.o: FindReplaceDialog.cpp FindReplaceDialog.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o FindReplaceDialog.o FindReplaceDialog.cpp

EditorPrefs.o: EditorPrefs.cpp EditorPrefs.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o EditorPrefs.o EditorPrefs.cpp

DecisionLogic.o: DecisionLogic.cpp stdafx.h \
		DecisionLogic.h \
		WorkerClass.h \
		ProjectManager.h \
		StringTable.h \
		DataSet.h \
		LogicTable.h \
		MDIChild.h \
		OpenLogicTable.h \
		CodeEditor.h \
		CodeControl.h \
		EditorPrefs.h \
		utilities.h \
		DebugOptions.h \
		CompileOptions.h \
		GUIClass.h \
		FindReplaceDialog.h \
		file.xpm \
		folder.xpm \
		folder_open.xpm \
		DecisionLogicIcon.xpm
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o DecisionLogic.o DecisionLogic.cpp

CodeControl.o: CodeControl.cpp stdafx.h \
		CodeControl.h \
		EditorPrefs.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o CodeControl.o CodeControl.cpp

####### Install

install:   FORCE

uninstall:   FORCE

FORCE:

