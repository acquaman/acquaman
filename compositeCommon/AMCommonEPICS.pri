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
            contains(USERNAME, iain){
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
        }
} else {
        # EPICS Dependencies:
        EPICS_INCLUDE_DIRS = $$PATH_TO_AM/../epics/base/include \
                                $$PATH_TO_AM/../epics/base/include/os/Linux
        contains(USERNAME, iain) {
            EPICS_LIB_DIR = $$PATH_TO_AM/../epics/base/lib/linux-x86_64
        } else {
            EPICS_LIB_DIR = $$PATH_TO_AM/../epics/base/lib/linux-x86
        }
}

# Special build paths and options for running on the Jenkins auto-build server (currently at http://srv-aci-01.clsi.ca)
CONFIG(jenkins_build) {

	message("Adding EPICS to jenkins build.")
        # EPICS Dependencies:
	EPICS_INCLUDE_DIRS = /home/epics/src/R3.14.12/base/include \
				/home/epics/src/R3.14.12/base/include/os/Linux
	EPICS_LIB_DIR = /home/epics/src/R3.14.12/base/lib/linux-x86_64
}

INCLUDEPATH *= $$EPICS_INCLUDE_DIRS
LIBS *= -L$$EPICS_LIB_DIR -lca -lCom -ldl

QMAKE_LFLAGS_DEBUG *= "-Wl,-rpath,$$EPICS_LIB_DIR"
QMAKE_LFLAGS_RELEASE *= "-Wl,-rpath,$$EPICS_LIB_DIR"
