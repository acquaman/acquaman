macx {
		# MPlot Source
		MPLOT_INCLUDE_DIR = $$PATH_TO_AM/../MPlot_4_8/include
		MPLOT_LIB_DIR = $$PATH_TO_AM/../MPlot_4_8/lib
} else {
		# MPlot Source
		MPLOT_INCLUDE_DIR = $$PATH_TO_AM/../MPlot/include
		MPLOT_LIB_DIR = $$PATH_TO_AM/../MPlot/lib
}

INCLUDEPATH *= $$MPLOT_INCLUDE_DIR

LIBS *= -L$$MPLOT_LIB_DIR -lMPlot

QMAKE_LFLAGS_DEBUG *= "-Wl,-rpath,$$MPLOT_LIB_DIR"
QMAKE_LFLAGS_RELEASE *= "-Wl,-rpath,$$MPLOT_LIB_DIR"
