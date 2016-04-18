exists($$EPICS_LIB_DIR) {

	LIBS *= -L$$EPICS_LIB_DIR -lca -lCom -ldl
	QMAKE_LFLAGS_DEBUG *= "-Wl,-rpath,$$EPICS_LIB_DIR"
	QMAKE_LFLAGS_RELEASE *= "-Wl,-rpath,$$EPICS_LIB_DIR"
} else {

	error(Can not find library dependency: EPICS at $$EPICS_LIB_DIR)
}

for(CURRENT_DIR, EPICS_INCLUDE_DIR) {
	exists($$CURRENT_DIR) {

		INCLUDEPATH *= $$CURRENT_DIR
	} else {

		error(Can not find include dependency: EPICS at $$CURRENT_DIR)
	}
}
