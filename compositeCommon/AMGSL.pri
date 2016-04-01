exists($$GSL_LIB_DIR){

	LIBS *= -L$$GSL_LIB_DIR -lgsl -lgslcblas
} else {

	error(Can not find library dependency: GSL at $$GSL_LIB_DIR)
}

for(CURRENT_DIR, GSL_INCLUDE_DIR) {
	exists($$CURRENT_DIR) {

		INCLUDEPATH *= $$CURRENT_DIR
	} else {

		error(Can not find include dependency: GSL at $$CURRENT_DIR)
	}
}
