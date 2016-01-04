#macx|linux-g++ {
#		# GSL Dependencies
#		GSL_INCLUDE_DIR = $$PATH_TO_AM/contrib/gsl-install/include
#		GSL_LIB = -L$$PATH_TO_AM/contrib/gsl-install/lib -lgsl
#		GSL_CBLAS_LIB = -L$$PATH_TO_AM/contrib/gsl-install/lib -lgslcblas
#} else:linux-g++-32 {
#		# GSL Dependencies
#		GSL_LIB = -lgsl
#		GSL_CBLAS_LIB = -lgslcblas
#} else:linux-g++-64 {
#	contains(USERNAME, david){
#		# GSL Dependencies
#		GSL_LIB = -lgsl
#		GSL_CBLAS_LIB = -lgslcblas
#	} else{

#		# GSL Dependencies
#		GSL_INCLUDE_DIR = /home/beamline/tools/gsl/gsl-1.14-install/include
#		GSL_LIB = -L/home/beamline/tools/gsl/gsl-1.14-install/lib -lgsl
#		GSL_CBLAS_LIB = -lgslcblas
#}
#}

INCLUDEPATH *= $$GSL_INCLUDE_DIR
LIBS *= -L$$GSL_LIB_DIR -lgsl -lgslcblas
