macx {
		# MPlot Source
		MPLOT_INCLUDE_DIR = $$PATH_TO_AM/../MPlot_4_8/include
		MPLOT_LIB_DIR = $$PATH_TO_AM/../MPlot_4_8/lib
}
linux-g++ {
		# MPlot Source
		MPLOT_INCLUDE_DIR = $$PATH_TO_AM/../MPlot/include
		MPLOT_LIB_DIR = $$PATH_TO_AM/../MPlot/lib
}
linux-g++-32 {
		# MPlot Source
		MPLOT_INCLUDE_DIR = $$PATH_TO_AM/../MPlot/include
		MPLOT_LIB_DIR = $$PATH_TO_AM/../MPlot/lib
}
linux-g++-64 {
		# MPlot Source
		MPLOT_INCLUDE_DIR = $$PATH_TO_AM/../MPlot/include
		MPLOT_LIB_DIR = $$PATH_TO_AM/../MPlot/lib
}

INCLUDEPATH *= $$MPLOT_INCLUDE_DIR

LIBS *= -L$$MPLOT_LIB_DIR -lMPlot
