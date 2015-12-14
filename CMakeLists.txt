cmake_minimum_required(VERSION 2.8.9)

project(OsgOculusProto CXX)

# ------------------------------ Default search locations -------------------
if ( MINGW )
	set(OSG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../osg-3.4.0-mingw64")
endif()

# Clion output dir
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "C:/Work/Tim16/OculusDemo/OsgOculusProto/_clioninstall")


#---------------
# Project Info
#---------------
set(OSGOCULUSPROTO_NAME "OsgOculusProto")

# ------------------------------ OpenSceneGraph -------------------------------
# Find OpenSceneGraph
find_package( OpenSceneGraph COMPONENTS
	osgGA
	osgUtil
	OpenThreads
	osg
	osgAnimation
	osgDB
	osgFX
	#osgIntrospection
	osgManipulator
	osgParticle
	osgShadow
	osgSim
	osgTerrain
	osgText
	osgVolume
	osgWidget
 )
find_package( osgViewer )

include_directories( SYSTEM
	${OPENSCENEGRAPH_INCLUDE_DIRS}
	${OSGVIEWER_INCLUDE_DIR}
)

# ------------------------

set(SRC "main.cpp" VXOculusViewer.cpp VXOculusViewer.h)

add_executable(OsgTest  ${SRC} )

target_link_libraries(OsgTest ${OPENSCENEGRAPH_LIBRARIES} ${OSGVIEWER_LIBRARIES})

install(TARGETS OsgTest DESTINATION .)
