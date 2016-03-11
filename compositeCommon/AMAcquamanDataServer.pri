macx {
	# AMDS Source
	PATH_TO_AMDS = $$PATH_TO_AM/../AcquamanDataServer
} else {
	# AMDS Source
	PATH_TO_AMDS = $$PATH_TO_AM/../AcquamanDataServer
}

# Special build paths and options for running on the Jenkins auto-build server (currently at http://srv-aci-01.clsi.ca)
CONFIG(jenkins_build) {

	message("Adding AMDS to jenkins build.")
	# AMDS Source
#	PATH_TO_AMDS = $$PATH_TO_AM/../AcquamanDataServer
}

QT += network

DEPENDPATH *= $$PATH_TO_AMDS $$PATH_TO_AMDS/source
INCLUDEPATH *= $$PATH_TO_AMDS $$PATH_TO_AMDS/source
VPATH *= $$PATH_TO_AMDS

HEADERS *= \
	$$PATH_TO_AMDS/source/appController/AMDSAppController.h \
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
	$$PATH_TO_AMDS/source/ClientRequest/AMDSClientConfigurationRequest.h \
	$$PATH_TO_AMDS/source/Connection/AMDSPacketStats.h \
	$$PATH_TO_AMDS/source/Connection/AMDSServer.h \
	$$PATH_TO_AMDS/source/Connection/AMDSClientTCPSocket.h \
	$$PATH_TO_AMDS/source/DataElement/AMDSDataTypeDefinitions.h \
	$$PATH_TO_AMDS/source/DataElement/AMDSConfigurationDef.h \
	$$PATH_TO_AMDS/source/DataElement/AMDSAxisInfo.h \
	$$PATH_TO_AMDS/source/DataElement/AMDSnDIndex.h \
	$$PATH_TO_AMDS/source/DataElement/AMDSEventData.h \
	$$PATH_TO_AMDS/source/DataElement/AMDSEventDataSupport.h \
	$$PATH_TO_AMDS/source/DataElement/AMDSFlatArray.h \
	$$PATH_TO_AMDS/source/DataElement/AMDSBufferGroupInfo.h \
	$$PATH_TO_AMDS/source/DataElement/AMDSDwellStatusData.h \
	$$PATH_TO_AMDS/source/DataElement/AMDSCommandManager.h \
	$$PATH_TO_AMDS/source/DataHolder/AMDSDataHolder.h \
	$$PATH_TO_AMDS/source/DataHolder/AMDSDataHolderSupport.h \
	$$PATH_TO_AMDS/source/DataHolder/AMDSGenericFlatArrayDataHolder.h \
	$$PATH_TO_AMDS/source/DataHolder/AMDSScalarDataHolder.h \
	$$PATH_TO_AMDS/source/DataHolder/AMDSSpectralDataHolder.h \
	$$PATH_TO_AMDS/source/util/AMDSMetaObjectSupport.h \
	$$PATH_TO_AMDS/source/util/AMDSRunTimeSupport.h

SOURCES *= \
	$$PATH_TO_AMDS/source/appController/AMDSAppController.cpp \
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
	$$PATH_TO_AMDS/source/ClientRequest/AMDSClientConfigurationRequest.cpp \
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
	$$PATH_TO_AMDS/source/DataElement/AMDSCommandManager.cpp \
	$$PATH_TO_AMDS/source/DataHolder/AMDSDataHolder.cpp \
	$$PATH_TO_AMDS/source/DataHolder/AMDSDataHolderSupport.cpp \
	$$PATH_TO_AMDS/source/DataHolder/AMDSGenericFlatArrayDataHolder.cpp \
	$$PATH_TO_AMDS/source/DataHolder/AMDSScalarDataHolder.cpp \
	$$PATH_TO_AMDS/source/DataHolder/AMDSSpectralDataHolder.cpp \
	$$PATH_TO_AMDS/source/util/AMDSMetaObjectSupport.cpp \
	$$PATH_TO_AMDS/source/util/AMDSRunTimeSupport.cpp
