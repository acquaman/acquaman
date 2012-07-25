#include "REIXSXASScanController.h"

#include "analysis/AM1DExpressionAB.h"
#include "beamline/REIXS/REIXSBeamline.h"
#include "dataman/AMUser.h"

REIXSXASScanController::REIXSXASScanController(REIXSXASScanConfiguration* configuration, QObject *parent) :
	AMSA1DScanController(REIXSBeamline::bl()->photonSource()->energy(), REIXSBeamline::bl()->xasDetectors()->saDetectors(), configuration, parent)
{
	config_ = configuration;
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

AMControl * REIXSXASScanController::control()
{
	if((currentRegion_ == -1 && currentStepInRegion_ == -1) || (currentRegion_ == 0 && currentStepInRegion_ == 0))
		return REIXSBeamline::bl()->photonSource()->energy();	// use for first move
	else
		return REIXSBeamline::bl()->photonSource()->directEnergy();	// use for "little" moves.
}

#include "dataman/AMSample.h"

void REIXSXASScanController::initializeScanMetaData()
{
	QString rangeString;
	if(config_->regionCount())
		rangeString = QString("%1-%2 eV").arg(config_->regionStart(0)).arg(config_->regionEnd(config_->regionCount()-1));

	if(config_->namedAutomatically()) {
		int sampleId = REIXSBeamline::bl()->currentSampleId();
		if(sampleId >= 1) {
			scan_->setSampleId(sampleId);
			QString sampleName = AMSample::sampleNameForId(AMDatabase::database("user"), sampleId); // scan_->sampleName() won't work until the scan is saved to the database.
			scan_->setName(QString("%1 %2 %3").arg(sampleName).arg(config_->autoScanName()).arg(rangeString));
			scan_->setNumber(scan_->largestNumberInScansWhere(AMDatabase::database("user"), QString("sampleId = %1").arg(sampleId))+1);
		}
		else {
			scan_->setName(QString("%1 %2").arg(config_->autoScanName()).arg(rangeString));
			scan_->setNumber(0);
			scan_->setSampleId(-1);
		}
	}
	else {
		scan_->setName(config_->userScanName());
		if(scan_->name().isEmpty())
			scan_->setName(QString("%1 %2").arg(config_->autoScanName()).arg(rangeString));
		scan_->setNumber(config_->scanNumber());
		scan_->setSampleId(config_->sampleId());
	}

	scan_->setRunId(AMUser::user()->currentRunId());
}

// Was thinking about disabling the settling time, and then restoring it after. Problem with this approach: we need the three-step move for the initial (possibly long-distance) move.  Instead, return a different control depending on the situation.
//bool REIXSXASScanController::customInitializeImplementation()
//{
//	REIXSBrokenMonoControl* energy = REIXSBeamline::bl()->photonSource()->energy();
//	singleMoveSettlingTime_ = energy->singleMoveSettlingTime();
//	repeatMoveSettlingTime_ = energy->repeatMoveSettlingTime();
//	tolerance_ = energy->tolerance();

//	energy->setSingleMoveSettlingTime(0);
//	energy->setRepeatMoveSettlingTime(0);
//	energy->setTolerance(1e10);	// that better be enough, pos.

//	qDebug() << "Disabled settling times and tolerance.";
//	setCustomInitializationFinished(true);
//	return true;
//}

//void REIXSXASScanController::customCleanupImplementation()
//{
//	REIXSBrokenMonoControl* energy = REIXSBeamline::bl()->photonSource()->energy();
//	energy->setSingleMoveSettlingTime(singleMoveSettlingTime_);
//	energy->setRepeatMoveSettlingTime(repeatMoveSettlingTime_);
//	energy->setTolerance(tolerance_);

//	qDebug() << "Re-enabled settling times and tolerance.";
//	setCustomCleanupFinished();
//}
