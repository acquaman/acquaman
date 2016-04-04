macx {

	CDF_LIB = $$CDF_LIB_DIR/libcdf.a
} else {

	CDF_LIB = -L$$CDF_LIB_DIR -lcdf
	QMAKE_LFLAGS_DEBUG *= "-Wl,-rpath,$$CDF_LIB_DIR"
	QMAKE_LFLAGS_RELEASE *= "-Wl,-rpath,$$CDF_LIB_DIR"
}

exists($$CDF_LIB_DIR){

	LIBS *= $$CDF_LIB
} else {

	error(Can not find library dependency: CDF at $$CDF_LIB_DIR)
}

for(CURRENT_DIR, CDF_INCLUDE_DIR) {
	exists($$CURRENT_DIR) {

		INCLUDEPATH *= $$CURRENT_DIR
	} else {

		error(Can not find include dependency: CDF at $$CURRENT_DIR)
	}
}
