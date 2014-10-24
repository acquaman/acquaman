macx {
	# EPICS Dependencies:
	EPICS_INCLUDE_DIRS = $$PATH_TO_AM/contrib/epics/base/include \
				$$PATH_TO_AM/contrib/epics/base/include/os/Darwin
	EPICS_LIB_DIR = $$PATH_TO_AM/contrib/epics/base/lib/darwin-x86
} else:linux-g++-64 {
	contains(USERNAME, david){
		# EPICS Dependencies:
		EPICS_INCLUDE_DIRS = $$PATH_TO_AM/contrib/base/include \
					$$PATH_TO_AM/contrib/base/include/os/Linux
		EPICS_LIB_DIR = $$PATH_TO_AM/contrib/base/lib/linux-x86_64
	} else{

	# EPICS Dependencies:
	EPICS_INCLUDE_DIRS = /home/epics/src/R3.14.12/base/include \
				/home/epics/src/R3.14.12/base/include/os/Linux
	EPICS_LIB_DIR = /home/epics/src/R3.14.12/base/lib/linux-x86_64
	}
} else {
	# EPICS Dependencies:
	EPICS_INCLUDE_DIRS = $$PATH_TO_AM/../epics/base/include \
				$$PATH_TO_AM/../epics/base/include/os/Linux
	EPICS_LIB_DIR = $$PATH_TO_AM/../epics/base/lib/linux-x86
}

# Special build paths and options for running on the Jenkins auto-build server (currently at http://beamteam.usask.ca:8080)
CONFIG(jenkins_build) {
	# EPICS Dependencies:
	EPICS_INCLUDE_DIRS = /home/mark/dev/epics/base/include \
				/home/mark/dev/epics/base/include/os/Linux
	EPICS_LIB_DIR = /home/mark/dev/epics/base/lib/linux-x86
}

INCLUDEPATH *= $$EPICS_INCLUDE_DIRS
LIBS *= -L$$EPICS_LIB_DIR -lca -lCom

QMAKE_LFLAGS_DEBUG *= "-Wl,-rpath,$$EPICS_LIB_DIR"
QMAKE_LFLAGS_RELEASE *= "-Wl,-rpath,$$EPICS_LIB_DIR"
