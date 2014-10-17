macx {
		# MPlot Source
		MPLOT_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/MPlot_4_8/include
		MPLOT_LIB_DIR = $$HOME_FOLDER/$$DEV_PATH/MPlot_4_8/lib
}
linux-g++ {
		# MPlot Source
		MPLOT_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/MPlot/include
		MPLOT_LIB_DIR = $$HOME_FOLDER/$$DEV_PATH/MPlot/lib
}
linux-g++-32 {
		# MPlot Source
		MPLOT_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/MPlot/include
		MPLOT_LIB_DIR = $$HOME_FOLDER/$$DEV_PATH/MPlot/lib
}
linux-g++-64 {
		# MPlot Source
		MPLOT_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/MPlot/include
		MPLOT_LIB_DIR = $$HOME_FOLDER/$$DEV_PATH/MPlot/lib
}

INCLUDEPATH *= $$MPLOT_INCLUDE_DIR

LIBS *= -L$$MPLOT_LIB_DIR -lMPlot
