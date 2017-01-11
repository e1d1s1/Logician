#
# Logician Suite
#
#
# C++ Demo
#

#
# copy over resource files
#

set(RESOURCE_FILES "HydraulicCylinderRules.xml")
file(INSTALL ${RESOURCE_FILES} DESTINATION .)


set(SOURCE_FILES "HydraulicCylinderC++.cpp")


set(HEADER_FILES "HydraulicCylinderC++.h")


include( "${wxWidgets_USE_FILE}" )
include( "${wxWidgets_USE_STATIC}" )

#
# Create the shared library here.
#

add_executable(HydraulicCylinder ${SOURCE_FILES} ${HEADER_FILES})

#
# Link against our dependencies.
#


target_link_libraries(  HydraulicCylinder
                    	${wxWidgets_LIBRARIES}
                        EDSEngine 
                        RelationalObjectModel
                        )
