include ( clsCommon.pri )
include ( sgmCommon.pri )

TARGET = SGMAcquaman	

RESOURCES += \

HEADERS += \
	source/application/SGM/SGMAppController.h \
	source/ui/SGM/SGMPersistentView.h \
    source/ui/SGM/SGMHexapodTrajectoryView.h \
    source/ui/SGM/SGMHexapodSystemRotationView.h \
    source/ui/SGM/SGMHexapodView.h \
    source/ui/SGM/SGMEnergyView.h

SOURCES += \
    source/application/SGM/SGMMain.cpp \
    source/application/SGM/SGMAppController.cpp \
	source/ui/SGM/SGMPersistentView.cpp \
    source/ui/SGM/SGMHexapodTrajectoryView.cpp \
    source/ui/SGM/SGMHexapodSystemRotationView.cpp \
    source/ui/SGM/SGMHexapodView.cpp \
    source/ui/SGM/SGMEnergyView.cpp

PATH_TO_AMDS = /BuildAcquaman/DavidAcquaman/AcquamanDataServer

DEPENDPATH *= $$PATH_TO_AMDS $$PATH_TO_AMDS/source
INCLUDEPATH *= $$PATH_TO_AMDS $$PATH_TO_AMDS/source
VPATH *= $$PATH_TO_AMDS

HEADERS *= \
	$$PATH_TO_AMDS/source/appController/AMDSClientAppController.h \
	$$PATH_TO_AMDS/source/ClientRequest/AMDSClientRequest.h \
	$$PATH_TO_AMDS/source/ClientRequest/AMDSClientRequestSupport.h \
	$$PATH_TO_AMDS/source/ClientRequest/AMDSClientRequestDefinitions.h \
	$$PATH_TO_AMDS/source/ClientRequest/AMDSClientDataRequest.h \
	$$PATH_TO_AMDS/source/ClientRequest/AMDSClientIntrospectionRequest.h \
	$$PATH_TO_AMDS/source/ClientRequest/AMDSClientStatisticsRequest.h \
	$$PATH_TO_AMDS/source/ClientRequest/AMDSClientStartTimePlusCountDataRequest.h \
	$$PATH_TO_AMDS/source/ClientRequest/AMDSClientRelativeCountPlusCountDataRequest.h \
	$$PATH_TO_AMDS/source/ClientRequest/AMDSClientStartTimeToEndTimeDataRequest.h \
	$$PATH_TO_AMDS/source/ClientRequest/AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest.h \
	$$PATH_TO_AMDS/source/ClientRequest/AMDSClientContinuousDataRequest.h \
	$$PATH_TO_AMDS/source/Connection/AMDSPacketStats.h \
	$$PATH_TO_AMDS/source/Connection/AMDSServer.h \
	$$PATH_TO_AMDS/source/Connection/AMDSClientTCPSocket.h \
	$$PATH_TO_AMDS/source/DataElement/AMDSDataTypeDefinitions.h \
	$$PATH_TO_AMDS/source/DataElement/AMDSAxisInfo.h \
	$$PATH_TO_AMDS/source/DataElement/AMDSnDIndex.h \
	$$PATH_TO_AMDS/source/DataElement/AMDSEventData.h \
	$$PATH_TO_AMDS/source/DataElement/AMDSEventDataSupport.h \
	$$PATH_TO_AMDS/source/DataElement/AMDSFlatArray.h \
	$$PATH_TO_AMDS/source/DataElement/AMDSBufferGroupInfo.h \
	$$PATH_TO_AMDS/source/DataElement/AMDSDwellStatusData.h \
	$$PATH_TO_AMDS/source/DataHolder/AMDSDataHolder.h \
	$$PATH_TO_AMDS/source/DataHolder/AMDSDataHolderSupport.h \
	$$PATH_TO_AMDS/source/DataHolder/AMDSGenericFlatArrayDataHolder.h \
	$$PATH_TO_AMDS/source/DataHolder/AMDSScalarDataHolder.h \
	$$PATH_TO_AMDS/source/DataHolder/AMDSSpectralDataHolder.h \
	$$PATH_TO_AMDS/source/util/AMDSMetaObjectSupport.h

SOURCES *= \
	$$PATH_TO_AMDS/source/appController/AMDSClientAppController.cpp \
	$$PATH_TO_AMDS/source/ClientRequest/AMDSClientRequest.cpp \
	$$PATH_TO_AMDS/source/ClientRequest/AMDSClientRequestSupport.cpp \
	$$PATH_TO_AMDS/source/ClientRequest/AMDSClientDataRequest.cpp \
	$$PATH_TO_AMDS/source/ClientRequest/AMDSClientIntrospectionRequest.cpp \
	$$PATH_TO_AMDS/source/ClientRequest/AMDSClientStatisticsRequest.cpp \
	$$PATH_TO_AMDS/source/ClientRequest/AMDSClientStartTimePlusCountDataRequest.cpp \
	$$PATH_TO_AMDS/source/ClientRequest/AMDSClientRelativeCountPlusCountDataRequest.cpp \
	$$PATH_TO_AMDS/source/ClientRequest/AMDSClientStartTimeToEndTimeDataRequest.cpp \
	$$PATH_TO_AMDS/source/ClientRequest/AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest.cpp \
	$$PATH_TO_AMDS/source/ClientRequest/AMDSClientContinuousDataRequest.cpp \
	$$PATH_TO_AMDS/source/Connection/AMDSPacketStats.cpp \
	$$PATH_TO_AMDS/source/Connection/AMDSServer.cpp \
	$$PATH_TO_AMDS/source/Connection/AMDSClientTCPSocket.cpp \
	$$PATH_TO_AMDS/source/DataElement/AMDSAxisInfo.cpp \
	$$PATH_TO_AMDS/source/DataElement/AMDSnDIndex.cpp \
	$$PATH_TO_AMDS/source/DataElement/AMDSEventData.cpp \
	$$PATH_TO_AMDS/source/DataElement/AMDSEventDataSupport.cpp \
	$$PATH_TO_AMDS/source/DataElement/AMDSFlatArray.cpp \
	$$PATH_TO_AMDS/source/DataElement/AMDSBufferGroupInfo.cpp \
	$$PATH_TO_AMDS/source/DataElement/AMDSDwellStatusData.cpp \
	$$PATH_TO_AMDS/source/DataHolder/AMDSDataHolder.cpp \
	$$PATH_TO_AMDS/source/DataHolder/AMDSDataHolderSupport.cpp \
	$$PATH_TO_AMDS/source/DataHolder/AMDSGenericFlatArrayDataHolder.cpp \
	$$PATH_TO_AMDS/source/DataHolder/AMDSScalarDataHolder.cpp \
	$$PATH_TO_AMDS/source/DataHolder/AMDSSpectralDataHolder.cpp \
	$$PATH_TO_AMDS/source/util/AMDSMetaObjectSupport.cpp
