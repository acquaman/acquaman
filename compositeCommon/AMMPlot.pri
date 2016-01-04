#macx {
#	# MPlot Source
#	MPLOT_INCLUDE_DIR = $$PATH_TO_AM/../MPlot_4_8/include
#	MPLOT_LIB_DIR = $$PATH_TO_AM/../MPlot_4_8/lib
#} else {
#	# MPlot Source
#	MPLOT_INCLUDE_DIR = $$PATH_TO_AM/../MPlot/include
#	MPLOT_LIB_DIR = $$PATH_TO_AM/../MPlot/lib
#}

## Special build paths and options for running on the Jenkins auto-build server (currently at http://srv-aci-01.clsi.ca)
#CONFIG(jenkins_build) {

#	message("Adding MPlot to jenkins build.")
#	# MPlot Source
#	MPLOT_INCLUDE_DIR = /var/lib/jenkins/jobs/MPlot/workspace/include
#	MPLOT_LIB_DIR = /var/lib/jenkins/jobs/MPlot/workspace/lib
#}
contains( DEFINES, AM_PRAGMA_WARNING_CONTROLS){
	DEFINES *= MPLOT_PRAGMA_WARNING_CONTROLS
}

INCLUDEPATH *= $$MPLOT_INCLUDE_DIR

LIBS *= -L$$MPLOT_LIB_DIR -lMPlot

QMAKE_LFLAGS_DEBUG *= "-Wl,-rpath,$$MPLOT_LIB_DIR"
QMAKE_LFLAGS_RELEASE *= "-Wl,-rpath,$$MPLOT_LIB_DIR"
