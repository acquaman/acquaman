#include "VESPERSTimedLineScanActionController.h"

#include "acquaman/VESPERS/VESPERSTimedLineScanActionControllerAssembler.h"
#include "beamline/AMBeamline.h"
#include "dataman/datastore/AMCDFDataStore.h"
#include "dataman/datastore/AMInMemoryDataStore.h"
#include "analysis/AM3DAdditionAB.h"
#include "analysis/AM2DNormalizationAB.h"

VESPERSTimedLineScanActionController::VESPERSTimedLineScanActionController(VESPERSTimedLineScanConfiguration *configuration, QObject *parent)
	: VESPERSSpatialLineScanActionController(configuration, parent)
{
	timedConfiguration_ = configuration;

	int yPoints = configuration_->scanAxisAt(1)->numberOfPoints();
	scan_->rawData()->addScanAxis(AMAxisInfo("Time", yPoints, "Time", "s"));

	VESPERS::FluorescenceDetectors xrfDetector = configuration_->fluorescenceDetector();
	AMDetector *detector = 0;

	if (xrfDetector.testFlag(VESPERS::SingleElement)){

		detector = AMBeamline::bl()->exposedDetectorByName("SingleElementVortex");
		detector->setHiddenFromUsers(true);
		detector->setIsVisible(false);
	}

	if (xrfDetector.testFlag(VESPERS::FourElement)){

		detector = AMBeamline::bl()->exposedDetectorByName("FourElementVortex");
		detector->setHiddenFromUsers(true);
		detector->setIsVisible(false);
	}
}

VESPERSTimedLineScanActionController::~VESPERSTimedLineScanActionController()
{

}

void VESPERSTimedLineScanActionController::createScanAssembler()
{
	scanAssembler_ = new VESPERSTimedLineScanActionControllerAssembler(timedConfiguration_->iterations(), timedConfiguration_->timePerAcquisition(), this);
}

void VESPERSTimedLineScanActionController::prefillScanPoints()
{
	AMnDIndex insertIndex;
	QVector<double> starts;
	QVector<double> steps;
	QVector<double> ends;
	QVector<int> axisSizes;

	// This assumes only one region per axis for now.
	foreach (AMScanAxis *axis, stepConfiguration_->scanAxes()){

		AMScanAxisRegion *region = axis->regionAt(0);
		starts << double(region->regionStart());
		steps << double(region->regionStep());
		ends << double(region->regionEnd());
		axisSizes << (int(round((double(region->regionEnd())-double(region->regionStart()))/double(region->regionStep()))) + 1);
	}

	scan_->rawData()->beginInsertRows(axisSizes.at(0), -1);

	for (int j = 0; j < axisSizes.at(1); j++){

		for (int i = 0; i < axisSizes.at(0); i++){

			insertIndex = AMnDIndex(i, j);
			scan_->rawData()->setAxisValue(0, insertIndex.i(), starts.at(0) + i*steps.at(0));
			scan_->rawData()->setAxisValue(1, insertIndex.j(), starts.at(1) + j*steps.at(1));
		}
	}

	scan_->rawData()->endInsertRows();
}

void VESPERSTimedLineScanActionController::buildScanController()
{
	// Build the scan assembler.
	createScanAssembler();
	// Create the axis order map for higher dimensional scans.
	createAxisOrderMap();

	currentAxisValueIndex_ = AMnDIndex(scan_->rawData()->scanAxesCount(), AMnDIndex::DoInit, 0);

	// Setup all the axes.
	for (int i = 0, axisCount = scan_->rawData()->scanAxesCount(); i < axisCount; i++){

		currentAxisValues_ << 0;
		stepConfiguration_->scanAxisAt(i)->setName(scan_->rawData()->scanAxisAt(i).name);

		for (int j = 0, regionCount = stepConfiguration_->scanAxisAt(i)->regionCount(); j < regionCount; j++)
			stepConfiguration_->scanAxisAt(i)->regionAt(j)->setName(QString("%1 %2 %3").arg(scan_->rawData()->scanAxisAt(i).name).arg("region").arg(j+1));
	}

	scanAssembler_->insertAxis(0, AMBeamline::bl()->exposedControlByInfo(stepConfiguration_->axisControlInfos().at(0)), stepConfiguration_->scanAxisAt(0));

	// Add all the detectors.
	for (int i = 0, size = stepConfiguration_->detectorConfigurations().count(); i < size; i++){

		AMDetector *oneDetector = AMBeamline::bl()->exposedDetectorByInfo(stepConfiguration_->detectorConfigurations().at(i));

		if (oneDetector && !scanAssembler_->addDetector(oneDetector)){

			AMErrorMon::alert(this, AMSTEPSCANACTIONCONTROLLER_COULD_NOT_ADD_DETECTOR, QString("Could not add the following detector to the assembler: %1").arg(oneDetector != 0 ? oneDetector->name() : "Not found"));
			return;
		}
	}

	// Handle some general scan stuff, including setting the default file path.
	scan_->setRunId(AMUser::user()->currentRunId());

	bool has1DDetectors = false;

	for (int i = 0, size = stepConfiguration_->detectorConfigurations().count(); i < size && !has1DDetectors; i++){

		AMDetector *detector = AMBeamline::bl()->exposedDetectorByInfo(stepConfiguration_->detectorConfigurations().at(i));

		if (detector && detector->rank() == 1)
			has1DDetectors = true;
	}

	QFileInfo fullPath(AMUserSettings::defaultRelativePathForScan(QDateTime::currentDateTime()));	// ex: 2010/09/Mon_03_12_24_48_0000   (Relative, and with no extension)

	// If using just the basic ascii files.
	if (scan_->fileFormat() == "amRegionAscii2013"){

		scan_->setFilePath(fullPath.filePath() % ".dat");	// relative path and extension (is what the database wants)

		if(has1DDetectors)
			scan_->setAdditionalFilePaths( QStringList() << fullPath.filePath() % "_spectra.dat" );
	}

	// If you want to use the CDF data file format.
	else if (scan_->fileFormat() == "amCDFv1"){

		// Get all the scan axes so they can be added to the new data store.
		QList<AMAxisInfo> scanAxes;

		for (int i = 0, size = scan_->rawData()->scanAxesCount(); i < size; i++)
			scanAxes << scan_->rawData()->scanAxisAt(i);

		scan_->setFilePath(fullPath.filePath() % ".cdf");
		scan_->replaceRawDataStore(new AMCDFDataStore(AMUserSettings::userDataFolder % scan_->filePath(), false));

		// Add all the old scan axes.
		foreach (AMAxisInfo axis, scanAxes)
			scan_->rawData()->addScanAxis(axis);

		connect(this, SIGNAL(started()), this, SLOT(flushCDFDataStoreToDisk()));
		connect(this, SIGNAL(cancelled()), this, SLOT(flushCDFDataStoreToDisk()));
		connect(this, SIGNAL(failed()), this, SLOT(flushCDFDataStoreToDisk()));
		connect(this, SIGNAL(finished()), this, SLOT(flushCDFDataStoreToDisk()));
	}

	qRegisterMetaType<AMScanActionControllerBasicFileWriter::FileWriterError>("FileWriterError");
	fileWriter_ = new AMScanActionControllerBasicFileWriter(AMUserSettings::userDataFolder % fullPath.filePath(), has1DDetectors);
	connect(fileWriter_, SIGNAL(fileWriterIsBusy(bool)), this, SLOT(onFileWriterIsBusy(bool)));
	connect(fileWriter_, SIGNAL(fileWriterError(AMScanActionControllerBasicFileWriter::FileWriterError)), this, SLOT(onFileWriterError(AMScanActionControllerBasicFileWriter::FileWriterError)));
	connect(this, SIGNAL(requestWriteToFile(int,QString)), fileWriter_, SLOT(writeToFile(int,QString)));
	connect(this, SIGNAL(finishWritingToFile()), fileWriter_, SLOT(finishWriting()));

	fileWriterThread_ = new QThread();
	fileWriter_->moveToThread(fileWriterThread_);
	fileWriterThread_->start();

	// Get all the detectors added to the scan.
	for (int i = 0, size = stepConfiguration_->detectorConfigurations().count(); i < size; i++){

		AMDetector *oneDetector = AMBeamline::bl()->exposedDetectorByInfo(stepConfiguration_->detectorConfigurations().at(i));

		if(oneDetector && scan_->rawData()->addMeasurement(AMMeasurementInfo(*oneDetector)))
			scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), oneDetector->isVisible(), oneDetector->hiddenFromUsers());
	}

	// There is always 2 scan axes.
	prefillScanPoints();

	connect(scanAssembler_, SIGNAL(actionTreeGenerated(AMAction3*)), this, SLOT(onScanningActionsGenerated(AMAction3*)));

	if (scanAssembler_->generateActionTree())
		buildScanControllerImplementation();

	else
		setFailed();
}

void VESPERSTimedLineScanActionController::buildScanControllerImplementation()
{
	VESPERS::FluorescenceDetectors xrfDetector = configuration_->fluorescenceDetector();
	AMXRFDetector *detector = 0;

	if (xrfDetector.testFlag(VESPERS::SingleElement))
		detector = qobject_cast<AMXRFDetector *>(AMBeamline::bl()->exposedDetectorByName("SingleElementVortex"));

	else if (xrfDetector.testFlag(VESPERS::FourElement))
		detector = qobject_cast<AMXRFDetector *>(AMBeamline::bl()->exposedDetectorByName("FourElementVortex"));

	if (detector){

		detector->removeAllRegionsOfInterest();

		QList<AMDataSource *> i0Sources = QList<AMDataSource *>()
				<< scan_->dataSourceAt(scan_->indexOfDataSource("SplitIonChamber"))
				   << scan_->dataSourceAt(scan_->indexOfDataSource("PreKBIonChamber"))
					  << scan_->dataSourceAt(scan_->indexOfDataSource("MiniIonChamber"));

		AMDataSource *spectraSource = 0;

		if (xrfDetector.testFlag(VESPERS::SingleElement) && xrfDetector.testFlag(VESPERS::FourElement)){

			AM3DAdditionAB *sumSpectra = new AM3DAdditionAB("SingleAndFourSpectra");
			sumSpectra->setInputDataSources(QList<AMDataSource *>() << scan_->dataSourceAt(scan_->indexOfDataSource("SingleElementVortex")) << scan_->dataSourceAt(scan_->indexOfDataSource("FourElementVortex")));
			scan_->addAnalyzedDataSource(sumSpectra, false, true);
			spectraSource = sumSpectra;
		}

		else
			spectraSource = scan_->dataSourceAt(scan_->indexOfDataSource(detector->name()));

		foreach (AMRegionOfInterest *region, configuration_->regionsOfInterest()){

			AMRegionOfInterestAB *regionAB = (AMRegionOfInterestAB *)region->valueSource();
			AMRegionOfInterestAB *newRegion = new AMRegionOfInterestAB(regionAB->name().remove(' '));
			newRegion->setBinningRange(regionAB->binningRange());
			newRegion->setInputDataSources(QList<AMDataSource *>() << spectraSource);
			scan_->addAnalyzedDataSource(newRegion, false, true);
			detector->addRegionOfInterest(region);

			AM2DNormalizationAB *normalizedRegion = new AM2DNormalizationAB(QString("norm_%1").arg(newRegion->name()));
			normalizedRegion->setInputDataSources(QList<AMDataSource *>() << newRegion << i0Sources);
			normalizedRegion->setDataName(newRegion->name());
			normalizedRegion->setNormalizationName(i0Sources.at(int(configuration_->incomingChoice()))->name());
			scan_->addAnalyzedDataSource(normalizedRegion, true, false);
		}
	}
}
