# #####################################################################
# QMake project file for acquaman build tests			October 2014. david.k.chevrier@gmail.com
# #####################################################################

TEMPLATE = subdirs
SUBDIRS += \
	AMControlInfoTest/AMControlInfoTest.pro \
	AMControlInfoListTest/AMControlInfoListTest.pro \
	AMDetectorInfoTest/AMDetectorInfoTest.pro \
	AMDetectorInfoSetTest/AMDetectorInfoSetTest.pro \
	AMControlTest/AMControlTest.pro \
	AMPVControlTest/AMPVControlTest.pro \
	AMControlSetTest/AMControlSetTest.pro \
	AMActionTest/AMActionTest.pro \
	AMDetectorTest/AMDetectorTest.pro \
	AMDetectorSetTest/AMDetectorSetTest.pro \
	AMXRFDetectorTest/AMXRFDetector.pro \
	AMScanConfigurationTest/AMScanConfigurationTest.pro
#	AMBeamlineTest/AMBeamlineTest.pro
