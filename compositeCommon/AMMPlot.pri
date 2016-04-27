contains( DEFINES, AM_PRAGMA_WARNING_CONTROLS){
	DEFINES *= MPLOT_PRAGMA_WARNING_CONTROLS
}

exists($$MPLOT_LIB_DIR){

	LIBS *= -L$$MPLOT_LIB_DIR -lMPlot

	QMAKE_LFLAGS_DEBUG *= "-Wl,-rpath,$$MPLOT_LIB_DIR"
	QMAKE_LFLAGS_RELEASE *= "-Wl,-rpath,$$MPLOT_LIB_DIR"
} else {

	error(Can not find library dependency: MPlot at $$MPLOT_LIB_DIR)
}

for(CURRENT_DIR, MPLOT_INCLUDE_DIR) {
	exists($$CURRENT_DIR) {

		INCLUDEPATH *= $$CURRENT_DIR
	} else {

		error(Can not find include dependency: MPlot at $$CURRENT_DIR)
	}
}
