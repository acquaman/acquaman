macx {
	# CDFlib dependencies
	contains(USERNAME, hunterd){
		CDF_LIB = /Applications/cdf34_1-dist/lib/libcdf.a
		CDF_INCLUDE_DIR = /Applications/cdf34_1-dist/include
	} else {

		CDF_LIB = /Applications/cdf34_0-dist/lib/libcdf.a
		CDF_INCLUDE_DIR = /Applications/cdf34_0-dist/include
	}
} else:linux-g++-64 {
	contains(USERNAME, david){
		# CDFlib dependencies
		CDF_LIB_DIR = $$PATH_TO_AM/contrib/cdf34_1-dist/lib
		CDF_LIB = -L$$CDF_LIB_DIR -lcdf
		CDF_INCLUDE_DIR = $$PATH_TO_AM/contrib/cdf34_1-dist/include
	} else{
            contains(USERNAME, iain){
                    # CDFlib dependencies
                    CDF_LIB_DIR = $$PATH_TO_AM/contrib/cdf34_1-dist/lib
                    CDF_LIB = -L$$CDF_LIB_DIR -lcdf
                    CDF_INCLUDE_DIR = $$PATH_TO_AM/contrib/cdf34_1-dist/include
            } else{

                # CDFlib dependencies
                CDF_LIB_DIR = /home/beamline/tools/cdf/lib
                CDF_LIB = -L$$CDF_LIB_DIR -lcdf
                CDF_INCLUDE_DIR = /home/beamline/tools/cdf/include
            }
        }

	QMAKE_LFLAGS_DEBUG *= "-Wl,-rpath,$$CDF_LIB_DIR"
	QMAKE_LFLAGS_RELEASE *= "-Wl,-rpath,$$CDF_LIB_DIR"

} else {
	# CDFlib dependencies
	CDF_LIB_DIR = $$PATH_TO_AM/contrib/cdf34_1-dist/lib
	CDF_LIB = -L$$CDF_LIB_DIR -lcdf
	CDF_INCLUDE_DIR = $$PATH_TO_AM/contrib/cdf34_1-dist/include
}

# Special build paths and options for running on the Jenkins auto-build server (currently at http://srv-aci-01.clsi.ca)
CONFIG(jenkins_build){

	message("Adding CDF to jenkins build.")
	# CDFlib dependencies
	CDF_LIB_DIR = /home/beamline/tools/cdf/lib
	CDF_LIB = -L$$CDF_LIB_DIR -lcdf
	CDF_INCLUDE_DIR = /home/beamline/tools/cdf/include

	QMAKE_LFLAGS_DEBUG *= "-Wl,-rpath,$$CDF_LIB_DIR"
	QMAKE_LFLAGS_RELEASE *= "-Wl,-rpath,$$CDF_LIB_DIR"
}

!macx {
	QMAKE_LFLAGS_DEBUG *= "-Wl,-rpath,$$CDF_LIB_DIR"
	QMAKE_LFLAGS_RELEASE *= "-Wl,-rpath,$$CDF_LIB_DIR"
}

INCLUDEPATH *= $$CDF_INCLUDE_DIR

LIBS *= $$CDF_LIB
