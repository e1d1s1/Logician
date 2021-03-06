#
# Logician Suite
#
# This is the *nix master makefile for the C++ components of the project


#
# Make sure there is a reasonable version of CMAKE installed.
#

cmake_minimum_required(VERSION 2.6)

#
# Define the base solution name.
#

project("LogicianSuite")

if(CMAKE_CONFIGURATION_TYPES)
        set(CMAKE_CONFIGURATION_TYPES Debug Release)
        set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING
         "Reset the configurations to what we need"
         FORCE)
endif()

SET(CMAKE_DEBUG_POSTFIX "d")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include_directories(./)


	
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    message("Setting up a Windows build")
    add_definitions("-DWIN32 -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS -DUSE_MSXML -DUSE_JAVASCRIPT -DUSE_WINDOWS_SCRIPTING")

    set(CMAKE_C_FLAGS "/W4 /EHsc")
    set(CMAKE_CXX_FLAGS "/W4 /EHsc")
	
else()
    message("Setting up a *nix build")
    add_definitions(-DUSE_JAVASCRIPT -DUSE_LIBXML -DPOSIX)
    #
    # Avoid needing to set LD_LIBRARY_PATH (or install to /usr/lib).
    # This tells our binaries to look into the same base directory for
    # their libraries.
    #

    set(CMAKE_BUILD_WITH_INSTALL_RPATH true)
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH false)
    set(CMAKE_SKIP_BUILD_RPATH false)
    set(CMAKE_SKIP_RPATH false)
    set(CMAKE_INSTALL_RPATH "\$ORIGIN/.:\$ORIGIN/..")

    #
    # We want to be using C++11 support.
    #
	
	if(CMAKE_COMPILER_IS_GNUCXX)
		SET(ENABLE_CXX11 "-std=c++11")

	   EXECUTE_PROCESS(COMMAND "${CMAKE_CXX_COMPILER} -dumpversion" OUTPUT_VARIABLE GCC_VERSION)
	   if (GCC_VERSION VERSION_LESS 4.7)
		  SET(ENABLE_CXX11 "-std=c++0x")
	   endif()

	   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ENABLE_CXX11}")
	endif()
	
	add_definitions("-DPOSIX -DUSE_LIBXML -DUSE_JAVASCRIPT")
	
endif()

#
# Make sure our dependencies are all present (as much as we
# care to check).
#

set (TOOLKIT_VERSION_NEED "3.0")
set (wxWidgets_CONFIG_OPTIONS --version=${TOOLKIT_VERSION_NEED})

set(Boost_USE_STATIC_LIBS        ON) # only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)

if (PYTHON)
	find_package(PythonInterp REQUIRED)
	find_package(PythonLibs REQUIRED)
	find_package(Boost REQUIRED COMPONENTS python)
endif()


if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	find_package(wxWidgets REQUIRED COMPONENTS core base adv net stc scintilla)

	if (NOT NOINTEGRATEDDEBUGGING)
		find_package(Boost REQUIRED COMPONENTS system iostreams filesystem zlib)
	else()
		find_package(Boost REQUIRED COMPONENTS iostreams filesystem zlib)
		add_definitions("-DDISABLE_DECISIONLOGIC_INTEGRATION")
	endif()

else()
    find_package(wxWidgets REQUIRED COMPONENTS core base adv net stc)

	if (NOT NOINTEGRATEDDEBUGGING)
		find_package(Boost REQUIRED COMPONENTS system iostreams filesystem)
	else()
		find_package(Boost REQUIRED COMPONENTS iostreams filesystem)
		add_definitions("-DDISABLE_DECISIONLOGIC_INTEGRATION")
	endif()
	
	find_package(mozjs REQUIRED)		
	find_package(LibXslt REQUIRED)
	find_package(Threads REQUIRED)	
    find_package(ZLIB REQUIRED)
endif()


find_package(iconv REQUIRED)
find_package(LibXml2 REQUIRED)
include_directories(${LIBXML2_INCLUDE_DIR})

add_definitions( -DBOOST_ALL_NO_LIB )
add_definitions(${Boost_LIB_DIAGNOSTIC_DEFINITIONS})


include_directories (
                      ${Boost_INCLUDE_DIRS}
                      ${TOOLCHAIN_INCLUDE_DIRECTORIES}
					  ${PROJECT_SRC_DIR}
                    )
					
link_directories(${LIBXML2_LIBRARIES})
					
#
# Move on to building the libraries now.
#

add_subdirectory(EDSEngine)
add_subdirectory(RelationalObjectModel)
add_subdirectory(DecisionLogic)
