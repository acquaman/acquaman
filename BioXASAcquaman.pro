# #####################################################################
# QMake project file for BioXAS
# #####################################################################

TEMPLATE = subdirs
SUBDIRS += \
	Initialize.pro \
	BioXASMainAcquaman.pro \
	BioXASMainTableCoordinator.pro \
	BioXASSideAcquaman.pro \
	BioXASSideTableCoordinator.pro \
	BioXASImagingAcquaman.pro \
	BioXASToolSuite.pro
