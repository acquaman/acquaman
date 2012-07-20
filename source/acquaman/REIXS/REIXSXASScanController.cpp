#include "REIXSXASScanController.h"

#include "analysis/AM1DExpressionAB.h"
#include "beamline/REIXS/REIXSBeamline.h"

REIXSXASScanController::REIXSXASScanController(REIXSXASScanConfiguration* configuration, QObject *parent) :
	AMSA1DScanController(REIXSBeamline::bl()->photonSource()->energy(), REIXSBeamline::bl()->xasDetectors(), configuration, parent)
{

}

REIXSXASScanController::~REIXSXASScanController() {

}

void REIXSXASScanController::createAnalysisBlocks()
{
	QList<AMDataSource*> rawDataSources;
	for(int i=0, cc=scan_->rawDataSourceCount(); i<cc; ++i)
		rawDataSources << scan_->rawDataSources()->at(i);

	AM1DExpressionAB* ab = new AM1DExpressionAB("TEYNorm");
	ab->setDescription("Normalized TEY");
	ab->setInputDataSources(rawDataSources);
	ab->setExpression("TEY/I0");
	scan_->addAnalyzedDataSource(ab);

	ab = new AM1DExpressionAB("TFYNorm");
	ab->setDescription("Normalized TFY");
	ab->setInputDataSources(rawDataSources);
	ab->setExpression("TFY/I0");
	scan_->addAnalyzedDataSource(ab);
}
