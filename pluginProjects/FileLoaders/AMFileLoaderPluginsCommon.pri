# #####################################################################
# QMake project file for acquaman file loaders.  	October 2014. david.k.chevrier@gmail.com
# ####################################################################

CONFIG(jenkins_build) {
	message("Checking file loader plugins for building with Jenkins.")
	message($$PATH_TO_AM)
}

include ( $$PATH_TO_AM/compositeCommon/AMCommon.pri )

include ( $$PATH_TO_AM/compositeCommon/AMMPlot.pri )
include ( $$PATH_TO_AM/compositeCommon/AMCDF.pri )

include ( $$PATH_TO_AM/compositeCommon/AMDbObject.pri )
include ( $$PATH_TO_AM/compositeCommon/AMControlInfoList.pri )
include ( $$PATH_TO_AM/compositeCommon/AMDetectorInfoSet.pri )

HEADERS         *= $$PATH_TO_AM/source/dataman/AMFileLoaderInterface.h \
		$$PATH_TO_AM/source/dataman/AMScan.h \
		$$PATH_TO_AM/source/dataman/AMScanDictionary.h \
		$$PATH_TO_AM/source/dataman/AMScanParametersDictionary.h \
		$$PATH_TO_AM/source/util/AMTagReplacementParser.h \
		$$PATH_TO_AM/source/util/AMDateTimeUtils.h \
		$$PATH_TO_AM/source/dataman/AMRun.h \
		$$PATH_TO_AM/source/dataman/datasource/AMDataSource.h \
		$$PATH_TO_AM/source/dataman/datasource/AMRawDataSource.h \
		$$PATH_TO_AM/source/dataman/datastore/AMDataStore.h \
		$$PATH_TO_AM/source/dataman/datastore/AMInMemoryDataStore.h \
		$$PATH_TO_AM/source/dataman/datastore/AMCDFDataStore.h \
		$$PATH_TO_AM/source/dataman/datasource/AMDataSourceImageData.h \
		$$PATH_TO_AM/source/dataman/datasource/AMDataSourceSeriesData.h \
		$$PATH_TO_AM/source/dataman/AMAnalysisBlock.h \
		$$PATH_TO_AM/source/dataman/AMAxisInfo.h \
		$$PATH_TO_AM/source/dataman/AMNumber.h \
		$$PATH_TO_AM/source/dataman/AMSamplePre2013.h \
		$$PATH_TO_AM/source/acquaman/AMScanConfiguration.h \
		$$PATH_TO_AM/source/util/AMSettings.h \
		$$PATH_TO_AM/source/util/AMPeriodicTable.h \
		$$PATH_TO_AM/source/util/AMElement.h \
		$$PATH_TO_AM/source/util/AMEmissionLine.h \
		$$PATH_TO_AM/source/util/AMAbsorptionEdge.h \
		$$PATH_TO_AM/source/application/AMPluginsManager.h \
		$$PATH_TO_AM/source/dataman/datasource/AMDataSourceImageDatawDefault.h \
		$$PATH_TO_AM/source/ui/dataman/AMSimpleDataSourceEditor.h \
		$$PATH_TO_AM/source/dataman/AMTextStream.h \
		$$PATH_TO_AM/source/dataman/AMSample.h \
		$$PATH_TO_AM/source/dataman/AMSamplePlate.h \
		$$PATH_TO_AM/source/beamline/camera/AMShapeData.h \
		$$PATH_TO_AM/source/dataman/AMSamplePlatePre2013.h \
		$$PATH_TO_AM/source/util/AMDataSourcePlotSettings.h \
		$$PATH_TO_AM/source/util/AMRange.h \
		$$PATH_TO_AM/source/util/AMDeferredFunctionCall.h \
		$$PATH_TO_AM/source/util/AMUtility.h

SOURCES         *= $$PATH_TO_AM/source/dataman/AMScan.cpp \
		$$PATH_TO_AM/source/dataman/AMScanDictionary.cpp \
		$$PATH_TO_AM/source/dataman/AMScanParametersDictionary.cpp \
		$$PATH_TO_AM/source/util/AMTagReplacementParser.cpp \
		$$PATH_TO_AM/source/util/AMDateTimeUtils.cpp \
		$$PATH_TO_AM/source/dataman/AMRun.cpp \
		$$PATH_TO_AM/source/dataman/datasource/AMDataSource.cpp \
		$$PATH_TO_AM/source/dataman/datasource/AMRawDataSource.cpp \
		$$PATH_TO_AM/source/dataman/datastore/AMDataStore.cpp \
		$$PATH_TO_AM/source/dataman/datastore/AMInMemoryDataStore.cpp \
		$$PATH_TO_AM/source/dataman/datastore/AMCDFDataStore.cpp \
		$$PATH_TO_AM/source/dataman/datasource/AMDataSourceImageData.cpp \
		$$PATH_TO_AM/source/dataman/datasource/AMDataSourceSeriesData.cpp \
		$$PATH_TO_AM/source/dataman/AMAnalysisBlock.cpp \
		$$PATH_TO_AM/source/dataman/AMAxisInfo.cpp \
		$$PATH_TO_AM/source/dataman/AMNumber.cpp \
		$$PATH_TO_AM/source/dataman/AMSamplePre2013.cpp \
		$$PATH_TO_AM/source/acquaman/AMScanConfiguration.cpp \
		$$PATH_TO_AM/source/util/AMSettings.cpp \
		$$PATH_TO_AM/source/util/AMPeriodicTable.cpp \
		$$PATH_TO_AM/source/util/AMElement.cpp \
		$$PATH_TO_AM/source/util/AMEmissionLine.cpp \
		$$PATH_TO_AM/source/util/AMAbsorptionEdge.cpp \
		$$PATH_TO_AM/source/application/AMPluginsManager.cpp \
		$$PATH_TO_AM/source/dataman/datasource/AMDataSourceImageDatawDefault.cpp \
		$$PATH_TO_AM/source/ui/dataman/AMSimpleDataSourceEditor.cpp \
		$$PATH_TO_AM/source/dataman/AMTextStream.cpp \
		$$PATH_TO_AM/source/dataman/AMSample.cpp \
		$$PATH_TO_AM/source/dataman/AMSamplePlate.cpp \
		$$PATH_TO_AM/source/beamline/camera/AMShapeData.cpp \
		$$PATH_TO_AM/source/dataman/AMSamplePlatePre2013.cpp \
		$$PATH_TO_AM/source/util/AMRange.cpp \
		$$PATH_TO_AM/source/util/AMDeferredFunctionCall.cpp \
		$$PATH_TO_AM/source/util/AMUtility.cpp
