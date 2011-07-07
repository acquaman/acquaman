#include "VESPERSXRFScanController.h"
#include "dataman/AMUser.h"
#include "dataman/AMRawDataSource.h"
#include "beamline/VESPERS/VESPERSBeamline.h"
#include "dataman/VESPERS/VESPERSXRFDataLoader.h"
#include "analysis/AMDeadTimeAB.h"
#include "analysis/AM1DSummingAB.h"

#include <QDir>

VESPERSXRFScanController::VESPERSXRFScanController(VESPERSXRFScanConfiguration *scanConfig, QObject *parent)
	: AMScanController(scanConfig, parent)
{
	if (scanConfig->detectorInfo().elements() == 1)
		detector_ = VESPERSBeamline::vespers()->vortexXRF1E();
	else
		detector_ = VESPERSBeamline::vespers()->vortexXRF4E();

	scanConfig->setDetectorInfo(detector_->toXRFInfo());

	scan_ = new AMXRFScan;
	generalScan_ = scan_;
	scan_->setScanConfiguration(scanConfig);
	scan_->setName(QString("XRF Scan - %1 el").arg(detector_->elements()));

	scan_->setFilePath(AMUserSettings::defaultRelativePathForScan(QDateTime::currentDateTime()) + ".dat");
	scan_->setFileFormat("vespersXRF");
	scan_->setRunId(AMUser::user()->currentRunId());

	int elements = detector_->elements();

	for (int i = 0; i < elements; i++){

		scan_->rawData()->addMeasurement(AMMeasurementInfo(QString("raw%1").arg(i+1), QString("Element %1").arg(i+1), "eV", detector_->axes()));
		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), i));
	}

	for (int i = 0; i < elements; i++){

		scan_->rawData()->addMeasurement(AMMeasurementInfo(QString("icr%1").arg(i+1), QString("Input count rate %1").arg(i+1), "%", QList<AMAxisInfo>()));
		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), i+elements));
	}

	for (int i = 0; i < elements; i++){

		scan_->rawData()->addMeasurement(AMMeasurementInfo(QString("ocr%1").arg(i+1), QString("Output count rate %1").arg(i+1), "%", QList<AMAxisInfo>()));
		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), i+2*elements));
	}

	for (int i = 0; i < elements; i++){

		AMDeadTimeAB *temp = new AMDeadTimeAB(QString("Corrected %1").arg(i+1));
		temp->setInputDataSourcesImplementation(QList<AMDataSource *>() << (AMDataSource *)scan_->rawDataSources()->at(i) << (AMDataSource *)scan_->rawDataSources()->at(i+elements) << (AMDataSource *)scan_->rawDataSources()->at(i+2*elements));
		scan_->addAnalyzedDataSource(temp);
	}

	if (elements > 1){

		AM1DSummingAB *corr = new AM1DSummingAB("corrSum");
		QList<AMDataSource *> list;
		for (int i = 0; i < scan_->analyzedDataSourceCount(); i++)
			list << (AMDataSource *)scan_->analyzedDataSources()->at(i);
		corr->setInputDataSourcesImplementation(list);
		scan_->addAnalyzedDataSource(corr);
	}
}

void VESPERSXRFScanController::onProgressUpdate()
{
	emit progress(detector_->elapsedTime(), detector_->integrationTime());
}

void VESPERSXRFScanController::onStatusChanged()
{
	if (!detector_->status())
		onDetectorAcquisitionFinished();
}

bool VESPERSXRFScanController::startImplementation()
{
	connect(detector_, SIGNAL(statusChanged(bool)), this, SLOT(onStatusChanged()));
	connect(detector_, SIGNAL(elapsedTimeChanged(double)), this, SLOT(onProgressUpdate()));
	detector_->start();

	setStarted();

	return true;
}

void VESPERSXRFScanController::onDetectorAcquisitionFinished()
{
	disconnect(detector_, SIGNAL(statusChanged(bool)), this, SLOT(onStatusChanged()));
	disconnect(detector_, SIGNAL(elapsedTimeChanged(double)), this, SLOT(onProgressUpdate()));


	for (int i = 0; i < detector_->elements(); i++){

		QVector<int> currSpectra(detector_->spectraValues(i));
		scan_->rawData()->setValue(AMnDIndex(), i, currSpectra.constData(), detector_->channels());
		scan_->rawData()->setValue(AMnDIndex(), i+detector_->elements(), AMnDIndex(), detector_->inputCountRate(i));
		scan_->rawData()->setValue(AMnDIndex(), i+2*detector_->elements(), AMnDIndex(), detector_->outputCountRate(i));
	}

	if(scan()->database())
		scan()->storeToDb(scan()->database());
	else
		scan()->storeToDb(AMDatabase::userdb());
	saveData();

	setFinished();
}

void VESPERSXRFScanController::saveData()
{
	VESPERSXRFDataLoader exporter(scan_);
	if (!exporter.saveToFile(AMUserSettings::userDataFolder + "/" + scan_->filePath()))
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Serious, 0, QString("Could not save XRF data.")));
}
