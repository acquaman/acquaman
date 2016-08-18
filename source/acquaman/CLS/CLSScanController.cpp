#include "CLSScanController.h"

#include "analysis/AMRegionOfInterestAB.h"
#include "analysis/AM2DNormalizationAB.h"
#include "beamline/AMXRFDetector.h"
#include "dataman/AMScan.h"
#include "dataman/datasource/AMDataSource.h"

CLSScanController::CLSScanController(CLSScanConfiguration *configuration)
{
	controllerScanInstance_ = 0;
	controllerScanconfiguration_ = configuration;
}

void CLSScanController::build2DXRFAnalysisBlock(AMXRFDetector *xrfDetector, const AMDataSource *spectraSource, const QList<AMDataSource *> &i0Sources)
{
	xrfDetector->removeAllRegionsOfInterest();

	foreach (AMRegionOfInterest *region, controllerScanconfiguration_->regionsOfInterest()){
		xrfDetector->addRegionOfInterest(region);

		AMRegionOfInterestAB *regionAB = (AMRegionOfInterestAB *)region->valueSource();
		AMRegionOfInterestAB *newRegion = new AMRegionOfInterestAB(regionAB->name().remove(' '));
		newRegion->setBinningRange(regionAB->binningRange());
		newRegion->setInputDataSources(QList<AMDataSource *>() << const_cast<AMDataSource* const>(spectraSource));
		controllerScanInstance_->addAnalyzedDataSource(newRegion, false, true);

		AM2DNormalizationAB *normalizedRegion = new AM2DNormalizationAB(QString("norm_%1").arg(newRegion->name()));
		normalizedRegion->setInputDataSources(QList<AMDataSource *>() << newRegion << i0Sources);
		normalizedRegion->setDataName(newRegion->name());
		normalizedRegion->setNormalizationName(i0Sources.first()->name());
		controllerScanInstance_->addAnalyzedDataSource(normalizedRegion, true, false);
	}
}
