macx {
		# CDFlib dependencies
		CDF_LIB = /Applications/cdf34_0-dist/lib/libcdf.a
		CDF_INCLUDE_DIR = /Applications/cdf34_0-dist/include
}
linux-g++ {
		# CDFlib dependencies
#		CDF_LIB = -lcdf
#		CDF_INCLUDE_DIR = /usr/local/include
		CDF_LIB_DIR = $$PATH_TO_AM/contrib/cdf34_1-dist/lib
		CDF_LIB = -L$$CDF_LIB_DIR -lcdf
		CDF_INCLUDE_DIR = $$PATH_TO_AM/contrib/cdf34_1-dist/include

		QMAKE_LFLAGS_DEBUG *= "-Wl,-rpath,$$CDF_LIB_DIR"
		QMAKE_LFLAGS_RELEASE *= "-Wl,-rpath,$$CDF_LIB_DIR"
}
linux-g++-32 {
		# CDFlib dependencies
		CDF_LIB_DIR = $$PATH_TO_AM/contrib/cdf34_1-dist/lib
		CDF_LIB = -L$$CDF_LIB_DIR -lcdf
		CDF_INCLUDE_DIR = $$PATH_TO_AM/contrib/cdf34_1-dist/include

		QMAKE_LFLAGS_DEBUG *= "-Wl,-rpath,$$CDF_LIB_DIR"
		QMAKE_LFLAGS_RELEASE *= "-Wl,-rpath,$$CDF_LIB_DIR"
}
linux-g++-64 {
		# CDFlib dependencies
		CDF_LIB_DIR = /home/beamline/tools/cdf/lib
		CDF_LIB = -L$$CDF_LIB_DIR -lcdf
		CDF_INCLUDE_DIR = /home/beamline/tools/cdf/include

		QMAKE_LFLAGS_DEBUG *= "-Wl,-rpath,$$CDF_LIB_DIR"
		QMAKE_LFLAGS_RELEASE *= "-Wl,-rpath,$$CDF_LIB_DIR"
}

INCLUDEPATH *= $$CDF_INCLUDE_DIR

LIBS *= $$CDF_LIB
