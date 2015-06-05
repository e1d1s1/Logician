#
# ROMAppConsoleTest
#
# This is the *nix master makefile for the C++ Relation Object Model library test application
set(SOURCE_FILES "stdafx.cpp"
		 "ROMAppConsoleTest.cpp")

set(HEADER_FILES "stdafx.h")
		 
add_definitions(-DUNICODE -D_UNICODE -DNOPYTHON)

add_executable(ROMAppConsoleTest ${SOURCE_FILES} ${HEADER_FILES})

target_link_libraries(ROMAppConsoleTest						
						EDSEngine
						RelationalObjectModel)