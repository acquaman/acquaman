#Eigen
#EIGEN_INCLUDE_DIR = $$PATH_TO_AM/source/Eigen
for(CURRENT_DIR, EIGEN_INCLUDE_DIR) {
	exists($$CURRENT_DIR) {

		INCLUDEPATH *= $$CURRENT_DIR
	} else {

		error(Can not find include dependency: Eigen at $$CURRENT_DIR)
	}
}
