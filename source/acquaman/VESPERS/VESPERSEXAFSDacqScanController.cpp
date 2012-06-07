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


#include "VESPERSEXAFSDacqScanController.h"
#include "beamline/VESPERS/VESPERSBeamline.h"
#include "actions/AMBeamlineActionsList.h"
#include "dataman/AMUser.h"
#include "analysis/AM1DExpressionAB.h"
#include "analysis/AM2DSummingAB.h"
#include "actions/AMBeamlineParallelActionsList.h"

#include <QDir>

VESPERSEXAFSDacqScanController::VESPERSEXAFSDacqScanController(VESPERSEXAFSScanConfiguration *cfg, QObject *parent)
	: AMDacqScanController(cfg, parent)
{
	config_ = cfg;
	config_->setUserScanName(config_->name());

	setupXASAction_ = 0;
	cleanupXASAction_ = 0;

	secondsElapsed_ = 0;
	secondsTotal_ = config_->totalTime();
	elapsedTime_.setInterval(1000);
	connect(this, SIGNAL(started()), &elapsedTime_, SLOT(start()));
	connect(this, SIGNAL(cancelled()), &elapsedTime_, SLOT(stop()));
	connect(this, SIGNAL(paused()), &elapsedTime_, SLOT(stop()));
	connect(this, SIGNAL(resumed()), &elapsedTime_, SLOT(start()));
	connect(this, SIGNAL(failed()), &elapsedTime_, SLOT(stop()));
	connect(this, SIGNAL(finished()), &elapsedTime_, SLOT(stop()));
	connect(&elapsedTime_, SIGNAL(timeout()), this, SLOT(onScanTimerUpdate()));

	scan_ = new AMXASScan(); 	// MB: Moved from line 363 in startImplementation.
	scan_->setName(config_->name());
	scan_->setFileFormat("vespers2011EXAFS");
	scan_->setScanConfiguration(config_);
	scan_->setRunId(AMUser::user()->currentRunId());
	scan_->setIndexType("fileSystem");
	scan_->rawData()->addScanAxis(AMAxisInfo("eV", 0, "Incident Energy", "eV"));

	QString notes;

	if (config_->fluorescenceDetectorChoice() == VESPERSEXAFSScanConfiguration::SingleElement)
		notes.append(QString("\nFluorescence detector distance to sample:\t%1 mm\n")).arg(VESPERSBeamline::vespers()->endstation()->distanceToSingleElementVortex(), 0, 'f', 1);
	else
		notes.append(QString("\nFluorescence detector distance to sample:\t%1 mm\n")).arg(VESPERSBeamline::vespers()->endstation()->distanceToFourElementVortex(), 0, 'f', 1);

	notes.append(QString("Filter thickness (aluminum):\t%1 %2m\n").arg(VESPERSBeamline::vespers()->endstation()->filterThickness()).arg(QString::fromUtf8("μ")));
	notes.append(QString("Horizontal slit separation:\t%1 mm\n").arg(VESPERSBeamline::vespers()->intermediateSlits()->gapX()));
	notes.append(QString("Vertical slit separation:\t%1 mm\n").arg(VESPERSBeamline::vespers()->intermediateSlits()->gapZ()));
	notes.append(QString("Gas used in ion chambers:\tN2\n"));
	notes.append(QString("\nIon Chamber Gain Settings\n"));
	CLSSplitIonChamber *split = VESPERSBeamline::vespers()->iSplit();
	notes.append(QString("%1:\t%2 %3\n").arg(split->name()).arg(split->sensitivityValueA()).arg(split->sensitivityUnitsA()));
	CLSIonChamber *chamber = VESPERSBeamline::vespers()->iPreKB();
	notes.append(QString("%1:\t%2 %3\n").arg(chamber->name()).arg(chamber->sensitivityValue()).arg(chamber->sensitivityUnits()));
	chamber = VESPERSBeamline::vespers()->iMini();
	notes.append(QString("%1:\t%2 %3\n").arg(chamber->name()).arg(chamber->sensitivityValue()).arg(chamber->sensitivityUnits()));
	chamber = VESPERSBeamline::vespers()->iPost();
	notes.append(QString("%1:\t%2 %3\n").arg(chamber->name()).arg(chamber->sensitivityValue()).arg(chamber->sensitivityUnits()));

	scan_->setNotes(notes);

	AMDetectorSet *ionChambers = VESPERSBeamline::vespers()->ionChambers();

	// Add all the raw datasources.
	AMMeasurementInfo temp(*(ionChambers->detectorAt((int)config_->incomingChoice())->toInfo()));
	temp.name = "I0";
	scan_->rawData()->addMeasurement(temp);
	scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), 0), true, false);
	temp = AMMeasurementInfo(*(ionChambers->detectorAt((int)config_->transmissionChoice())->toInfo()));
	temp.name = "It";
	scan_->rawData()->addMeasurement(temp);
	scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), 1), true, false);

	// Adding in the extra ion chambers.
	for (int i = 0; i < ionChambers->count(); i++){

		if (i != (int)config_->incomingChoice() && i != (int)config_->transmissionChoice()){

			temp = AMMeasurementInfo(*(ionChambers->detectorAt(i)->toInfo()));
			temp.name = ionChambers->detectorAt(i)->detectorName();
			scan_->rawData()->addMeasurement(temp);
			scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount() - 1), false, true);
		}
	}

	if (config_->fluorescenceDetectorChoice() == VESPERSEXAFSScanConfiguration::SingleElement){

		XRFDetector *detector = VESPERSBeamline::vespers()->vortexXRF1E();
		QString edge = config_->edge();

		// If this is an L edge, chop out the number for comparison purposes.
		if (edge.contains(QRegExp("L\\d$")))
			edge.chop(1);

		// This is safe and okay because I always have the regions of interest set taking up 0-X where X is the count-1 of the number of regions of interest.
		for (int i = 0; i < detector->roiInfoList()->count(); i++){

			scan_->rawData()->addMeasurement(AMMeasurementInfo(detector->roiInfoList()->at(i).name().remove(" "), detector->roiInfoList()->at(i).name()));

			if (detector->roiInfoList()->at(i).name().contains(edge))
				scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount() - 1), true, false);

			else
				scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount() - 1), false, true);
		}

		// Add all the extra raw data sources.
		addExtraDatasources();

		temp = AMMeasurementInfo(detector->toXRFInfo());
		temp.name = "spectra";
		scan_->rawData()->addMeasurement(temp);
		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), true, false);
	}
	else if (config_->fluorescenceDetectorChoice() == VESPERSEXAFSScanConfiguration::FourElement){

		XRFDetector *detector = VESPERSBeamline::vespers()->vortexXRF4E();
		QString edge = config_->edge();

		// If this is an L edge, chop out the number for comparison purposes.
		if (edge.contains(QRegExp("L\\d$")))
			edge.chop(1);

		// This is safe and okay because I always have the regions of interest set taking up 0-X where X is the count-1 of the number of regions of interest.
		for (int i = 0; i < detector->roiInfoList()->count(); i++){

			scan_->rawData()->addMeasurement(AMMeasurementInfo(detector->roiInfoList()->at(i).name().remove(" "), detector->roiInfoList()->at(i).name()));

			if (detector->roiInfoList()->at(i).name().contains(edge))
				scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount() - 1), true, false);

			else
				scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount() - 1), false, true);
		}

		// Add all the extra raw data sources.
		addExtraDatasources();

		temp = AMMeasurementInfo(detector->toXRFInfo());
		temp.name = "corrSum";
		scan_->rawData()->addMeasurement(temp);
		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), true, false);

		for (int i = 0; i < detector->elements(); i++){

			temp = AMMeasurementInfo(detector->toXRFInfo());
			temp.name = QString("raw%1").arg(i+1);
			scan_->rawData()->addMeasurement(temp);
			scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount() - 1), false, true);
		}
	}

	else {

		// Add all the extra raw data sources.  Only needs to happen after the ion chambers for transmission.
		addExtraDatasources();
	}

	// Analysis blocks.
	if (config_->fluorescenceDetectorChoice() == VESPERSEXAFSScanConfiguration::SingleElement){

		AM2DSummingAB* pfy = new AM2DSummingAB("PFY");
		pfy->setDescription("PFY");
		QList<AMDataSource*> pfySource;
		pfySource << scan_->rawDataSources()->at(scan_->rawDataSourceCount()-1);
		pfy->setInputDataSources(pfySource);
		pfy->setSumAxis(1);
		pfy->setSumRangeMax(scan_->rawDataSources()->at(scan_->rawDataSourceCount()-1)->size(1)-1);
		scan_->addAnalyzedDataSource(pfy, true, false);

		AM1DExpressionAB *normPFY;
		normPFY = new AM1DExpressionAB("norm_PFY");
		normPFY->setDescription("Normalized PFY");
		normPFY->setInputDataSources(QList<AMDataSource *>() << scan_->rawDataSources()->at(0) << scan_->analyzedDataSources()->at(0));
		normPFY->setExpression(QString("%1/%2").arg(scan_->analyzedDataSources()->at(0)->name()).arg(scan_->rawDataSources()->at(0)->name()));
		scan_->addAnalyzedDataSource(normPFY, true, false);

		QString edge = config_->edge();

		// If this is an L edge, chop out the number for comparison purposes.
		if (edge.contains(QRegExp("L\\d$")))
			edge.chop(1);

		for (int i = 0; i < scan_->rawDataSourceCount(); i++){

			if (scan_->rawDataSources()->at(i)->name().contains(edge.remove(" "))){

				normPFY = new AM1DExpressionAB("norm_"+scan_->rawDataSources()->at(i)->name());
				normPFY->setDescription("Normalized "+scan_->rawDataSources()->at(i)->description());
				normPFY->setInputDataSources(QList<AMDataSource *>() << scan_->rawDataSources()->at(0) << scan_->rawDataSources()->at(i));
				normPFY->setExpression(QString("%1/%2").arg(scan_->rawDataSources()->at(i)->name()).arg(scan_->rawDataSources()->at(0)->name()));
				scan_->addAnalyzedDataSource(normPFY, true, false);
			}
		}

		AM1DExpressionAB* transmission = new AM1DExpressionAB("trans");
		transmission->setDescription("Transmission");
		transmission->setInputDataSources(QList<AMDataSource *>() << scan_->rawDataSources()->at(0) << scan_->rawDataSources()->at(1));
		transmission->setExpression(QString("ln(%1/%2)").arg(scan_->rawDataSources()->at(0)->name()).arg(scan_->rawDataSources()->at(1)->name()));

		scan_->addAnalyzedDataSource(transmission, true, false);
	}
	else if (config_->fluorescenceDetectorChoice() == VESPERSEXAFSScanConfiguration::FourElement){

		AM2DSummingAB* pfy = new AM2DSummingAB("PFY");
		pfy->setDescription("PFY");
		QList<AMDataSource*> pfySource;
		pfySource << scan_->rawDataSources()->at(scan_->rawDataSourceCount()-5);
		pfy->setInputDataSources(pfySource);
		pfy->setSumAxis(1);
		pfy->setSumRangeMax(scan_->rawDataSources()->at(scan_->rawDataSourceCount()-5)->size(1)-1);
		scan_->addAnalyzedDataSource(pfy, true, false);

		AM1DExpressionAB *normPFY;
		normPFY = new AM1DExpressionAB("norm_PFY");
		normPFY->setDescription("Normalized PFY");
		normPFY->setInputDataSources(QList<AMDataSource *>() << scan_->rawDataSources()->at(0) << scan_->analyzedDataSources()->at(0));
		normPFY->setExpression(QString("%1/%2").arg(scan_->analyzedDataSources()->at(0)->name()).arg(scan_->rawDataSources()->at(0)->name()));
		scan_->addAnalyzedDataSource(normPFY, true, false);

		QString edge = config_->edge();

		// If this is an L edge, chop out the number for comparison purposes.
		if (edge.contains(QRegExp("L\\d$")))
			edge.chop(1);

		for (int i = 0; i < scan_->rawDataSourceCount(); i++){

			if (scan_->rawDataSources()->at(i)->name().contains(edge.remove(" "))){

				normPFY = new AM1DExpressionAB("norm_"+scan_->rawDataSources()->at(i)->name());
				normPFY->setDescription("Normalized "+scan_->rawDataSources()->at(i)->description());
				normPFY->setInputDataSources(QList<AMDataSource *>() << scan_->rawDataSources()->at(0) << scan_->rawDataSources()->at(i));
				normPFY->setExpression(QString("%1/%2").arg(scan_->rawDataSources()->at(i)->name()).arg(scan_->rawDataSources()->at(0)->name()));
				scan_->addAnalyzedDataSource(normPFY, true, false);
			}
		}

		AM1DExpressionAB* transmission = new AM1DExpressionAB("trans");
		transmission->setDescription("Transmission");
		transmission->setInputDataSources(QList<AMDataSource *>() << scan_->rawDataSources()->at(0) << scan_->rawDataSources()->at(1));
		transmission->setExpression(QString("ln(%1/%2)").arg(scan_->rawDataSources()->at(0)->name()).arg(scan_->rawDataSources()->at(1)->name()));

		scan_->addAnalyzedDataSource(transmission, true, false);
	}

	else {

		AM1DExpressionAB* transmission = new AM1DExpressionAB("trans");
		transmission->setDescription("Transmission");
		transmission->setInputDataSources(QList<AMDataSource *>() << scan_->rawDataSources()->at(0) << scan_->rawDataSources()->at(1));
		transmission->setExpression(QString("ln(%1/%2)").arg(scan_->rawDataSources()->at(0)->name()).arg(scan_->rawDataSources()->at(1)->name()));

		scan_->addAnalyzedDataSource(transmission, true, false);
	}

	useDwellTimes(VESPERSBeamline::vespers()->dwellTimeTrigger(), VESPERSBeamline::vespers()->dwellTimeConfirmed());
}

void VESPERSEXAFSDacqScanController::addExtraDatasources()
{
	// Common to all three types are Ea, K, dwell time and ring current.
	scan_->rawData()->addMeasurement(AMMeasurementInfo("Ea", "Energy Setpoint", "eV"));
	scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);
	scan_->rawData()->addMeasurement(AMMeasurementInfo("K", "K Setpoint"));
	scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);
	scan_->rawData()->addMeasurement(AMMeasurementInfo("DwellTime", "Dwell Time", "s"));
	scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);
	scan_->rawData()->addMeasurement(AMMeasurementInfo("RingCurrent", "Ring Current", "mA"));
	scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);

	if (config_->fluorescenceDetectorChoice() == VESPERSEXAFSScanConfiguration::SingleElement){

		// Dead time, real time, live time, fast peaks, slow peaks, spectrum index.
		scan_->rawData()->addMeasurement(AMMeasurementInfo("DeadTime", "Dead Time", "%"));
		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);
		scan_->rawData()->addMeasurement(AMMeasurementInfo("RealTime", "Real Time", "s"));
		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);
		scan_->rawData()->addMeasurement(AMMeasurementInfo("LiveTime", "Live Time", "s"));
		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);
		scan_->rawData()->addMeasurement(AMMeasurementInfo("FastPeaks", "Fast Peaks"));
		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);
		scan_->rawData()->addMeasurement(AMMeasurementInfo("SlowPeaks", "Slow Peaks"));
		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);
	}

	else if (config_->fluorescenceDetectorChoice() == VESPERSEXAFSScanConfiguration::FourElement){

		// Real time (x4), Live time (x4), fast peaks (x4), slow peaks (x4), dead time (x4)
		scan_->rawData()->addMeasurement(AMMeasurementInfo("RealTime1", "Real Time 1", "s"));
		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);
		scan_->rawData()->addMeasurement(AMMeasurementInfo("RealTime2", "Real Time 2", "s"));
		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);
		scan_->rawData()->addMeasurement(AMMeasurementInfo("RealTime3", "Real Time 3", "s"));
		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);
		scan_->rawData()->addMeasurement(AMMeasurementInfo("RealTime4", "Real Time 4", "s"));
		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);
		scan_->rawData()->addMeasurement(AMMeasurementInfo("LiveTime1", "Live Time 1", "s"));
		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);
		scan_->rawData()->addMeasurement(AMMeasurementInfo("LiveTime2", "Live Time 2", "s"));
		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);
		scan_->rawData()->addMeasurement(AMMeasurementInfo("LiveTime3", "Live Time 3", "s"));
		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);
		scan_->rawData()->addMeasurement(AMMeasurementInfo("LiveTime4", "Live Time 4", "s"));
		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);
		scan_->rawData()->addMeasurement(AMMeasurementInfo("FastPeaks1", "Fast Peaks 1"));
		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);
		scan_->rawData()->addMeasurement(AMMeasurementInfo("FastPeaks2", "Fast Peaks 2"));
		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);
		scan_->rawData()->addMeasurement(AMMeasurementInfo("FastPeaks3", "Fast Peaks 3"));
		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);
		scan_->rawData()->addMeasurement(AMMeasurementInfo("FastPeaks4", "Fast Peaks 4"));
		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);
		scan_->rawData()->addMeasurement(AMMeasurementInfo("SlowPeaks1", "Slow Peaks 1"));
		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);
		scan_->rawData()->addMeasurement(AMMeasurementInfo("SlowPeaks2", "Slow Peaks 2"));
		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);
		scan_->rawData()->addMeasurement(AMMeasurementInfo("SlowPeaks3", "Slow Peaks 3"));
		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);
		scan_->rawData()->addMeasurement(AMMeasurementInfo("SlowPeaks4", "Slow Peaks 4"));
		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);
		scan_->rawData()->addMeasurement(AMMeasurementInfo("DeadTime1", "Dead Time 1", "%"));
		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);
		scan_->rawData()->addMeasurement(AMMeasurementInfo("DeadTime2", "Dead Time 2", "%"));
		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);
		scan_->rawData()->addMeasurement(AMMeasurementInfo("DeadTime3", "Dead Time 3", "%"));
		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);
		scan_->rawData()->addMeasurement(AMMeasurementInfo("DeadTime4", "Dead Time 4", "%"));
		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);
	}
}

bool VESPERSEXAFSDacqScanController::initializeImplementation()
{
	// To initialize the XAS scan, there are four stages.
	/*
		First: Enable/Disable all the pertinent detectors.  The scalar is ALWAYS enabled.
		Second: Set the mode to single shot,set the time on the synchronized dwell time, and set the variable integration time if an EXAFS scan.
		Third: Make sure the relative energy postion is back to zero and move the sample stage to the correct location (if enabled).
		Fourth: Move the mono to the correct energy.
	 */
	AMBeamlineParallelActionsList *setupXASActionsList = new AMBeamlineParallelActionsList;

	if (!setupXASAction_)
		onInitializationActionFinished();

	setupXASAction_ = new AMBeamlineListAction(setupXASActionsList);

	// First stage.
	setupXASActionsList->appendStage(new QList<AMBeamlineActionItem*>());
	// Scalar
	setupXASActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementAt(0)->createEnableAction(true));
	// Single element vortex
	if (config_->fluorescenceDetectorChoice() == VESPERSEXAFSScanConfiguration::SingleElement)
		setupXASActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementAt(1)->createEnableAction(true));
	else
		setupXASActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementAt(1)->createEnableAction(false));
	// CCD
	setupXASActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementAt(2)->createEnableAction(false));
	// Picoammeters
	setupXASActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementAt(3)->createEnableAction(false));
	// Four element vortex
	if (config_->fluorescenceDetectorChoice() == VESPERSEXAFSScanConfiguration::FourElement)
		setupXASActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementAt(4)->createEnableAction(true));
	else
		setupXASActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementAt(4)->createEnableAction(false));

	// Second stage.
	setupXASActionsList->appendStage(new QList<AMBeamlineActionItem*>());
	setupXASActionsList->appendAction(1, VESPERSBeamline::vespers()->scaler()->createStartAction(false));
	setupXASActionsList->appendAction(1, VESPERSBeamline::vespers()->scaler()->createScansPerBufferAction(1));
	setupXASActionsList->appendAction(1, VESPERSBeamline::vespers()->scaler()->createTotalScansAction(1));
	setupXASActionsList->appendAction(1, VESPERSBeamline::vespers()->synchronizedDwellTime()->createModeAction(CLSSynchronizedDwellTime::SingleShot));
	setupXASActionsList->appendAction(1, VESPERSBeamline::vespers()->synchronizedDwellTime()->createMasterTimeAction(config_->regionTime(0)));
	if (config_->exafsRegions()->hasKSpace() && !config_->useFixedTime()){

		int regionCount = config_->regionCount();
		double time = (regionCount > 1) ? config_->regionTime(regionCount - 2) : 1; // Grab the time from the region before the EXAFS region or default it to 1 second.
		setupXASActionsList->appendAction(1, VESPERSBeamline::vespers()->variableIntegrationTime()->createSetupAction(CLSVariableIntegrationTime::EnabledwThreshold,
																													  time,
																													  config_->regionStart(regionCount - 1),
																													  VESPERSBeamline::vespers()->variableIntegrationTime()->function(),
																													  config_->regionStart(regionCount - 1),
																													  config_->regionEnd(regionCount - 1),
																													  config_->regionTime(regionCount - 1)));
	}

	// Third stage.
	setupXASActionsList->appendStage(new QList<AMBeamlineActionItem *>());
	setupXASActionsList->appendAction(2, VESPERSBeamline::vespers()->mono()->createDelEAction(0));
	if (config_->goToPosition()){

		setupXASActionsList->appendAction(2, VESPERSBeamline::vespers()->pseudoSampleStage()->createHorizontalMoveAction(config_->x()));
		setupXASActionsList->appendAction(2, VESPERSBeamline::vespers()->pseudoSampleStage()->createVerticalMoveAction(config_->y()));
	}

	// Fourth stage.
	setupXASActionsList->appendStage(new QList<AMBeamlineActionItem *>());
	setupXASActionsList->appendAction(3, VESPERSBeamline::vespers()->mono()->createEoAction(config_->energy()));

	// Integrity check.  Make sure no actions are null.
	for (int i = 0; i < setupXASActionsList->stageCount(); i++){

		for (int j = 0; j < setupXASActionsList->stage(i)->size(); j++){

			if (setupXASActionsList->action(i, j) == 0){

				onInitializationActionsFailed(0);
				return false;
			}
		}
	}

	connect(setupXASAction_, SIGNAL(succeeded()), this, SLOT(onInitializationActionsSucceeded()));
	connect(setupXASAction_, SIGNAL(failed(int)), this, SLOT(onInitializationActionsFailed(int)));
	connect(setupXASAction_, SIGNAL(progress(double,double)), this, SLOT(onInitializationActionsProgress(double,double)));
	setupXASAction_->start();

	return true;
}

bool VESPERSEXAFSDacqScanController::startImplementation()
{
	currentRegionIndex_ = 0;

	scan_->scanInitialConditions()->append(VESPERSBeamline::vespers()->sampleStageHorizontal()->toInfo());
	scan_->scanInitialConditions()->append(VESPERSBeamline::vespers()->sampleStageVertical()->toInfo());

	// Setup the real config.
	switch(config_->fluorescenceDetectorChoice()){

	case VESPERSEXAFSScanConfiguration::None:
		if (!setupTransmissionXAS()){

			AMErrorMon::alert(this,
					VESPERSEXAFSDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
					"Error, VESPERS EXAFS DACQ Scan Controller failed to start (the config file failed to load). Please report this bug to the Acquaman developers.");
			return false;
		}
		break;

	case VESPERSEXAFSScanConfiguration::SingleElement:
		if (!setupSingleElementXAS()){

			AMErrorMon::alert(this,
					VESPERSEXAFSDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
					"Error, VESPERS EXAFS DACQ Scan Controller failed to start (the config file failed to load). Please report this bug to the Acquaman developers.");
			return false;
		}
		break;

	case VESPERSEXAFSScanConfiguration::FourElement:
		if (!setupFourElementXAS()){

			AMErrorMon::alert(this,
					VESPERSEXAFSDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
					"Error, VESPERS EXAFS DACQ Scan Controller failed to start (the config file failed to load). Please report this bug to the Acquaman developers.");
			return false;
		}
		break;

	default:
		AMErrorMon::alert(this,
				VESPERSEXAFSDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
				"Error, VESPERS EXAFS DACQ Scan Controller failed to start (Invalid Fluorescence Detector chosen). Please report this bug to the Acquaman developers.");
		return false;
	}

	for (int i = 0; i < config_->regionCount(); i++){

		if (advAcq_->getNumRegions() == i){

			AMPVwStatusControl *control = 0;

			if (config_->regionType(i) == AMEXAFSRegion::Energy){

				control = qobject_cast<AMPVwStatusControl *>(config_->regions()->defaultControl());

				if (i == 0)
					advAcq_->addRegion(i, control->writePVName(), config_->regionStart(i), config_->regionDelta(i), config_->regionEnd(i), 1);
				else
					advAcq_->addRegion(i, control->writePVName(), config_->regionStart(i)+config_->regionDelta(i), config_->regionDelta(i), config_->regionEnd(i), 1);
			}
			else if (config_->regionType(i) == AMEXAFSRegion::kSpace){

				control = qobject_cast<AMPVwStatusControl *>(config_->exafsRegions()->defaultKControl());

				if (i == 0)
					advAcq_->addRegion(i, control->writePVName(), config_->regionStartByType(i, AMEXAFSRegion::kSpace), config_->regionDelta(i), config_->regionEnd(i), 1);
				else
					advAcq_->addRegion(i, control->writePVName(), config_->regionStartByType(i, AMEXAFSRegion::kSpace)+config_->regionDelta(i), config_->regionDelta(i), config_->regionEnd(i), 1);
			}
		}

		else {

			if (i == 0)
				advAcq_->setStart(i, config_->regionStart(i));
			else
				advAcq_->setStart(i, config_->regionStart(i)+config_->regionDelta(i));

			advAcq_->setDelta(i, config_->regionDelta(i));
			advAcq_->setEnd(i, config_->regionEnd(i));
		}
	}
//	advAcq_->saveConfigFile("/home/hunterd/Desktop/writeTest.cfg");

	return AMDacqScanController::startImplementation();
}

void VESPERSEXAFSDacqScanController::cleanup()
{
	// To cleanup the XAS scan, there is one stage.
	/*
		First: Set the dwell time to 1 second.  Disables the variable integration time.  Set the relative energy PV to 0.
  Second: Set the scan mode to continuous.  This starts the synchronized dwell time.
	 */
	AMBeamlineParallelActionsList *cleanupXASActionsList = new AMBeamlineParallelActionsList;

	if (!cleanupXASAction_)
		onCleanupActionFinished();

	cleanupXASAction_ = new AMBeamlineListAction(cleanupXASActionsList);

	// First stage.
	cleanupXASActionsList->appendStage(new QList<AMBeamlineActionItem*>());
	// Synchronized dwell time.
	cleanupXASActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->createMasterTimeAction(1.0));
	// Variable integration time.
	cleanupXASActionsList->appendAction(0, VESPERSBeamline::vespers()->variableIntegrationTime()->createModeAction(CLSVariableIntegrationTime::Disabled));
	// Energy.
	cleanupXASActionsList->appendAction(0, VESPERSBeamline::vespers()->mono()->createDelEAction(0));

	// Second stage.
	cleanupXASActionsList->appendStage(new QList<AMBeamlineActionItem *>());
	// Start the synchronized dwell time.
	cleanupXASActionsList->appendAction(1, VESPERSBeamline::vespers()->synchronizedDwellTime()->createModeAction(CLSSynchronizedDwellTime::Continuous));

	connect(cleanupXASAction_, SIGNAL(succeeded()), this, SLOT(onCleanupFinished()));
	connect(cleanupXASAction_, SIGNAL(failed(int)), this, SLOT(onCleanupFinished()));
	cleanupXASAction_->start();
}

void VESPERSEXAFSDacqScanController::onCleanupFinished()
{
	AMDacqScanController::onDacqStop();
}

void VESPERSEXAFSDacqScanController::onDwellTimeTriggerChanged(double newValue)
{
	if( fabs(newValue - 1.0) < 0.1){

		// Only set the time for the region if in energy space or if told explicitly to do so.  The variable integration time app handles this in k-space.
		if ((config_->regionType(currentRegionIndex_) == AMEXAFSRegion::Energy) || (config_->regionType(currentRegionIndex_) == AMEXAFSRegion::kSpace && config_->useFixedTime()))
			VESPERSBeamline::vespers()->synchronizedDwellTime()->setTime(config_->regionTime(currentRegionIndex_));

		currentRegionIndex_++;
		dwellTimeTrigger_->move(0);
		dwellTimeConfirmed_->move(1);
	}
}

AMnDIndex VESPERSEXAFSDacqScanController::toScanIndex(QMap<int, double> aeData)
{
	Q_UNUSED(aeData)
	return AMnDIndex(scan_->rawData()->scanSize(0));
}

void VESPERSEXAFSDacqScanController::onScanTimerUpdate()
{
	if (elapsedTime_.isActive()){

		if (secondsElapsed_ >= secondsTotal_)
			secondsElapsed_ = secondsTotal_;
		else
			secondsElapsed_ += 1.0;

		emit progress(secondsElapsed_, secondsTotal_);
	}
}

void VESPERSEXAFSDacqScanController::onInitializationActionsSucceeded()
{
	setInitialized();
}

void VESPERSEXAFSDacqScanController::onInitializationActionsFailed(int explanation)
{
	Q_UNUSED(explanation)

	AMErrorMon::alert(this, 0, "XAS scan failed to initialize.");
	onInitializationActionFinished();
	setFailed();
}

void VESPERSEXAFSDacqScanController::onInitializationActionsProgress(double elapsed, double total)
{
	Q_UNUSED(elapsed)
	Q_UNUSED(total)
}

QString VESPERSEXAFSDacqScanController::getHomeDirectory()
{
	// Find out which path we are using for acquaman (depends on whether you are on Mac or Linux or beamline OPI).
	QString homeDir = QDir::homePath();
	if(QDir(homeDir+"/dev").exists())
		homeDir.append("/dev");
	else if(QDir(homeDir+"/beamline/programming").exists())
		homeDir.append("/beamline/programming");

	return homeDir;
}

bool VESPERSEXAFSDacqScanController::setupTransmissionXAS()
{
	bool loadSuccess = false;

	loadSuccess = advAcq_->setConfigFile(getHomeDirectory().append("/acquaman/devConfigurationFiles/VESPERS/XANES.cfg"));

	if(!loadSuccess){
		AMErrorMon::alert(this,
				VESPERSEXAFSDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
				"Error, VESPERS XAS DACQ Scan Controller failed to start (the config file failed to load). Please report this bug to the Acquaman developers.");
		return false;
	}

	AMDetectorSet *ionChambers = VESPERSBeamline::vespers()->ionChambers();

	advAcq_->appendRecord(VESPERSBeamline::vespers()->pvName(ionChambers->detectorAt((int)config_->incomingChoice())->detectorName()), true, false, detectorReadMethodToDacqReadMethod(ionChambers->detectorAt((int)config_->incomingChoice())->readMethod()));
	advAcq_->appendRecord(VESPERSBeamline::vespers()->pvName(ionChambers->detectorAt((int)config_->transmissionChoice())->detectorName()), true, false, detectorReadMethodToDacqReadMethod(ionChambers->detectorAt((int)config_->transmissionChoice())->readMethod()));

	for (int i = 0; i < ionChambers->count(); i++)
		if (i != (int)config_->incomingChoice() && i != (int)config_->transmissionChoice())
			advAcq_->appendRecord(VESPERSBeamline::vespers()->pvName(ionChambers->detectorAt(i)->detectorName()), true, false, detectorReadMethodToDacqReadMethod(ionChambers->detectorAt(i)->readMethod()));

	/// In order to mimic the current configs, I've hardcoded all the names so that the file matches the reference file.  These should and will be migrated to proper maps of detectors and controls names.
	// These will all likely change and be modified.
	advAcq_->appendRecord("07B2_Mono_SineB_Ea", true, false, 0);
	advAcq_->appendRecord("07B2_Mono_SineB_K", true, false, 0);
	advAcq_->appendRecord("BL1607-B2-1:dwell:setTime", true, false, 0);
	advAcq_->appendRecord("PCT1402-01:mA:fbk", true, false, 0);

	return loadSuccess;
}

bool VESPERSEXAFSDacqScanController::setupSingleElementXAS()
{
	bool loadSuccess = false;

	loadSuccess = advAcq_->setConfigFile(getHomeDirectory().append("/acquaman/devConfigurationFiles/VESPERS/XANES-1Elem.cfg"));

	if(!loadSuccess){
		AMErrorMon::alert(this,
				VESPERSEXAFSDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
				"Error, VESPERS XAS DACQ Scan Controller failed to start (the config file failed to load). Please report this bug to the Acquaman developers.");
		return false;
	}

	usingSpectraDotDatFile_ = true;

	// Remove all the "goober" records that were added to create enough space for the Dacq.  (Hack the Dacq solution).
	while (advAcq_->deleteRecord(1)){}

	AMDetectorSet *ionChambers = VESPERSBeamline::vespers()->ionChambers();

	/// In order to mimic the current configs, I've hardcoded all the names so that the file matches the reference file.  These should and will be migrated to proper maps of detectors and controls names.
	// These will all likely change and be modified.
	advAcq_->appendRecord(VESPERSBeamline::vespers()->pvName(ionChambers->detectorAt((int)config_->incomingChoice())->detectorName()), true, false, detectorReadMethodToDacqReadMethod(ionChambers->detectorAt((int)config_->incomingChoice())->readMethod()));
	advAcq_->appendRecord(VESPERSBeamline::vespers()->pvName(ionChambers->detectorAt((int)config_->transmissionChoice())->detectorName()), true, false, detectorReadMethodToDacqReadMethod(ionChambers->detectorAt((int)config_->transmissionChoice())->readMethod()));

	for (int i = 0; i < ionChambers->count(); i++)
		if (i != (int)config_->incomingChoice() && i != (int)config_->transmissionChoice())
			advAcq_->appendRecord(VESPERSBeamline::vespers()->pvName(ionChambers->detectorAt(i)->detectorName()), true, false, detectorReadMethodToDacqReadMethod(ionChambers->detectorAt(i)->readMethod()));

	XRFDetector *detector = VESPERSBeamline::vespers()->vortexXRF1E();
	int roiCount = detector->roiInfoList()->count();

	for (int i = 0; i < roiCount; i++)
		advAcq_->appendRecord("IOC1607-004:mca1.R"+QString::number(i), true, false, 0);

	advAcq_->appendRecord("07B2_Mono_SineB_Ea", true, false, 0);
	advAcq_->appendRecord("07B2_Mono_SineB_K", true, false, 0);
	advAcq_->appendRecord("BL1607-B2-1:dwell:setTime", true, false, 0);
	advAcq_->appendRecord("PCT1402-01:mA:fbk", true, false, 0);
	advAcq_->appendRecord("IOC1607-004:mca1.DTIM", true, false, 0);
	advAcq_->appendRecord("IOC1607-004:mca1.ERTM", true, false, 0);
	advAcq_->appendRecord("IOC1607-004:mca1.ELTM", true, false, 0);
	advAcq_->appendRecord("IOC1607-004:dxp1.FAST_PEAKS", true, false, 0);
	advAcq_->appendRecord("IOC1607-004:dxp1.SLOW_PEAKS", true, false, 0);

	advAcq_->appendRecord("IOC1607-004:mca1", true, true, 1);

	// End of hardcoded.

	return loadSuccess;
}

bool VESPERSEXAFSDacqScanController::setupFourElementXAS()
{
	bool loadSuccess = false;

	loadSuccess = advAcq_->setConfigFile(getHomeDirectory().append("/acquaman/devConfigurationFiles/VESPERS/XANES-4Elem.cfg"));

	if(!loadSuccess){
		AMErrorMon::alert(this,
				VESPERSEXAFSDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
				"Error, VESPERS XAS DACQ Scan Controller failed to start (the config file failed to load). Please report this bug to the Acquaman developers.");
		return false;
	}

	usingSpectraDotDatFile_ = true;

	// Remove all the "goober" records that were added to create enough space for the Dacq.  (Hack the Dacq solution).
	while (advAcq_->deleteRecord(1)){}

	AMDetectorSet *ionChambers = VESPERSBeamline::vespers()->ionChambers();

	/// In order to mimic the current configs, I've hardcoded all the names so that the file matches the reference file.  These should and will be migrated to proper maps of detectors and controls names.
	// These will all likely change and be modified.
	advAcq_->appendRecord(VESPERSBeamline::vespers()->pvName(ionChambers->detectorAt((int)config_->incomingChoice())->detectorName()), true, false, detectorReadMethodToDacqReadMethod(ionChambers->detectorAt((int)config_->incomingChoice())->readMethod()));
	advAcq_->appendRecord(VESPERSBeamline::vespers()->pvName(ionChambers->detectorAt((int)config_->transmissionChoice())->detectorName()), true, false, detectorReadMethodToDacqReadMethod(ionChambers->detectorAt((int)config_->transmissionChoice())->readMethod()));

	for (int i = 0; i < ionChambers->count(); i++)
		if (i != (int)config_->incomingChoice() && i != (int)config_->transmissionChoice())
			advAcq_->appendRecord(VESPERSBeamline::vespers()->pvName(ionChambers->detectorAt(i)->detectorName()), true, false, detectorReadMethodToDacqReadMethod(ionChambers->detectorAt(i)->readMethod()));

	XRFDetector *detector = VESPERSBeamline::vespers()->vortexXRF4E();
	int roiCount = detector->roiInfoList()->count();

	for (int i = 0; i < roiCount; i++)
		advAcq_->appendRecord("dxp1607-B21-04:mcaCorrected.R"+QString::number(i), true, false, 1);

	advAcq_->appendRecord("07B2_Mono_SineB_Ea", true, false, 0);
	advAcq_->appendRecord("07B2_Mono_SineB_K", true, false, 0);
	advAcq_->appendRecord("BL1607-B2-1:dwell:setTime", true, false, 0);
	advAcq_->appendRecord("PCT1402-01:mA:fbk", true, false, 0);
	advAcq_->appendRecord("dxp1607-B21-04:mca1.ERTM", true, false, 0);
	advAcq_->appendRecord("dxp1607-B21-04:mca2.ERTM", true, false, 0);
	advAcq_->appendRecord("dxp1607-B21-04:mca3.ERTM", true, false, 0);
	advAcq_->appendRecord("dxp1607-B21-04:mca4.ERTM", true, false, 0);
	advAcq_->appendRecord("dxp1607-B21-04:mca1.ELTM", true, false, 0);
	advAcq_->appendRecord("dxp1607-B21-04:mca2.ELTM", true, false, 0);
	advAcq_->appendRecord("dxp1607-B21-04:mca3.ELTM", true, false, 0);
	advAcq_->appendRecord("dxp1607-B21-04:mca4.ELTM", true, false, 0);
	advAcq_->appendRecord("dxp1607-B21-04:dxp1.FAST_PEAKS", true, false, 1);
	advAcq_->appendRecord("dxp1607-B21-04:dxp2.FAST_PEAKS", true, false, 1);
	advAcq_->appendRecord("dxp1607-B21-04:dxp3.FAST_PEAKS", true, false, 1);
	advAcq_->appendRecord("dxp1607-B21-04:dxp4.FAST_PEAKS", true, false, 1);
	advAcq_->appendRecord("dxp1607-B21-04:dxp1.SLOW_PEAKS", true, false, 1);
	advAcq_->appendRecord("dxp1607-B21-04:dxp2.SLOW_PEAKS", true, false, 1);
	advAcq_->appendRecord("dxp1607-B21-04:dxp3.SLOW_PEAKS", true, false, 1);
	advAcq_->appendRecord("dxp1607-B21-04:dxp4.SLOW_PEAKS", true, false, 1);
	advAcq_->appendRecord("dxp1607-B21-04:mca1.DTIM", true, false, 1);
	advAcq_->appendRecord("dxp1607-B21-04:mca2.DTIM", true, false, 1);
	advAcq_->appendRecord("dxp1607-B21-04:mca3.DTIM", true, false, 1);
	advAcq_->appendRecord("dxp1607-B21-04:mca4.DTIM", true, false, 1);

	advAcq_->appendRecord("dxp1607-B21-04:mcaCorrected", true, true, 0);
	advAcq_->appendRecord("dxp1607-B21-04:mca1", true, true, 0);
	advAcq_->appendRecord("dxp1607-B21-04:mca2", true, true, 0);
	advAcq_->appendRecord("dxp1607-B21-04:mca3", true, true, 0);
	advAcq_->appendRecord("dxp1607-B21-04:mca4", true, true, 0);

	// End of hardcored.

	return loadSuccess;
}

void VESPERSEXAFSDacqScanController::onInitializationActionFinished()
{
	if (setupXASAction_ == 0)
		return;

	// Disconnect all signals and return all memory.
	setupXASAction_->disconnect();
	AMBeamlineParallelActionsList *actionList = setupXASAction_->list()	;

	for (int i = 0; i < actionList->stageCount(); i++){

		while (actionList->stage(i)->size())
			actionList->stage(i)->takeAt(0)->deleteLater();
	}

	setupXASAction_->deleteLater();
	setupXASAction_ = 0;
}

void VESPERSEXAFSDacqScanController::onCleanupActionFinished()
{
	if (cleanupXASAction_ == 0)
		return;

	// Disconnect all signals and return all memory.
	cleanupXASAction_->disconnect();
	AMBeamlineParallelActionsList *actionList = cleanupXASAction_->list()	;

	for (int i = 0; i < actionList->stageCount(); i++){

		while (actionList->stage(i)->size())
			actionList->stage(i)->takeAt(0)->deleteLater();
	}

	cleanupXASAction_->deleteLater();
	cleanupXASAction_ = 0;
}
