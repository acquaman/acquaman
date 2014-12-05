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
	source/actions3/actions/AMDetectorSetAsDarkCurrentCorrectionAction.h \
	source/actions3/actions/AMDetectorInitializeAction.h \
	source/actions3/actions/AMDetectorDwellTimeAction.h \
	source/actions3/actions/AMDetectorAcquisitionAction.h \
	source/actions3/actions/AMDetectorTriggerAction.h \
	source/actions3/actions/AMDetectorReadAction.h \
	source/actions3/actions/AMDetectorCleanupAction.h \
	source/actions3/actions/AMDetectorSetAsDarkCurrentCorrectionActionInfo.h \
	source/actions3/actions/AMDetectorInitializeActionInfo.h \
	source/actions3/actions/AMDetectorDwellTimeActionInfo.h \
	source/actions3/actions/AMDetectorAcquisitionActionInfo.h \
	source/actions3/actions/AMDetectorTriggerActionInfo.h \
	source/actions3/actions/AMDetectorReadActionInfo.h \
	source/actions3/actions/AMDetectorCleanupActionInfo.h

SOURCES *= \
	source/acquaman/AMAgnosticDataAPI.cpp \
	source/beamline/AMBeamlineSupport.cpp \
	source/beamline/AMDetector.cpp \
	source/beamline/AMDetectorTriggerSource.cpp \
	source/beamline/AMSynchronizedDwellTime.cpp \
	source/dataman/AMAxisInfo.cpp \
	source/dataman/AMNumber.cpp \
	source/dataman/datasource/AMDataSource.cpp \
	source/actions3/actions/AMDetectorSetAsDarkCurrentCorrectionAction.cpp \
	source/actions3/actions/AMDetectorInitializeAction.cpp \
	source/actions3/actions/AMDetectorDwellTimeAction.cpp \
	source/actions3/actions/AMDetectorAcquisitionAction.cpp \
	source/actions3/actions/AMDetectorTriggerAction.cpp \
	source/actions3/actions/AMDetectorReadAction.cpp \
	source/actions3/actions/AMDetectorCleanupAction.cpp \
	source/actions3/actions/AMDetectorSetAsDarkCurrentCorrectionActionInfo.cpp \
	source/actions3/actions/AMDetectorInitializeActionInfo.cpp \
	source/actions3/actions/AMDetectorDwellTimeActionInfo.cpp \
	source/actions3/actions/AMDetectorAcquisitionActionInfo.cpp \
	source/actions3/actions/AMDetectorTriggerActionInfo.cpp \
	source/actions3/actions/AMDetectorReadActionInfo.cpp \
	source/actions3/actions/AMDetectorCleanupActionInfo.cpp
