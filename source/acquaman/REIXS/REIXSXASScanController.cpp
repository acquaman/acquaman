/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").
Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "REIXSXASScanController.h"

#include "analysis/AM1DExpressionAB.h"
#include "beamline/REIXS/REIXSBeamline.h"
#include "dataman/AMUser.h"

REIXSXASScanController::REIXSXASScanController(REIXSXASScanConfiguration* configuration, QObject *parent) :
	AMSA1DScanController(REIXSBeamline::bl()->photonSource()->energy(), REIXSBeamline::bl()->xasDetectors()->saDetectors(), configuration, parent)
{
	config_ = configuration;
	initialMoveAction_ = 0;
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

	ab = new AM1DExpressionAB("PFYNorm");
	ab->setDescription("Normalized PFY");
	ab->setInputDataSources(rawDataSources);
	ab->setExpression("PFY/I0");
	scan_->addAnalyzedDataSource(ab);
}

AMControl * REIXSXASScanController::control()
{
	if((currentRegion_ == -1 && currentStepInRegion_ == -1) || (currentRegion_ == 0 && currentStepInRegion_ == 0))
		return REIXSBeamline::bl()->photonSource()->energy();	// use for first move
	else
		return REIXSBeamline::bl()->photonSource()->directEnergy();	// use for "little" moves.
}

#include "dataman/AMSamplePre2013.h"

void REIXSXASScanController::initializeScanMetaData()
{
	QString rangeString;
	if(config_->regionCount())
		rangeString = QString("%1-%2 eV").arg(config_->regionStart(0)).arg(config_->regionEnd(config_->regionCount()-1));

	if(config_->namedAutomatically()) {
		int sampleId = REIXSBeamline::bl()->currentSampleId();
		if(sampleId >= 1) {
			scan_->setSampleId(sampleId);
			QString sampleName = AMSamplePre2013::sampleNameForId(AMDatabase::database("user"), sampleId); // scan_->sampleName() won't work until the scan is saved to the database.
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

bool REIXSXASScanController::customInitializeImplementation()
{
	// remember the state of the beamline at the beginning of the scan.
//	AMControlInfoList positions(REIXSBeamline::bl()->allControlsSet()->toInfoList());
//	scan_->scanInitialConditions()->setValuesFrom(positions);

//	// Need to move the mono, epu, or exit slit into position?
//	if(config_->applyMonoGrating() || config_->applyMonoMirror() || config_->applyPolarization() || config_->applySlitWidth()) {
//		initialMoveAction_ = new AMListAction(new AMActionInfo("REIXS XAS Initial Move"), AMListAction::ParallelMode);
//		if(config_->applyMonoGrating())
//			initialMoveAction_->addSubAction(new AMInternalControlMoveAction(REIXSBeamline::bl()->photonSource()->monoGratingSelector(), config_->monoGrating()));
//		if(config_->applyMonoMirror())
//			initialMoveAction_->addSubAction(new AMInternalControlMoveAction(REIXSBeamline::bl()->photonSource()->monoMirrorSelector(), config_->monoMirror()));
//		if(config_->applySlitWidth())
//			initialMoveAction_->addSubAction(new AMInternalControlMoveAction(REIXSBeamline::bl()->photonSource()->monoSlit(), config_->slitWidth()));
//		if(config_->applyPolarization()) {
//			initialMoveAction_->addSubAction(new AMInternalControlMoveAction(REIXSBeamline::bl()->photonSource()->epuPolarization(), config_->polarization()));
//			if(config_->polarization() == 5)
//				initialMoveAction_->addSubAction(new AMInternalControlMoveAction(REIXSBeamline::bl()->photonSource()->epuPolarizationAngle(), config_->polarizationAngle()));
//		}

//		connect(initialMoveAction_, SIGNAL(succeeded()), this, SLOT(onInitializationMoveSucceeded()));
//		connect(initialMoveAction_, SIGNAL(failed()), this, SLOT(onInitializationMoveFailed()));

//		initialMoveAction_->start();
//	}
//	else {
//		setCustomInitializationFinished(true);
//	}

	return true;
}

void REIXSXASScanController::onInitializationMoveSucceeded()
{
//	disconnect(initialMoveAction_, 0, this, 0);
//	initialMoveAction_->deleteLater();
//	initialMoveAction_ = 0;

	setCustomInitializationFinished(true);
}

void REIXSXASScanController::onInitializationMoveFailed()
{
//	disconnect(initialMoveAction_, 0, this, 0);
//	initialMoveAction_->deleteLater();
//	initialMoveAction_ = 0;

//	AMErrorMon::alert(this, -300, "Could not start the scan because there was an error moving the mono grating, mirror, EPU polarization, or slit width to the starting positions. Please report this problem to the beamline staff.");
	setCustomInitializationFinished(false);
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

//	qdebug() << "Disabled settling times and tolerance.";
//	setCustomInitializationFinished(true);
//	return true;
//}

//void REIXSXASScanController::customCleanupImplementation()
//{
//	REIXSBrokenMonoControl* energy = REIXSBeamline::bl()->photonSource()->energy();
//	energy->setSingleMoveSettlingTime(singleMoveSettlingTime_);
//	energy->setRepeatMoveSettlingTime(repeatMoveSettlingTime_);
//	energy->setTolerance(tolerance_);

//	qdebug() << "Re-enabled settling times and tolerance.";
//	setCustomCleanupFinished();
//}
