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


#include "VESPERS2DDacqScanController.h"

#include "beamline/VESPERS/VESPERSBeamline.h"
#include "actions/AMBeamlineActionsList.h"
#include "dataman/AMUser.h"
#include "analysis/AM2DNormalizationAB.h"
#include "analysis/AM2DAdditionAB.h"
#include "actions/AMBeamlineParallelActionsList.h"
#include "util/VESPERS/VESPERSConfigurationFileBuilder.h"
#include "analysis/AM3DAdditionAB.h"

#include <QDir>
#include <QStringBuilder>

VESPERS2DDacqScanController::VESPERS2DDacqScanController(VESPERS2DScanConfiguration *cfg, QObject *parent)
	: AM2DDacqScanController(cfg, parent)
{
	config_ = cfg;
	config_->setUserScanName(config_->name());

	initializationActions_ = 0;
	cleanupActions_ = 0;

	secondsElapsed_ = 0;
	secondsTotal_ = config_->totalTime(true);
	elapsedTime_.setInterval(1000);
	connect(this, SIGNAL(started()), &elapsedTime_, SLOT(start()));
	connect(this, SIGNAL(cancelled()), &elapsedTime_, SLOT(stop()));
	connect(this, SIGNAL(paused()), &elapsedTime_, SLOT(stop()));
	connect(this, SIGNAL(resumed()), &elapsedTime_, SLOT(start()));
	connect(this, SIGNAL(failed()), &elapsedTime_, SLOT(stop()));
	connect(this, SIGNAL(finished()), &elapsedTime_, SLOT(stop()));
	connect(&elapsedTime_, SIGNAL(timeout()), this, SLOT(onScanTimerUpdate()));

	scan_ = new AM2DScan(); 	// MB: Moved from line 363 in startImplementation.
	scan_->setName(config_->name());
	scan_->setScanConfiguration(config_);
	scan_->setRunId(AMUser::user()->currentRunId());
	scan_->setIndexType("fileSystem");

	int yPoints = int((config_->yEnd() - config_->yStart())/config_->yStep());
	if ((config_->yEnd() - config_->yStart() - (yPoints + 0.01)*config_->yStep()) < 0)
		yPoints += 1;
	else
		yPoints += 2;

	AMPVwStatusControl *control = 0;

	switch(config_->motorsChoice()){

	case VESPERS2DScanConfiguration::HAndV:
		control = qobject_cast<AMPVwStatusControl *>(VESPERSBeamline::vespers()->pseudoSampleStage()->horiz());
		xAxisPVName_ = control != 0 ? control->writePVName() : "";
		control = qobject_cast<AMPVwStatusControl *>(VESPERSBeamline::vespers()->pseudoSampleStage()->vert());
		yAxisPVName_ = control != 0 ? control->writePVName() : "";
		scan_->rawData()->addScanAxis(AMAxisInfo("H", 0, "Horizontal Position", "mm"));
		scan_->rawData()->addScanAxis(AMAxisInfo("V", yPoints, "Vertical Position", "mm"));
		break;

	case VESPERS2DScanConfiguration::XAndZ:
		control = qobject_cast<AMPVwStatusControl *>(VESPERSBeamline::vespers()->sampleStageX());
		xAxisPVName_ = control != 0 ? control->writePVName() : "";
		control = qobject_cast<AMPVwStatusControl *>(VESPERSBeamline::vespers()->sampleStageZ());
		yAxisPVName_ = control != 0 ? control->writePVName() : "";
		scan_->rawData()->addScanAxis(AMAxisInfo("X", 0, "Horizontal Position", "mm"));
		scan_->rawData()->addScanAxis(AMAxisInfo("Z", yPoints, "Vertical Position", "mm"));
		break;

	default:
		xAxisPVName_ = "";
		yAxisPVName_ = "";
		break;
	}

	if (config_->fluorescenceDetectorChoice() == VESPERS2DScanConfiguration::SingleElement && !config_->usingCCD())
		scan_->setFileFormat("vespers2012XRF1El");

	else if (config_->fluorescenceDetectorChoice() == VESPERS2DScanConfiguration::SingleElement && config_->usingCCD())
		scan_->setFileFormat("vespers2012XRF1ElXRD");

	else if (config_->fluorescenceDetectorChoice() == VESPERS2DScanConfiguration::FourElement && !config_->usingCCD())
		scan_->setFileFormat("vespers2012XRF4El");

	else if (config_->fluorescenceDetectorChoice() == VESPERS2DScanConfiguration::FourElement && config_->usingCCD())
		scan_->setFileFormat("vespers2012XRF4ElXRD");

	else if (config_->fluorescenceDetectorChoice() == (VESPERS2DScanConfiguration::SingleElement | VESPERS2DScanConfiguration::FourElement) && !config_->usingCCD())
		scan_->setFileFormat("vespers2012XRF1Eln4El");

	else if (config_->fluorescenceDetectorChoice() == (VESPERS2DScanConfiguration::SingleElement | VESPERS2DScanConfiguration::FourElement) && config_->usingCCD())
		scan_->setFileFormat("vespers2012XRF1Eln4ElXRD");

	else
		AMErrorMon::error(this, VESPERS2DDACQSCANCONTROLLER_CANT_INTIALIZE, "Could not recognize the format type of the scan.");

	QString notes;

	switch ((int)config_->fluorescenceDetectorChoice()){

	case VESPERS2DScanConfiguration::None:
		break;

	case VESPERS2DScanConfiguration::SingleElement:
		notes.append(QString("\nFluorescence detector distance to sample:\t%1 mm\n").arg(VESPERSBeamline::vespers()->endstation()->distanceToSingleElementVortex(), 0, 'f', 1));
		break;

	case VESPERS2DScanConfiguration::FourElement:
		notes.append(QString("\nFluorescence detector distance to sample:\t%1 mm\n").arg(VESPERSBeamline::vespers()->endstation()->distanceToFourElementVortex(), 0, 'f', 1));
		break;

	case VESPERS2DScanConfiguration::SingleElement | VESPERS2DScanConfiguration::FourElement:
		notes.append(QString("\nFluorescence detector (1-el Vortex) distance to sample:\t%1 mm\n").arg(VESPERSBeamline::vespers()->endstation()->distanceToSingleElementVortex(), 0, 'f', 1));
		notes.append(QString("\nFluorescence detector (4-el Vortex) distance to sample:\t%1 mm\n").arg(VESPERSBeamline::vespers()->endstation()->distanceToFourElementVortex(), 0, 'f', 1));
		break;
	}

	if (config_->usingCCD())
		notes.append(QString("CCD detector distance to sample:\t%1 mm\n").arg(VESPERSBeamline::vespers()->endstation()->distanceToRoperCCD(), 0, 'f', 1));

	switch(VESPERSBeamline::vespers()->currentBeam()){

	case VESPERSBeamline::None:
		// This should never happen.
		break;

	case VESPERSBeamline::Pink:
		notes.append("Beam used:\tPink\n");
		break;

	case VESPERSBeamline::TenPercent:
		notes.append(QString("Beam used:\t10% bandpass\nMonochromator energy:%1 eV\n").arg(VESPERSBeamline::vespers()->mono()->energy(), 0, 'f', 2));
		break;

	case VESPERSBeamline::OnePointSixPercent:
		notes.append(QString("Beam used:\t1.6% bandpass\nMonochromator energy:%1 eV\n").arg(VESPERSBeamline::vespers()->mono()->energy(), 0, 'f', 2));
		break;

	case VESPERSBeamline::Si:
		notes.append(QString("Beam used:\tSi (%2E/E = 10^-4)\nMonochromator energy:%1 eV\n").arg(VESPERSBeamline::vespers()->mono()->energy(), 0, 'f', 2).arg(QString::fromUtf8("Δ")));
		break;
	}

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

	// Add all the data sources.
	////////////////////////////////////////////////

	// Add the feedback coordinates.
	if (config_->motorsChoice() == VESPERS2DScanConfiguration::HAndV){

		scan_->rawData()->addMeasurement(AMMeasurementInfo("H:fbk", "Horizontal Feedback", "mm"));
		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);
		scan_->rawData()->addMeasurement(AMMeasurementInfo("V:fbk", "Vertical Feedback", "mm"));
		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);
	}

	else {

		scan_->rawData()->addMeasurement(AMMeasurementInfo("X:fbk", "Horizontal Feedback", "mm"));
		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);
		scan_->rawData()->addMeasurement(AMMeasurementInfo("Z:fbk", "Vertical Feedback", "mm"));
		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);
	}

	XRFDetector *detector = 0;

	switch ((int)config_->fluorescenceDetectorChoice()){

	case VESPERS2DScanConfiguration::None:
		break;

	case VESPERS2DScanConfiguration::SingleElement:
	case VESPERS2DScanConfiguration::FourElement:{

		if (config_->fluorescenceDetectorChoice() == VESPERS2DScanConfiguration::SingleElement)
			detector = VESPERSBeamline::vespers()->vortexXRF1E();
		else if (config_->fluorescenceDetectorChoice() == VESPERS2DScanConfiguration::FourElement)
			detector = VESPERSBeamline::vespers()->vortexXRF4E();

		// This should never happen.
		if (!detector)
			return;

		int roiCount = detector->roiInfoList()->count();

		// This is safe and okay because I always have the regions of interest set taking up 0-X where X is the count-1 of the number of regions of interest.
		for (int i = 0; i < roiCount; i++){

			scan_->rawData()->addMeasurement(AMMeasurementInfo(detector->roiInfoList()->at(i).name().remove(" "), detector->roiInfoList()->at(i).name()));
			scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount() - 1), false, true);
		}

		break;
	}

	case VESPERS2DScanConfiguration::SingleElement | VESPERS2DScanConfiguration::FourElement:{

		detector = VESPERSBeamline::vespers()->vortexXRF1E();

		// This should never happen.
		if (!detector)
			return;

		int roiCount = detector->roiInfoList()->count();

		// This is safe and okay because I always have the regions of interest set taking up 0-X where X is the count-1 of the number of regions of interest.
		for (int i = 0; i < roiCount; i++){

			scan_->rawData()->addMeasurement(AMMeasurementInfo(detector->roiInfoList()->at(i).name().remove(" ") % QString("-1"), detector->roiInfoList()->at(i).name()));
			scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount() - 1), false, true);
		}

		detector = VESPERSBeamline::vespers()->vortexXRF4E();

		// This should never happen.
		if (!detector)
			return;

		roiCount = detector->roiInfoList()->count();

		// This is safe and okay because I always have the regions of interest set taking up 0-X where X is the count-1 of the number of regions of interest.
		for (int i = 0; i < roiCount; i++){

			scan_->rawData()->addMeasurement(AMMeasurementInfo(detector->roiInfoList()->at(i).name().remove(" ") % "-4", detector->roiInfoList()->at(i).name()));
			scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount() - 1), false, true);
		}

		break;
	}
	}

	// Add the rest (includes the ion chambers).  This sets I0 as well; it is the only visible raw data source.
	addExtraDatasources();

	// Add analysis blocks.
	QList<AMDataSource *> i0List(QList<AMDataSource *>() << scan_->dataSourceAt(scan_->indexOfDataSource("Isplit"))
															<< scan_->dataSourceAt(scan_->indexOfDataSource("Iprekb"))
															<< scan_->dataSourceAt(scan_->indexOfDataSource("Imini")));

	QString i0Name("");

	switch (config_->incomingChoice()){

	case VESPERS2DScanConfiguration::Isplit:
		i0Name = i0List.at(0)->name();
		break;

	case VESPERS2DScanConfiguration::Iprekb:
		i0Name = i0List.at(1)->name();
		break;

	case VESPERS2DScanConfiguration::Imini:
		i0Name = i0List.at(2)->name();
		break;

	case VESPERS2DScanConfiguration::Ipost:
		i0Name = "";
		break;
	}

	switch ((int)config_->fluorescenceDetectorChoice()){

	case VESPERS2DScanConfiguration::None:
		break;

	case VESPERS2DScanConfiguration::SingleElement:
	case VESPERS2DScanConfiguration::FourElement:{

		AMDataSource *rawDataSource = 0;
		AM2DNormalizationAB *normROI = 0;
		int roiCount = detector->roiInfoList()->count();

		for (int i = 0; i < roiCount; i++){

			rawDataSource = scan_->rawDataSources()->at(i+2);
			normROI = new AM2DNormalizationAB("norm_"+rawDataSource->name());
			normROI->setDescription("Normalized "+rawDataSource->description());
			normROI->setDataName(rawDataSource->name());
			normROI->setNormalizationName(i0Name);
			normROI->setInputDataSources(QList<AMDataSource *>() << rawDataSource << i0List);
			scan_->addAnalyzedDataSource(normROI, true, false);
		}

		break;
	}

	case VESPERS2DScanConfiguration::SingleElement | VESPERS2DScanConfiguration::FourElement:{

		AMDataSource *roi1 = 0;
		AMDataSource *roi4 = 0;
		AM2DAdditionAB *sumAB = 0;

		QList<QPair<int, int> > sameRois = findRoiPairs();
		QStringList roiNames;
		int singleElRoiCount = VESPERSBeamline::vespers()->vortexXRF1E()->roiInfoList()->count();

		for (int i = 0, count = sameRois.size(); i < count; i++){

			roi1 = scan_->rawDataSources()->at(sameRois.at(i).first+2);
			roi4 = scan_->rawDataSources()->at(sameRois.at(i).second+2+singleElRoiCount);
			QString name = roi1->name().left(roi1->name().size()-2);
			roiNames << name;
			sumAB = new AM2DAdditionAB("sum_" % name);
			sumAB->setDescription("Summed " % roi1->description());
			sumAB->setInputDataSources(QList<AMDataSource *>() << roi1 << roi4);
			scan_->addAnalyzedDataSource(sumAB, false, true);
		}

		AM2DNormalizationAB *normROI = 0;
		AMDataSource *source = 0;

		for (int i = 0, count = scan_->analyzedDataSourceCount(); i < count; i++){

			source = scan_->analyzedDataSources()->at(i);
			normROI = new AM2DNormalizationAB("norm_" % roiNames.at(i));
			normROI->setDescription("Normalized " % source->description());
			normROI->setDataName(source->name());
			normROI->setNormalizationName(i0Name);
			normROI->setInputDataSources(QList<AMDataSource *>() << source << i0List);
			scan_->addAnalyzedDataSource(normROI, true, false);
		}

		AM3DAdditionAB *spectraSum = new AM3DAdditionAB("sumSpectra");
		spectraSum->setDescription("Sum of Single and Four Element detectors");
		spectraSum->setInputDataSources(QList<AMDataSource *>() << scan_->dataSourceAt(scan_->indexOfDataSource("spectra")) << scan_->dataSourceAt(scan_->indexOfDataSource("corrSum")));
		scan_->addAnalyzedDataSource(spectraSum, false, true);

		break;
	}
	}
}

QList<QPair<int, int> > VESPERS2DDacqScanController::findRoiPairs() const
{
	AMROIInfoList *el1 = VESPERSBeamline::vespers()->vortexXRF1E()->roiInfoList();
	AMROIInfoList *el4 = VESPERSBeamline::vespers()->vortexXRF4E()->roiInfoList();
	QList<QPair<int, int> > list;

	// Do it the easy way first.  Only possible when the sizes are the same.
	if (el1->count() == el4->count()){

		bool allLinedUp = true;

		for (int i = 0, count = el1->count(); i < count; i++)
			if (el1->at(i).name() != el4->at(i).name())
				allLinedUp = false;

		// If true, this is really straight forward.
		if (allLinedUp){

			for (int i = 0, count = el1->count(); i < count; i++)
				list << qMakePair(i, i);
		}

		// Otherwise, we have to check each individually.  Not all may match and only matches will be added to the list.
		else {

			for (int i = 0, count = el1->count(); i < count; i++)
				for (int j = 0; j < count; j++)
					if (el1->at(i).name() == el4->at(j).name())
						list << qMakePair(i, j);
		}
	}

	// This is the same the above double for-loop but with different boundaries.
	else {

		for (int i = 0, count1 = el1->count(); i < count1; i++)
			for (int j = 0, count4 = el4->count(); j < count4; j++)
				if (el1->at(i).name() == el4->at(j).name())
					list << qMakePair(i, j);
	}

	return list;
}

void VESPERS2DDacqScanController::addExtraDatasources()
{
	// Common to both types is the ring current.
	scan_->rawData()->addMeasurement(AMMeasurementInfo("RingCurrent", "Ring Current", "mA"));
	scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);

	switch ((int)config_->fluorescenceDetectorChoice()){

	case VESPERS2DScanConfiguration::None:
		break;

	case VESPERS2DScanConfiguration::SingleElement:{

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

		break;
	}

	case VESPERS2DScanConfiguration::FourElement:{

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

		break;
	}

	case VESPERS2DScanConfiguration::SingleElement | VESPERS2DScanConfiguration::FourElement:{

		// Adding measurments for the single element.
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
		// Adding measurments for the four element.
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

		break;
	}
	}

	// Adding in the extra ion chambers but not Ipost.
	AMMeasurementInfo temp("", "");
	AMDetectorSet *ionChambers = VESPERSBeamline::vespers()->ionChambers();

	for (int i = 0; i < ionChambers->count(); i++){

		if (ionChambers->detectorAt(i)->detectorName() != "Ipost"){

			temp = AMMeasurementInfo(*(ionChambers->detectorAt(i)->toInfo()));
			temp.name = ionChambers->detectorAt(i)->detectorName();
			scan_->rawData()->addMeasurement(temp);
			scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount() - 1), false, false);
		}
	}

	// If using the CCD for XRD simultaneously.
	if (config_->usingCCD()){

		scan_->rawData()->addMeasurement(AMMeasurementInfo("CCDFileNumber", "CCD file number"));
		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);
	}

	// Add the spectra.
	switch ((int)config_->fluorescenceDetectorChoice()){

	case VESPERS2DScanConfiguration::None:
		break;

	case VESPERS2DScanConfiguration::SingleElement:{

		temp = AMMeasurementInfo(VESPERSBeamline::vespers()->vortexXRF1E()->toXRFInfo());
		temp.name = "spectra";
		scan_->rawData()->addMeasurement(temp);
		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);

		break;
	}

	case VESPERS2DScanConfiguration::FourElement:{

		temp = AMMeasurementInfo(VESPERSBeamline::vespers()->vortexXRF4E()->toXRFInfo());
		temp.name = "corrSum";
		scan_->rawData()->addMeasurement(temp);
		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);

		for (int i = 0; i < VESPERSBeamline::vespers()->vortexXRF4E()->elements(); i++){

			temp.name = QString("raw%1").arg(i+1);
			scan_->rawData()->addMeasurement(temp);
			scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount() - 1), false, true);
		}

		break;
	}

	case VESPERS2DScanConfiguration::SingleElement | VESPERS2DScanConfiguration::FourElement:{

		temp = AMMeasurementInfo(VESPERSBeamline::vespers()->vortexXRF1E()->toXRFInfo());
		temp.name = "spectra";
		scan_->rawData()->addMeasurement(temp);
		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);

		temp = AMMeasurementInfo(VESPERSBeamline::vespers()->vortexXRF4E()->toXRFInfo());
		temp.name = "corrSum";
		scan_->rawData()->addMeasurement(temp);
		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);

		for (int i = 0; i < VESPERSBeamline::vespers()->vortexXRF4E()->elements(); i++){

			temp.name = QString("raw%1").arg(i+1);
			scan_->rawData()->addMeasurement(temp);
			scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount() - 1), false, true);
		}

		break;
	}
	}
}

bool VESPERS2DDacqScanController::initializeImplementation()
{
	// To initialize the 2D scan, there are two stages.
	/*
		First: Enable/Disable all the pertinent detectors.  The scalar is ALWAYS enabled.
		Second: Set the mode to single shot,set the time on the synchronized dwell time.
	 */
	AMBeamlineParallelActionsList *initializationActionsList = new AMBeamlineParallelActionsList;

	if (!initializationActions_)
		onInitializationActionFinished();

	initializationActions_ = new AMBeamlineListAction(initializationActionsList);

	// First stage.
	initializationActionsList->appendStage(new QList<AMBeamlineActionItem*>());
	// Scalar
	initializationActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementAt(0)->createEnableAction(true));
	// Single element vortex
	if ((config_->fluorescenceDetectorChoice() == VESPERS2DScanConfiguration::SingleElement) || (config_->fluorescenceDetectorChoice() == (VESPERS2DScanConfiguration::SingleElement | VESPERS2DScanConfiguration::FourElement)))
		initializationActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementAt(1)->createEnableAction(true));
	else
		initializationActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementAt(1)->createEnableAction(false));
	// CCD
	if (config_->usingCCD())
		initializationActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementAt(2)->createEnableAction(true));
	else
		initializationActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementAt(2)->createEnableAction(false));
	// Picoammeters
	initializationActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementAt(3)->createEnableAction(false));
	// Four element vortex
	if ((config_->fluorescenceDetectorChoice() == VESPERS2DScanConfiguration::FourElement) || (config_->fluorescenceDetectorChoice() == (VESPERS2DScanConfiguration::SingleElement | VESPERS2DScanConfiguration::FourElement)))
		initializationActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementAt(4)->createEnableAction(true));
	else
		initializationActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementAt(4)->createEnableAction(false));

	// Second stage.
	initializationActionsList->appendStage(new QList<AMBeamlineActionItem*>());
	initializationActionsList->appendAction(1, VESPERSBeamline::vespers()->scaler()->createScansPerBufferAction(1));
	initializationActionsList->appendAction(1, VESPERSBeamline::vespers()->scaler()->createTotalScansAction(1));
	initializationActionsList->appendAction(1, VESPERSBeamline::vespers()->synchronizedDwellTime()->createModeAction(CLSSynchronizedDwellTime::SingleShot));
	initializationActionsList->appendAction(1, VESPERSBeamline::vespers()->synchronizedDwellTime()->createMasterTimeAction(config_->timeStep()));

	// Integrity check.  Make sure no actions are null.
	for (int i = 0; i < initializationActionsList->stageCount(); i++){

		for (int j = 0; j < initializationActionsList->stage(i)->size(); j++){

			if (initializationActionsList->action(i, j) == 0){

				onInitializationActionsFailed(0);
				return false;
			}
		}
	}

	connect(initializationActions_, SIGNAL(succeeded()), this, SLOT(onInitializationActionsSucceeded()));
	connect(initializationActions_, SIGNAL(failed(int)), this, SLOT(onInitializationActionsFailed(int)));
	connect(initializationActions_, SIGNAL(progress(double,double)), this, SLOT(onInitializationActionsProgress(double,double)));
	initializationActions_->start();

	return true;
}

bool VESPERS2DDacqScanController::startImplementation()
{
	bool configSuccess = false;

	if (config_->fluorescenceDetectorChoice() == VESPERS2DScanConfiguration::SingleElement)
		configSuccess = setupSingleElementMap();

	else if (config_->fluorescenceDetectorChoice() == VESPERS2DScanConfiguration::FourElement)
		configSuccess = setupFourElementMap();

	else if (config_->fluorescenceDetectorChoice() == (VESPERS2DScanConfiguration::SingleElement | VESPERS2DScanConfiguration::FourElement))
		configSuccess = setupSingleAndFourElementMap();

	if (!configSuccess){

		AMErrorMon::alert(this, VESPERS2DDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE, "Could not recognize the configuration file.");
		return false;
	}

	return AM2DDacqScanController::startImplementation();
}

void VESPERS2DDacqScanController::cleanup()
{
	// To cleanup the 2D scan, there are two stages.
	/*
		First: Only have the scalar running in the syncrhonized dwell time.
		Second: Set the dwell time to 1 second.  Disables the variable integration time.  Make sure the CCD is no longer armed if it was used.
		Third: Set the scan mode to continuous.  This starts the synchronized dwell time.
	 */
	AMBeamlineParallelActionsList *cleanupActionsList = new AMBeamlineParallelActionsList;

	if (!cleanupActions_)
		onCleanupActionFinished();

	cleanupActions_ = new AMBeamlineListAction(cleanupActionsList);

	// First stage.
	cleanupActionsList->appendStage(new QList<AMBeamlineActionItem*>());
	// Scalar
	cleanupActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementAt(0)->createEnableAction(true));
	// Single element vortex
	cleanupActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementAt(1)->createEnableAction(false));
	// CCD
	cleanupActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementAt(2)->createEnableAction(false));
	// Picoammeters
	cleanupActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementAt(3)->createEnableAction(false));
	// Four element vortex
	cleanupActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementAt(4)->createEnableAction(false));

	// Second stage.
	cleanupActionsList->appendStage(new QList<AMBeamlineActionItem*>());
	// Synchronized dwell time.
	cleanupActionsList->appendAction(1, VESPERSBeamline::vespers()->synchronizedDwellTime()->createMasterTimeAction(1.0));
	// Variable integration time.
	cleanupActionsList->appendAction(1, VESPERSBeamline::vespers()->variableIntegrationTime()->createModeAction(CLSVariableIntegrationTime::Disabled));
	// CCD.
	if (config_->usingCCD())
		cleanupActionsList->appendAction(1, VESPERSBeamline::vespers()->roperCCD()->createStopAction());

	// Third stage.
	cleanupActionsList->appendStage(new QList<AMBeamlineActionItem *>());
	// Start the synchronized dwell time.
	cleanupActionsList->appendAction(2, VESPERSBeamline::vespers()->synchronizedDwellTime()->createModeAction(CLSSynchronizedDwellTime::Continuous));

	connect(cleanupActions_, SIGNAL(succeeded()), this, SLOT(onCleanupFinished()));
	connect(cleanupActions_, SIGNAL(failed(int)), this, SLOT(onCleanupFinished()));
	cleanupActions_->start();
}

void VESPERS2DDacqScanController::onCleanupFinished()
{
	AM2DDacqScanController::onDacqStop();
}

void VESPERS2DDacqScanController::onScanTimerUpdate()
{
	if (elapsedTime_.isActive()){

		if (secondsElapsed_ >= secondsTotal_)
			secondsElapsed_ = secondsTotal_;
		else
			secondsElapsed_ += 1.0;

		emit progress(secondsElapsed_, secondsTotal_);
	}
}

void VESPERS2DDacqScanController::onInitializationActionsSucceeded()
{
	setInitialized();
}

void VESPERS2DDacqScanController::onInitializationActionsFailed(int explanation)
{
	Q_UNUSED(explanation)

	AMErrorMon::alert(this, VESPERS2DDACQSCANCONTROLLER_CANT_INTIALIZE, "2D scan failed to initialize.");
	onInitializationActionFinished();
	setFailed();
}

void VESPERS2DDacqScanController::onInitializationActionsProgress(double elapsed, double total)
{
	Q_UNUSED(elapsed)
	Q_UNUSED(total)
}

QString VESPERS2DDacqScanController::getHomeDirectory()
{
	// Find out which path we are using for acquaman (depends on whether you are on Mac or Linux or beamline OPI).
	QString homeDir = QDir::homePath();
	if(QDir(homeDir+"/dev").exists())
		homeDir.append("/dev");
	else if(QDir(homeDir+"/beamline/programming").exists())
		homeDir.append("/beamline/programming");

	return homeDir;
}

void VESPERS2DDacqScanController::onInitializationActionFinished()
{
	if (initializationActions_ == 0)
		return;

	// Disconnect all signals and return all memory.
	initializationActions_->disconnect();
	AMBeamlineParallelActionsList *actionList = initializationActions_->list()	;

	for (int i = 0; i < actionList->stageCount(); i++){

		while (actionList->stage(i)->size())
			actionList->stage(i)->takeAt(0)->deleteLater();
	}

	initializationActions_->deleteLater();
	initializationActions_ = 0;
}

void VESPERS2DDacqScanController::onCleanupActionFinished()
{
	if (cleanupActions_ == 0)
		return;

	// Disconnect all signals and return all memory.
	cleanupActions_->disconnect();
	AMBeamlineParallelActionsList *actionList = cleanupActions_->list()	;

	for (int i = 0; i < actionList->stageCount(); i++){

		while (actionList->stage(i)->size())
			actionList->stage(i)->takeAt(0)->deleteLater();
	}

	cleanupActions_->deleteLater();
	cleanupActions_ = 0;
}

bool VESPERS2DDacqScanController::setupSingleElementMap()
{
	VESPERSConfigurationFileBuilder builder;
	builder.setDimensions(2);
	builder.setSingleElement(true);
	builder.setRoperCCD(config_->usingCCD());
	builder.setPvNameAxis1(xAxisPVName_);	// This is fine because we have already checked what sample stage we're using in the constructor.
	builder.setPvNameAxis2(yAxisPVName_);	// Ditto.
	builder.buildConfigurationFile();

	bool loadSuccess = 	setConfigFile(getHomeDirectory().append("/acquaman/devConfigurationFiles/VESPERS/template.cfg"));

	if(!loadSuccess){
		AMErrorMon::alert(this,
				VESPERS2DDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
				"Error, VESPERS 2D DACQ Scan Controller failed to start (the config file failed to load). Please report this bug to the Acquaman developers.");
		return false;
	}

	usingSpectraDotDatFile_ = true;

	// Remove all the "goober" records that were added to create enough space for the Dacq.  (Hack the Dacq solution).
	while (advAcq_->deleteRecord(2)){}

	advAcq_->appendRecord(xAxisPVName()+":fbk", true, false, 0);
	advAcq_->appendRecord(yAxisPVName()+":fbk", true, false, 0);

	XRFDetector *detector = VESPERSBeamline::vespers()->vortexXRF1E();
	int roiCount = detector->roiInfoList()->count();

	for (int i = 0; i < roiCount; i++)
		advAcq_->appendRecord("IOC1607-004:mca1.R"+QString::number(i), true, false, 1);

	advAcq_->appendRecord("PCT1402-01:mA:fbk", true, false, 0);
	advAcq_->appendRecord("IOC1607-004:mca1.DTIM", true, false, 0);
	advAcq_->appendRecord("IOC1607-004:mca1.ERTM", true, false, 0);
	advAcq_->appendRecord("IOC1607-004:mca1.ELTM", true, false, 0);
	advAcq_->appendRecord("IOC1607-004:dxp1.FAST_PEAKS", true, false, 0);
	advAcq_->appendRecord("IOC1607-004:dxp1.SLOW_PEAKS", true, false, 0);

	AMDetectorSet *ionChambers = VESPERSBeamline::vespers()->ionChambers();

	for (int i = 0; i < ionChambers->count(); i++)
		if (ionChambers->detectorAt(i)->detectorName() != "Ipost")
			advAcq_->appendRecord(VESPERSBeamline::vespers()->pvName(ionChambers->detectorAt(i)->detectorName()), true, false, detectorReadMethodToDacqReadMethod(ionChambers->detectorAt(i)->readMethod()));

	if (config_->usingCCD())
		advAcq_->appendRecord("IOC1607-003:det1:FileNumber", true, false, 0);

	advAcq_->appendRecord("IOC1607-004:mca1", true, true, 1);

	return true;
}

bool VESPERS2DDacqScanController::setupFourElementMap()
{
	VESPERSConfigurationFileBuilder builder;
	builder.setDimensions(2);
	builder.setFourElement(true);
	builder.setRoperCCD(config_->usingCCD());
	builder.setPvNameAxis1(xAxisPVName_);	// This is fine because we have already checked what sample stage we're using in the constructor.
	builder.setPvNameAxis2(yAxisPVName_);	// Ditto.
	builder.buildConfigurationFile();

	bool loadSuccess = setConfigFile(getHomeDirectory().append("/acquaman/devConfigurationFiles/VESPERS/template.cfg"));

	if(!loadSuccess){
		AMErrorMon::alert(this,
				VESPERS2DDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
				"Error, VESPERS 2D DACQ Scan Controller failed to start (the config file failed to load). Please report this bug to the Acquaman developers.");
		return false;
	}

	usingSpectraDotDatFile_ = true;

	// Remove all the "goober" records that were added to create enough space for the Dacq.  (Hack the Dacq solution).
	while (advAcq_->deleteRecord(2)){}

	advAcq_->appendRecord(xAxisPVName()+":fbk", true, false, 0);
	advAcq_->appendRecord(yAxisPVName()+":fbk", true, false, 0);

	XRFDetector *detector = VESPERSBeamline::vespers()->vortexXRF4E();
	int roiCount = detector->roiInfoList()->count();

	for (int i = 0; i < roiCount; i++)
		advAcq_->appendRecord("dxp1607-B21-04:mcaCorrected.R"+QString::number(i), true, false, 1);

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

	AMDetectorSet *ionChambers = VESPERSBeamline::vespers()->ionChambers();

	for (int i = 0; i < ionChambers->count(); i++)
		if (ionChambers->detectorAt(i)->detectorName() != "Ipost")
			advAcq_->appendRecord(VESPERSBeamline::vespers()->pvName(ionChambers->detectorAt(i)->detectorName()), true, false, detectorReadMethodToDacqReadMethod(ionChambers->detectorAt(i)->readMethod()));

	if (config_->usingCCD())
		advAcq_->appendRecord("IOC1607-003:det1:FileNumber", true, false, 0);

	advAcq_->appendRecord("dxp1607-B21-04:mcaCorrected", true, true, 0);
	advAcq_->appendRecord("dxp1607-B21-04:mca1", true, true, 0);
	advAcq_->appendRecord("dxp1607-B21-04:mca2", true, true, 0);
	advAcq_->appendRecord("dxp1607-B21-04:mca3", true, true, 0);
	advAcq_->appendRecord("dxp1607-B21-04:mca4", true, true, 0);

	return true;
}

bool VESPERS2DDacqScanController::setupSingleAndFourElementMap()
{
	VESPERSConfigurationFileBuilder builder;
	builder.setDimensions(2);
	builder.setSingleElement(true);
	builder.setFourElement(true);
	builder.setRoperCCD(config_->usingCCD());
	builder.setPvNameAxis1(xAxisPVName_);	// This is fine because we have already checked what sample stage we're using in the constructor.
	builder.setPvNameAxis2(yAxisPVName_);	// Ditto.
	builder.buildConfigurationFile();

	bool loadSuccess = setConfigFile(getHomeDirectory().append("/acquaman/devConfigurationFiles/VESPERS/template.cfg"));

	if(!loadSuccess){
		AMErrorMon::alert(this,
				VESPERS2DDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
				"Error, VESPERS 2D DACQ Scan Controller failed to start (the config file failed to load). Please report this bug to the Acquaman developers.");
		return false;
	}

	usingSpectraDotDatFile_ = true;

	// Remove all the "goober" records that were added to create enough space for the Dacq.  (Hack the Dacq solution).
	while (advAcq_->deleteRecord(2)){}

	advAcq_->appendRecord(xAxisPVName()+":fbk", true, false, 0);
	advAcq_->appendRecord(yAxisPVName()+":fbk", true, false, 0);

	// Add the single element ROIs.
	XRFDetector *detector = VESPERSBeamline::vespers()->vortexXRF1E();
	int roiCount = detector->roiInfoList()->count();

	for (int i = 0; i < roiCount; i++)
		advAcq_->appendRecord("IOC1607-004:mca1.R"+QString::number(i), true, false, 1);

	// Add the four element ROIs.
	detector = VESPERSBeamline::vespers()->vortexXRF4E();
	roiCount = detector->roiInfoList()->count();

	for (int i = 0; i < roiCount; i++)
		advAcq_->appendRecord("dxp1607-B21-04:mcaCorrected.R"+QString::number(i), true, false, 1);

	// Single element detector values.
	advAcq_->appendRecord("PCT1402-01:mA:fbk", true, false, 0);
	advAcq_->appendRecord("IOC1607-004:mca1.DTIM", true, false, 0);
	advAcq_->appendRecord("IOC1607-004:mca1.ERTM", true, false, 0);
	advAcq_->appendRecord("IOC1607-004:mca1.ELTM", true, false, 0);
	advAcq_->appendRecord("IOC1607-004:dxp1.FAST_PEAKS", true, false, 0);
	advAcq_->appendRecord("IOC1607-004:dxp1.SLOW_PEAKS", true, false, 0);

	// Four element detector values.
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

	// Ion chambers.
	AMDetectorSet *ionChambers = VESPERSBeamline::vespers()->ionChambers();

	for (int i = 0; i < ionChambers->count(); i++)
		if (ionChambers->detectorAt(i)->detectorName() != "Ipost")
			advAcq_->appendRecord(VESPERSBeamline::vespers()->pvName(ionChambers->detectorAt(i)->detectorName()), true, false, detectorReadMethodToDacqReadMethod(ionChambers->detectorAt(i)->readMethod()));

	// Using the CCD?
	if (config_->usingCCD())
		advAcq_->appendRecord("IOC1607-003:det1:FileNumber", true, false, 0);

	// The spectra for each detector.
	advAcq_->appendRecord("IOC1607-004:mca1", true, true, 1);

	advAcq_->appendRecord("dxp1607-B21-04:mcaCorrected", true, true, 0);
	advAcq_->appendRecord("dxp1607-B21-04:mca1", true, true, 0);
	advAcq_->appendRecord("dxp1607-B21-04:mca2", true, true, 0);
	advAcq_->appendRecord("dxp1607-B21-04:mca3", true, true, 0);
	advAcq_->appendRecord("dxp1607-B21-04:mca4", true, true, 0);

	return true;
}
