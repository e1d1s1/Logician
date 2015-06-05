#
# EDSEngineTestApp
#
# This is the *nix master makefile for the C++ EDSEngine library test application
set(SOURCE_FILES "stdafx.cpp"
		 "EDSEngineTestApp.cpp")

set(HEADER_FILES "stdafx.h"
		 "targetver.h")
		 
add_definitions(-DUNICODE -D_UNICODE -DNOPYTHON)

configure_file(test_project.gz test_project.gz COPYONLY)

add_executable(EDSEngineTestApp ${SOURCE_FILES} ${HEADER_FILES})

target_link_libraries(EDSEngineTestApp
						${Boost_LIBRARIES}
						EDSEngine)