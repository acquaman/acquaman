macx {
	# CDFlib dependencies
	CDF_LIB = /Applications/cdf34_0-dist/lib/libcdf.a
	CDF_INCLUDE_DIR = /Applications/cdf34_0-dist/include
} else:linux-g++-64 {
	contains(USERNAME, david){
		# CDFlib dependencies
		CDF_LIB_DIR = $$PATH_TO_AM/contrib/cdf34_1-dist/lib
		CDF_LIB = -L$$CDF_LIB_DIR -lcdf
		CDF_INCLUDE_DIR = $$PATH_TO_AM/contrib/cdf34_1-dist/include
	} else{

	# CDFlib dependencies
	CDF_LIB_DIR = /home/beamline/tools/cdf/lib
	CDF_LIB = -L$$CDF_LIB_DIR -lcdf
	CDF_INCLUDE_DIR = /home/beamline/tools/cdf/include

	QMAKE_LFLAGS_DEBUG *= "-Wl,-rpath,$$CDF_LIB_DIR"
	QMAKE_LFLAGS_RELEASE *= "-Wl,-rpath,$$CDF_LIB_DIR"
}
} else {
	# CDFlib dependencies
	CDF_LIB_DIR = $$PATH_TO_AM/contrib/cdf34_1-dist/lib
	CDF_LIB = -L$$CDF_LIB_DIR -lcdf
	CDF_INCLUDE_DIR = $$PATH_TO_AM/contrib/cdf34_1-dist/include
}

!macx {
	QMAKE_LFLAGS_DEBUG *= "-Wl,-rpath,$$CDF_LIB_DIR"
	QMAKE_LFLAGS_RELEASE *= "-Wl,-rpath,$$CDF_LIB_DIR"
}

INCLUDEPATH *= $$CDF_INCLUDE_DIR

LIBS *= $$CDF_LIB
