#include "BioXASXRFScanController.h"

#include "dataman/AMUser.h"
#include "dataman/datasource/AMRawDataSource.h"
#include "analysis/AM1DSummingAB.h"
#include "beamline/AMBeamline.h"
#include "beamline/AMXspress3XRFDetector.h"

#include <QDir>

BioXASXRFScanController::BioXASXRFScanController(BioXASXRFScanConfiguration *configuration, QObject *parent)
	: AMScanController(configuration, parent)
{
	detector_ = qobject_cast<AMXRFDetector *>(AMBeamline::bl()->exposedDetectorByInfo(configuration->detectorConfigurations().at(0)));

	scan_ = new AMScan;
	scan_->setScanConfiguration(configuration);
	scan_->setName(QString("XRF Scan - %1 el").arg(detector_->elements()));

	scan_->setFilePath(AMUserSettings::defaultRelativePathForScan(QDateTime::currentDateTime()) + ".dat");
	scan_->setFileFormat("vespers2011XRF");
	scan_->setRunId(AMUser::user()->currentRunId());

	int elements = detector_->elements();

	AMXspress3XRFDetector *detector = qobject_cast<AMXspress3XRFDetector *>(detector_);

	QString notes;

	notes.append(QString("%1\n").arg(detector_->description()));
	notes.append(QString("Acquisition time: %1 seconds\n").arg(detector->acquisitionTime()));
	notes.append(QString("Frame %1 of %2\n").arg(detector->currentFrame()+1).arg(detector->framesPerAcquisition()));
	notes.append(QString("This should list the enable/disabled channels"));
	notes.append("\n");

	scan_->setNotes(notes);

	for (int i = 0; i < elements; i++){

		scan_->rawData()->addMeasurement(AMMeasurementInfo(QString("raw%1").arg(i+1), QString("Element %1").arg(i+1), "eV", detector_->axes()));
		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), i), true, false);
	}

	AM1DSummingAB *summedSpectra = new AM1DSummingAB("SummedSpectra");
	QList<AMDataSource *> list;

	for (int i = 0; i < scan_->rawDataSourceCount(); i++)
		list << (AMDataSource *)scan_->rawDataSources()->at(i);

	summedSpectra->setInputDataSources(list);
	scan_->addAnalyzedDataSource(summedSpectra, true, false);
}

BioXASXRFScanController::~BioXASXRFScanController()
{

}

void BioXASXRFScanController::onProgressUpdate()
{
	emit progress(detector_->elapsedTime(), detector_->acquisitionTime());
}

void BioXASXRFScanController::onStatusChanged()
{
	AMDetector::AcqusitionState state = detector_->acquisitionState();

	if (state == AMDetector::Succeeded || state == AMDetector::Cancelled || state == AMDetector::Failed)
		onDetectorAcquisitionFinished();
}

bool BioXASXRFScanController::startImplementation()
{
	connect(detector_, SIGNAL(acquisitionStateChanged(AMDetector::AcqusitionState)), this, SLOT(onStatusChanged()));
	connect(detector_, SIGNAL(elapsedTimeChanged(double)), this, SLOT(onProgressUpdate()));
	detector_->acquire();

	setStarted();

	return true;
}

void BioXASXRFScanController::onDetectorAcquisitionFinished()
{
	disconnect(detector_, SIGNAL(acquisitionStateChanged(AMDetector::AcqusitionState)), this, SLOT(onStatusChanged()));
	disconnect(detector_, SIGNAL(elapsedTimeChanged(double)), this, SLOT(onProgressUpdate()));

	QVector<double> spectrum = QVector<double>(detector_->size(0));

	for (int i = 0, elements = detector_->elements(); i < elements; i++){

		detector_->rawSpectrumSources().at(i)->values(AMnDIndex(0), AMnDIndex(detector_->size(0)-1), spectrum.data());
		scan_->rawData()->setValue(AMnDIndex(), i, spectrum.constData());
	}

	// MB: Notice for Darren: at setFinished(), AMScanController will automatically ensure scan()->setScanController(0), and this will trigger a change in the scan's db state. (currentlyScanning()).
	// If you want this to not create a prompt in AMGenericScanEditor ("The scan has been modified... Do you want to save it?") can do this first:
	scan()->setScanController(0);
	// before saving to the DB.

	saveData();

	if(scan()->database())
		scan()->storeToDb(scan()->database());

	else
		scan()->storeToDb(AMDatabase::database("user"));

	setFinished();
}

void BioXASXRFScanController::saveData()
{
	QFile file(AMUserSettings::userDataFolder + "/" + scan_->filePath());
	if(!file.open(QIODevice::WriteOnly)) {
		AMErrorMon::error(this, -1, "Could not save XRF data.");
		return;
	}

	QTextStream out(&file);

	// There are two types of raw data sources.  The spectra of rank 1 and the dead time of rank 0.
	for (int i = 0; i < scan_->rawDataSources()->count(); i++){

		if (scan_->rawDataSources()->at(i)->rank() == 1){

			qint32 counts = scan_->rawData()->value(AMnDIndex(), i, AMnDIndex(0));
			out << counts;

			for (int j = 1; j < scan_->rawData()->measurementAt(i).size(0); j++){

				qint32 counts = scan_->rawData()->value(AMnDIndex(), i, AMnDIndex(j));
				out << "," << counts;
			}
		}
		else {

			double counts = scan_->rawData()->value(AMnDIndex(), i, AMnDIndex());
			out << counts;
		}

		out << "\n";
	}

	file.close();
}
