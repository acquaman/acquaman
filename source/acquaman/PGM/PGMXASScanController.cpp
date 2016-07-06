#include "PGMXASScanController.h"

#include "dataman/export/AMExporterXDIFormat.h"

#include "application/AMAppControllerSupport.h"
#include "application/PGM/PGM.h"

PGMXASScanController::PGMXASScanController(PGMXASScanConfiguration *configuration, QObject *parent)
	: AMGenericStepScanController(configuration, parent)
{
	configuration_ = configuration;

	AMExporterOptionXDIFormat *pgmXASExportOptions = PGM::buildXDIFormatExporterOption("PGMXASDefault", true);
	if(pgmXASExportOptions->id() > 0)
		AMAppControllerSupport::registerClass<PGMXASScanConfiguration, AMExporterXDIFormat, AMExporterOptionXDIFormat>(pgmXASExportOptions->id());
}

