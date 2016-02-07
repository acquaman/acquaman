/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#include "VESPERSScanController.h"

#include "beamline/VESPERS/VESPERSBeamline.h"
//#include "acquaman/dacq3_3/qepicsadvacq.h"
#include "actions3/AMListAction3.h"
#include "beamline/AMCurrentAmplifier.h"
#include "actions3/AMActionSupport.h"

#include <QStringBuilder>

VESPERSScanController::VESPERSScanController(VESPERSScanConfiguration *configuration)
{
	config_ = configuration;
}

VESPERSScanController::~VESPERSScanController(){}

AMAction3 *VESPERSScanController::buildBaseInitializationAction(double firstRegionTime)
{
	// To initialize the XAS scan, there are four stages.
	/*
		First: Enable/Disable all the pertinent detectors.  The scalar is ALWAYS enabled.
		Second: Set the mode to single shot,set the time on the synchronized dwell time.
	 */
	AMSequentialListAction3 *initializationAction = new AMSequentialListAction3(new AMSequentialListActionInfo3("VESPERS Scan Initialization Actions", "VESPERS Scan Initialization Actions"));
	CLSSIS3820Scaler *scaler = VESPERSBeamline::vespers()->scaler();

	AMListAction3 *stage1 = new AMListAction3(new AMListActionInfo3("VESPERS Initialization Stage 1", "VESPERS Initialization Stage 1"), AMListAction3::Sequential);
	stage1->addSubAction(scaler->createContinuousEnableAction3(false));

	AMListAction3 *stage2 = new AMListAction3(new AMListActionInfo3("VESPERS Initialization Stage 2", "VESPERS Initialization Stage 2"), AMListAction3::Sequential);

	stage2->addSubAction(scaler->createStartAction3(false));
	stage2->addSubAction(scaler->createScansPerBufferAction3(1));
	stage2->addSubAction(scaler->createTotalScansAction3(1));

//	AMListAction3 *stage3 = new AMListAction3(new AMListActionInfo3("VESPERS Initialization Stage 3", "VESPERS Initialization Stage 3"), AMListAction3::Sequential);
//	stage3->addSubAction(scaler->createStartAction3(true));
//	stage3->addSubAction(scaler->createWaitForDwellFinishedAction(scaler->dwellTime() + 5.0));

	initializationAction->addSubAction(stage1);
	initializationAction->addSubAction(stage2);
//	initializationAction->addSubAction(stage3);
	initializationAction->addSubAction(scaler->createDwellTimeAction3(firstRegionTime));

	if (VESPERSBeamline::vespers()->endstation()->shutterControl()->value() == 0)
		initializationAction->addSubAction(AMActionSupport::buildControlMoveAction(VESPERSBeamline::vespers()->endstation()->shutterControl(), 1.0));

	return initializationAction;
}

AMAction3 *VESPERSScanController::buildCCDInitializationAction(VESPERS::CCDDetectors ccdChoice, const QString &ccdName, int newScanNumber)
{
	AMParallelListAction3 *action = new AMParallelListAction3(new AMParallelListActionInfo3("CCD Setup Action", "Sets the path, base file name and file number to the detector."));

	if (ccdChoice.testFlag(VESPERS::Roper)){

		VESPERSCCDDetector *ccd = VESPERSBeamline::vespers()->vespersRoperCCD();
		QString uniqueName = getUniqueCCDName(ccd->ccdFilePath(), ccdName);

		if (config_->ccdFileName() != uniqueName)
			config_->setCCDFileName(uniqueName);

		action->addSubAction(ccd->createFileNameAction(uniqueName));
		action->addSubAction(ccd->createFileNumberAction(1));
	}

	if (ccdChoice.testFlag(VESPERS::Mar)){

		VESPERSCCDDetector *ccd = VESPERSBeamline::vespers()->vespersMarCCD();
		QString uniqueName = getUniqueCCDName(ccd->ccdFilePath(), ccdName);

		if (config_->ccdFileName() != uniqueName)
			config_->setCCDFileName(uniqueName);

		action->addSubAction(ccd->createFileNameAction(uniqueName));
		action->addSubAction(ccd->createFileNumberAction(1));
	}

	if (ccdChoice.testFlag(VESPERS::Pilatus)){

		VESPERSCCDDetector *ccd = VESPERSBeamline::vespers()->vespersPilatusAreaDetector();

		QString dataFolder = AMUserSettings::userDataFolder;

		if (dataFolder.contains(QRegExp("\\d{2,2}-\\d{4,4}")))
			action->addSubAction(ccd->createFilePathAction(QString("/ramdisk/%1/%2_%3").arg(dataFolder.mid(dataFolder.indexOf(QRegExp("\\d{2,2}-\\d{4,4}")), 7))
													.arg(config_->ccdFileName())
													.arg(newScanNumber)));

		action->addSubAction(ccd->createFileNameAction(config_->ccdFileName()));
		action->addSubAction(ccd->createFileNumberAction(1));
	}

	return action;
}

AMAction3 *VESPERSScanController::buildCleanupAction()
{
	// To cleanup the XAS scan, there is one stage.
	/*
		First: Only have the scalar running in the syncrhonized dwell time.
		Second: Set the dwell time to 1 second.  Set the scan mode to continuous.  This starts the synchronized dwell time.
		Third: Disables the variable integration time.  Set the relative energy PV to 0.
	 */
	CLSSIS3820Scaler *scaler = VESPERSBeamline::vespers()->scaler();

	AMListAction3 *cleanup = new AMListAction3(new AMListActionInfo3("VESPERS Cleanup", "VESPERS Cleanup"), AMListAction3::Sequential);

	if (config_->cleanupScaler()){

		cleanup->addSubAction(scaler->createDwellTimeAction3(1.0));
		cleanup->addSubAction(scaler->createContinuousEnableAction3(true));
	}

	if (config_->closeFastShutter())
		cleanup->addSubAction(AMActionSupport::buildControlMoveAction(VESPERSBeamline::vespers()->endstation()->shutterControl(), 0.0));

	return cleanup;
}

QString VESPERSScanController::buildNotes()
{
	// Build the notes for the scan.
	QString notes;
	VESPERS::FluorescenceDetectors xrfFlag = config_->fluorescenceDetector();

	if (xrfFlag == VESPERS::SingleElement)
		notes.append(QString("\nFluorescence detector (1-el Vortex) distance to sample:\t%1 mm\n").arg(VESPERSBeamline::vespers()->endstation()->distanceToSingleElementVortex(), 0, 'f', 1));

	if (xrfFlag == VESPERS::FourElement)
		notes.append(QString("\nFluorescence detector (4-el Vortex) distance to sample:\t%1 mm\n").arg(VESPERSBeamline::vespers()->endstation()->distanceToFourElementVortex(), 0, 'f', 1));

	if (config_->ccdDetector() == VESPERS::Pilatus)
		notes.append(QString("\nPilatus distance to sample:\t%1 mm\n").arg(VESPERSBeamline::vespers()->endstation()->distanceToCCD(), 0, 'f', 1));

	notes.append(AMBeamline::bl()->details());

	return notes;
}

QString VESPERSScanController::getUniqueCCDName(const QString &path, const QString &name) const
{
	// Because the paths that go into the PV's are not the paths we use to check, we need to create the right path.  So far, that is /nas/vespers
	QString newPath = path;

	if (config_->ccdDetector() == VESPERS::Roper){

		newPath.replace("Y:\\", "/nas/vespers/");
		newPath.replace('\\', '/');
	}

	else if (config_->ccdDetector() == VESPERS::Pilatus)
		newPath.replace("/ramdisk/", "/nas/pilatus/500_500/");

	if (!VESPERS::fileNameExists(newPath, name))
		return name;

	QString newName = name;

	while (VESPERS::fileNameExists(newPath, newName))
		newName = VESPERS::appendUniqueIdentifier(name);

	return newName;
}
