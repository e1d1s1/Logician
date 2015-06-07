#
# DecisionLogic
#
# This is the *nix master makefile for the DecisionLogic application
set(SOURCE_FILES "stdafx.cpp"
		 "DecisionLogic.cpp"
		 "CodeControl.cpp"
		 "EditorPrefs.cpp"
		 "FindReplaceDialog.cpp"
		 "GUIClass.cpp"
		 "LogicTable.cpp"
		 "MDIChild.cpp"
		 "ProjectManager.cpp"
		 "utilities.cpp"
		 "WorkerClass.cpp"
		 "utilities.cpp")

set(HEADER_FILES "stdafx.h"
		 "resource.h"
		 "CodeControl.h"
		 "CodeEditor.h"
		 "CompileOptions.h"
		 "DataSet.h"
		 "DebugOptions.h"
		 "EditorPrefs.h"
		 "FindReplaceDialog.h"
		 "GUIClass.h"
		 "LogicTable.h"
		 "MDIChild.h"
		 "OpenLogicTable.h"
		 "ProjectManager.h"
		 "utilities.h"
		 "WorkerClass.h"
		 "utilities.h"
		 "StringTable.h")
		 
include("${wxWidgets_USE_FILE}")

include_directories(${LIBXML2_INCLUDE_DIR}
					${ICONV_INCLUDE_DIR})
		 
add_definitions(-DUNICODE -D_UNICODE)

add_executable(DecisionLogic ${SOURCE_FILES} ${HEADER_FILES})

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	set_target_properties(DecisionLogic PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
endif()

target_link_libraries(DecisionLogic						
						${wxWidgets_LIBRARIES}
						libxml2
						${Boost_LIBRARIES})
						
#configure_file(${LIBXML2_INCLUDE_DIR}/bin/libxml2.dll libxml2.dll COPYONLY)