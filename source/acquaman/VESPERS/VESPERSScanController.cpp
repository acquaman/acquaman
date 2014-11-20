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

#include <QStringBuilder>

VESPERSScanController::VESPERSScanController(VESPERSScanConfiguration *configuration)
{
	config_ = configuration;
}

VESPERSScanController::~VESPERSScanController(){}

AMAction3 *VESPERSScanController::buildBaseInitializationAction()
{
	// To initialize the XAS scan, there are four stages.
	/*
		First: Enable/Disable all the pertinent detectors.  The scalar is ALWAYS enabled.
		Second: Set the mode to single shot,set the time on the synchronized dwell time.
	 */
	AMSequentialListAction3 *initializationAction = new AMSequentialListAction3(new AMSequentialListActionInfo3("VESPERS Scan Initialization Actions", "VESPERS Scan Initialization Actions"));
	CLSSIS3820Scaler *scaler = VESPERSBeamline::vespers()->scaler();

	AMListAction3 *stage1 = new AMListAction3(new AMListActionInfo3("VESPERS Initialization Stage 1", "VESPERS Initialization Stage 1"), AMListAction3::Parallel);
	stage1->addSubAction(scaler->createContinuousEnableAction3(false));

	AMListAction3 *stage3 = new AMListAction3(new AMListActionInfo3("VESPERS Initialization Stage 3", "VESPERS Initialization Stage 3"), AMListAction3::Parallel);

	stage3->addSubAction(scaler->createStartAction3(false));
	stage3->addSubAction(scaler->createScansPerBufferAction3(1));
	stage3->addSubAction(scaler->createTotalScansAction3(1));

	initializationAction->addSubAction(stage1);
	initializationAction->addSubAction(stage3);

	return initializationAction;
}

AMAction3 *VESPERSScanController::buildCCDInitializationAction(VESPERS::CCDDetectors ccdChoice, const QString &ccdName)
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
			action->addSubAction(ccd->createFilePathAction("/ramdisk/" % dataFolder.mid(dataFolder.indexOf(QRegExp("\\d{2,2}-\\d{4,4}")), 7)));

		QString uniqueName = getUniqueCCDName(ccd->ccdFilePath(), ccdName);

		if (config_->ccdFileName() != uniqueName)
			config_->setCCDFileName(uniqueName);

		action->addSubAction(ccd->createFileNameAction(uniqueName));
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
	AMListAction3 *cleanup = new AMListAction3(new AMListActionInfo3("VESPERS Cleanup", "VESPERS Cleanup"), AMListAction3::Sequential);
	CLSSIS3820Scaler *scaler = VESPERSBeamline::vespers()->scaler();

	AMListAction3 *stage1 = new AMListAction3(new AMListActionInfo3("VESPERS Cleanup Stage 2", "Setting synchronized dwell options"), AMListAction3::Parallel);
	stage1->addSubAction(scaler->createDwellTimeAction3(1.0));
	stage1->addSubAction(scaler->createContinuousEnableAction3(true));

	cleanup->addSubAction(stage1);

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

	switch(VESPERSBeamline::vespers()->currentBeam()){

	case VESPERS::NoBeam:
		// This should never happen.
		break;

	case VESPERS::Pink:
		notes.append("Beam used:\tPink\n");
		break;

	case VESPERS::TenPercent:
		notes.append(QString("Beam used:\t10% bandpass\nMonochromator energy:\t%1 eV\n").arg(VESPERSBeamline::vespers()->mono()->energy(), 0, 'f', 2));
		break;

	case VESPERS::OnePointSixPercent:
		notes.append(QString("Beam used:\t1.6% bandpass\nMonochromator energy:\t%1 eV\n").arg(VESPERSBeamline::vespers()->mono()->energy(), 0, 'f', 2));
		break;

	case VESPERS::Si:
		notes.append(QString("Beam used:\tSi (%2E/E = 10^-4)\nMonochromator energy:\t%1 eV\n").arg(VESPERSBeamline::vespers()->mono()->energy(), 0, 'f', 2).arg(QString::fromUtf8("Δ")));
		break;
	}

	notes.append(QString("Filter thickness (aluminum):\t%1 %2m\n").arg(VESPERSBeamline::vespers()->endstation()->filterThickness()).arg(QString::fromUtf8("μ")));
	notes.append(QString("Horizontal slit separation:\t%1 mm\n").arg(VESPERSBeamline::vespers()->intermediateSlits()->gapX()));
	notes.append(QString("Vertical slit separation:\t%1 mm\n").arg(VESPERSBeamline::vespers()->intermediateSlits()->gapZ()));
	notes.append(QString("Gas used in ion chambers:\tN2\n"));
	notes.append(QString("\nIon Chamber Gain Settings\n"));

	CLSSIS3820ScalerChannel *channel = VESPERSBeamline::vespers()->scaler()->channelAt(5);
	AMCurrentAmplifier *sr570 = channel->currentAmplifier();
	if (sr570)
		notes.append(QString("%1:\t%2 %3\n").arg("Split").arg(sr570->value()).arg(sr570->units()));
	channel = VESPERSBeamline::vespers()->scaler()->channelAt(7);
	sr570 = channel->currentAmplifier();
		if (sr570)
	notes.append(QString("%1:\t%2 %3\n").arg("Pre-KB").arg(sr570->value()).arg(sr570->units()));
	channel = VESPERSBeamline::vespers()->scaler()->channelAt(8);
	sr570 = channel->currentAmplifier();
		if (sr570)
	notes.append(QString("%1:\t%2 %3\n").arg("Mini").arg(sr570->value()).arg(sr570->units()));
	channel = VESPERSBeamline::vespers()->scaler()->channelAt(9);
	sr570 = channel->currentAmplifier();
		if (sr570)
	notes.append(QString("%1:\t%2 %3\n").arg("Post").arg(sr570->value()).arg(sr570->units()));

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
