include( AMAction.pri )
include( AMControlSet.pri )

DESTDIR = build

HEADERS *=\
	source/beamline/AMBeamlineSupport.h \
	source/dataman/info/AMDetectorInfo.h \
	source/beamline/AMDetector.h \
	source/beamline/AMDetectorTriggerSource.h \
	source/acquaman/AMAgnosticDataAPI.h \
	source/qjson/json_parser.hh \
	source/qjson/json_scanner.h \
	source/qjson/location.hh \
	source/qjson/parser_p.h \
	source/qjson/parser.h \
	source/qjson/parserrunnable.h \
	source/qjson/position.hh \
	source/qjson/qjson_debug.h \
	source/qjson/qjson_export.h \
	source/qjson/qobjecthelper.h \
	source/qjson/serializer.h \
	source/qjson/serializerrunnable.h \
	source/qjson/stack.hh \
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
	source/actions3/actions/AMDetectorCleanupActionInfo.h \
	source/beamline/AMSynchronizedDwellTime.h

SOURCES *= \
	source/beamline/AMBeamlineSupport.cpp \
	source/dataman/info/AMDetectorInfo.cpp \
	source/beamline/AMDetector.cpp \
	source/beamline/AMDetectorTriggerSource.cpp \
	source/acquaman/AMAgnosticDataAPI.cpp \
	source/qjson/json_parser.cc \
	source/qjson/json_scanner.cpp \
	source/qjson/parser.cpp \
	source/qjson/parserrunnable.cpp \
	source/qjson/qobjecthelper.cpp \
	source/qjson/serializer.cpp \
	source/qjson/serializerrunnable.cpp \
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
	source/actions3/actions/AMDetectorCleanupActionInfo.cpp \
	source/beamline/AMSynchronizedDwellTime.cpp
