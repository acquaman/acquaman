macx {

	HDF5_LIB = $$HDF5_LIB_DIR/libhdf5.a -lz -ldl -lm
} else {

	HDF5_LIB = -L$$HDF5_LIB_DIR -lhdf5
	QMAKE_LFLAGS_DEBUG *= "-Wl,-rpath,$$HDF5_LIB_DIR"
	QMAKE_LFLAGS_RELEASE *= "-Wl,-rpath,$$HDF5_LIB_DIR"
}

exists($$HDF5_LIB_DIR){

	LIBS *= $$HDF5_LIB
} else {

	error(Can not find library dependency: HDF5 at $$HDF5_LIB_DIR)
}

for(CURRENT_DIR, HDF5_INCLUDE_DIR) {
	exists($$CURRENT_DIR) {

		INCLUDEPATH *= $$CURRENT_DIR
	} else {

		error(Can not find include dependency: HDF5 at $$CURRENT_DIR)
	}
}
