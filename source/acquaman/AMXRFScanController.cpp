#include "AMXRFScanController.h"

#include "analysis/AMnDDeadTimeAB.h"
#include "analysis/AMAdditionAB.h"
#include "beamline/AMBeamline.h"
#include "dataman/AMScan.h"
#include "dataman/AMUser.h"
#include "dataman/datastore/AMCDFDataStore.h"

#include <QStringBuilder>

AMXRFScanController::AMXRFScanController(AMXRFScanConfiguration *configuration, QObject *parent)
	: AMScanController(configuration, parent)
{
	detector_ = qobject_cast<AMXRFDetector *>(AMBeamline::bl()->exposedDetectorByInfo(configuration->detectorConfigurations().at(0)));

	scan_ = new AMScan;
	scan_->setScanConfiguration(configuration);
	scan_->setName(QString("XRF Scan - %1").arg(detector_->name()));
	scan_->setFileFormat("amCDFv1");
	scan_->setFilePath(AMUserSettings::defaultRelativePathForScan(QDateTime::currentDateTime()) % ".cdf");
	scan_->replaceRawDataStore(new AMCDFDataStore(AMUserSettings::userDataFolder % scan_->filePath(), false));
	scan_->setRunId(AMUser::user()->currentRunId());
	scan_->setNotes(QString("%1\n\n%2\n\n").arg(detector_->details()).arg(AMBeamline::bl()->details()));

	int elements = detector_->elements();

	QList<AMDataSource *> rawSpectra;
	QList<AMDataSource *> inputCountRates;
	QList<AMDataSource *> outputCountRates;
	QList<AMDataSource *> correctedSpectra;

	for (int i = 0; i < elements; i++){

		scan_->rawData()->addMeasurement(AMMeasurementInfo(QString("Raw %1").arg(i+1), QString("Element %1").arg(i+1), "eV", detector_->axes()));

		AMRawDataSource *newSource = new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1);
		scan_->addRawDataSource(newSource, true, false);
		rawSpectra << newSource;
	}

	if (!detector_->inputCountSources().isEmpty() && !detector_->outputCountSources().isEmpty()){

		for (int i = 0; i < elements; i++){

			scan_->rawData()->addMeasurement(AMMeasurementInfo(QString("icr%1").arg(i+1), QString("Input count rate %1").arg(i+1), "%", QList<AMAxisInfo>()));

			AMRawDataSource *newSource = new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1);
			scan_->addRawDataSource(newSource, false, true);
			inputCountRates << newSource;
		}

		for (int i = 0; i < elements; i++){

			scan_->rawData()->addMeasurement(AMMeasurementInfo(QString("ocr%1").arg(i+1), QString("Output count rate %1").arg(i+1), "%", QList<AMAxisInfo>()));

			AMRawDataSource *newSource = new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1);
			scan_->addRawDataSource(newSource, false, true);
			outputCountRates << newSource;
		}

		for (int i = 0; i < elements; i++){

			AMnDDeadTimeAB *newSource = new AMnDDeadTimeAB(QString("Corrected %1").arg(i+1));
			newSource->setInputDataSources(QList<AMDataSource *>() << rawSpectra.at(i) << inputCountRates.at(i) << outputCountRates.at(i));
			scan_->addAnalyzedDataSource(newSource, true, false);
			correctedSpectra << newSource;
		}
	}

	if (elements > 1){

		AMAdditionAB *newSource = new AMAdditionAB("CorrectedSum");
		newSource->setInputDataSources(correctedSpectra.isEmpty() ? rawSpectra : correctedSpectra);
		scan_->addAnalyzedDataSource(newSource, true, false);
	}
}

AMXRFScanController::~AMXRFScanController()
{

}

void AMXRFScanController::onDetectorAcquisitionFinished()
{
	disconnect(detector_, SIGNAL(acquisitionStateChanged(AMDetector::AcqusitionState)), this, SLOT(onStatusChanged()));
	disconnect(detector_, SIGNAL(elapsedTimeChanged(double)), this, SLOT(onProgressUpdate()));

	QVector<double> spectrum = QVector<double>(detector_->size(0));

	if (detector_->inputCountSources().isEmpty()){

		for (int i = 0, elements = detector_->elements(); i < elements; i++){

			detector_->rawSpectrumSources().at(i)->values(AMnDIndex(0), AMnDIndex(detector_->size(0)-1), spectrum.data());
			scan_->rawData()->setValue(AMnDIndex(), i, spectrum.constData());
		}
	}

	else if (!detector_->outputCountSources().isEmpty()){
		for (int i = 0, elements = detector_->elements(); i < elements; i++){

			detector_->rawSpectrumSources().at(i)->values(AMnDIndex(0), AMnDIndex(detector_->size(0)-1), spectrum.data());
			scan_->rawData()->setValue(AMnDIndex(), i, spectrum.constData());
			scan_->rawData()->setValue(AMnDIndex(), i+elements, AMnDIndex(), detector_->inputCountSourceAt(i)->value(AMnDIndex()));
			scan_->rawData()->setValue(AMnDIndex(), i+2*elements, AMnDIndex(), detector_->outputCountSourceAt(i)->value(AMnDIndex()));
		}
	}

	scan()->setScanController(0);
	flushCDFDataStoreToDisk();
	scan()->storeToDb(AMDatabase::database("user"));

	setFinished();
}

void AMXRFScanController::onProgressUpdate()
{
	emit progress(detector_->elapsedTime(), detector_->acquisitionTime());
}

void AMXRFScanController::onStatusChanged()
{
	AMDetector::AcqusitionState state = detector_->acquisitionState();

	if (state == AMDetector::Succeeded || state == AMDetector::Cancelled || state == AMDetector::Failed)
		onDetectorAcquisitionFinished();
}

bool AMXRFScanController::initializeImplementation()
{
	setInitialized();

	return true;
}

bool AMXRFScanController::startImplementation()
{
	flushCDFDataStoreToDisk();
	connect(detector_, SIGNAL(acquisitionStateChanged(AMDetector::AcqusitionState)), this, SLOT(onStatusChanged()));
	connect(detector_, SIGNAL(elapsedTimeChanged(double)), this, SLOT(onProgressUpdate()));
	detector_->acquire();

	setStarted();

	return true;
}

void AMXRFScanController::cancelImplementation()
{
	setCancelled();
}

void AMXRFScanController::stopImplementation(const QString &command)
{
	Q_UNUSED(command)
	setFinished();
}

void AMXRFScanController::flushCDFDataStoreToDisk()
{
	AMCDFDataStore *dataStore = qobject_cast<AMCDFDataStore *>(scan_->rawData());

	if(dataStore && !dataStore->flushToDisk())
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, AMXRFSCANCONTROLLER_COULD_NOT_WRITE_TO_CDF, "Error saving the currently-running scan's raw data file to disk. Watch out... your data may not be saved! Please report this bug to your beamline's software developers."));
}
