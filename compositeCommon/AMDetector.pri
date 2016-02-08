include( AMAction.pri )
include( AMControlSet.pri )
include( QJSON.pri )
include( AMDetectorInfo.pri )

DESTDIR = build

HEADERS *=\
	source/acquaman/AMAgnosticDataAPI.h \
	source/beamline/AMBeamlineSupport.h \
	source/beamline/AMDetector.h \
	source/beamline/AMDetectorTriggerSource.h \
	source/beamline/AMSynchronizedDwellTime.h \
	source/dataman/AMMeasurementInfo.h \
	source/dataman/AMAxisInfo.h \
	source/dataman/AMNumber.h \
	source/dataman/datasource/AMDataSource.h \
	source/actions3/actions/AMDetectorInitializeAction.h \
	source/actions3/actions/AMDetectorDwellTimeAction.h \
	source/actions3/actions/AMDetectorAcquisitionAction.h \
	source/actions3/actions/AMDetectorTriggerAction.h \
	source/actions3/actions/AMDetectorReadAction.h \
	source/actions3/actions/AMDetectorCleanupAction.h \
	source/actions3/actions/AMDetectorInitializeActionInfo.h \
	source/actions3/actions/AMDetectorDwellTimeActionInfo.h \
	source/actions3/actions/AMDetectorAcquisitionActionInfo.h \
	source/actions3/actions/AMDetectorTriggerActionInfo.h \
	source/actions3/actions/AMDetectorReadActionInfo.h \
	source/actions3/actions/AMDetectorCleanupActionInfo.h \
	source/actions3/actions/AMDetectorSetDarkCurrentValidStateActionInfo.h \
	source/actions3/actions/AMDetectorSetDarkCurrentValidStateAction.h \
	source/actions3/actions/AMDetectorSetLastMeasurementAsDarkCurrentActionInfo.h \
	source/actions3/actions/AMDetectorSetLastMeasurementAsDarkCurrentAction.h \
	source/actions3/actions/AMDetectorSetDarkCurrentTimeAction.h \
	source/actions3/actions/AMDetectorSetDarkCurrentTimeActionInfo.h \
	source/actions3/actions/AMDetectorSetDarkCurrentValueActionInfo.h \
	source/actions3/actions/AMDetectorSetDarkCurrentValueAction.h \
	source/util/AMRange.h \
	source/util/AMUtility.h \
    $$PWD/../source/actions3/actions/AMDetectorWaitForAcquisitionStateAction.h \
    $$PWD/../source/actions3/actions/AMDetectorWaitForAcquisitionStateActionInfo.h

SOURCES *= \
	source/acquaman/AMAgnosticDataAPI.cpp \
	source/beamline/AMBeamlineSupport.cpp \
	source/beamline/AMDetector.cpp \
	source/beamline/AMDetectorTriggerSource.cpp \
	source/beamline/AMSynchronizedDwellTime.cpp \
	source/dataman/AMAxisInfo.cpp \
	source/dataman/AMNumber.cpp \
	source/dataman/datasource/AMDataSource.cpp \
	source/actions3/actions/AMDetectorInitializeAction.cpp \
	source/actions3/actions/AMDetectorDwellTimeAction.cpp \
	source/actions3/actions/AMDetectorAcquisitionAction.cpp \
	source/actions3/actions/AMDetectorTriggerAction.cpp \
	source/actions3/actions/AMDetectorReadAction.cpp \
	source/actions3/actions/AMDetectorCleanupAction.cpp \
	source/actions3/actions/AMDetectorInitializeActionInfo.cpp \
	source/actions3/actions/AMDetectorDwellTimeActionInfo.cpp \
	source/actions3/actions/AMDetectorAcquisitionActionInfo.cpp \
	source/actions3/actions/AMDetectorTriggerActionInfo.cpp \
	source/actions3/actions/AMDetectorReadActionInfo.cpp \
	source/actions3/actions/AMDetectorCleanupActionInfo.cpp \
	source/actions3/actions/AMDetectorSetDarkCurrentValidStateActionInfo.cpp \
	source/actions3/actions/AMDetectorSetDarkCurrentValidStateAction.cpp \
	source/actions3/actions/AMDetectorSetLastMeasurementAsDarkCurrentActionInfo.cpp \
	source/actions3/actions/AMDetectorSetLastMeasurementAsDarkCurrentAction.cpp \
	source/actions3/actions/AMDetectorSetDarkCurrentTimeAction.cpp \
	source/actions3/actions/AMDetectorSetDarkCurrentTimeActionInfo.cpp \
	source/actions3/actions/AMDetectorSetDarkCurrentValueActionInfo.cpp \
	source/actions3/actions/AMDetectorSetDarkCurrentValueAction.cpp \
	source/util/AMRange.cpp \
	source/util/AMUtility.cpp \
    $$PWD/../source/actions3/actions/AMDetectorWaitForAcquisitionStateAction.cpp \
    $$PWD/../source/actions3/actions/AMDetectorWaitForAcquisitionStateActionInfo.cpp
