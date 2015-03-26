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


#include "SXRMBXRFScanController.h"

#include <QDir>


#include "analysis/AMDeadTimeAB.h"
#include "analysis/AM1DSummingAB.h"
#include "dataman/AMUser.h"
#include "dataman/datasource/AMRawDataSource.h"

#include "application/SXRMB/SXRMB.h"
#include "beamline/SXRMB/SXRMBBeamline.h"


SXRMBXRFScanController::~SXRMBXRFScanController(){}

SXRMBXRFScanController::SXRMBXRFScanController(SXRMBXRFScanConfiguration *scanConfig, QObject *parent)
	: AMScanController(scanConfig, parent)
{
	detector_ = qobject_cast<AMXRFDetector *>(SXRMBBeamline::sxrmb()->exposedDetectorByInfo(scanConfig->detectorConfigurations().at(0)));

	scan_ = new AMXRFScan;
	scan_->setScanConfiguration(scanConfig);
	scan_->setName(QString("XRF Scan - %1 el").arg(detector_->elements()));

	scan_->setFilePath(AMUserSettings::defaultRelativePathForScan(QDateTime::currentDateTime()) + ".dat");
	scan_->setFileFormat("vespers2011XRF");
	scan_->setRunId(AMUser::user()->currentRunId());

	int elements = detector_->elements();

	for (int i = 0; i < elements; i++){

		scan_->rawData()->addMeasurement(AMMeasurementInfo(QString("raw%1").arg(i+1), QString("Element %1").arg(i+1), "eV", detector_->axes()));
		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), i), true, false);
	}

	if (detector_->inputCountSources().size() > 0) {
		for (int i = 0; i < elements; i++){

			scan_->rawData()->addMeasurement(AMMeasurementInfo(QString("icr%1").arg(i+1), QString("Input count rate %1").arg(i+1), "%", QList<AMAxisInfo>()));
			scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), i+elements), false, true);
		}
	}

	if (detector_->outputCountSources().size() > 0) {
		for (int i = 0; i < elements; i++){

			scan_->rawData()->addMeasurement(AMMeasurementInfo(QString("ocr%1").arg(i+1), QString("Output count rate %1").arg(i+1), "%", QList<AMAxisInfo>()));
			scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), i+2*elements), false, true);
		}
	}

	if (detector_->inputCountSources().size() > 0 && detector_->outputCountSources().size() > 0) {
		for (int i = 0; i < elements; i++){

			AMDeadTimeAB *temp = new AMDeadTimeAB(QString("Corrected %1").arg(i+1));
			temp->setInputDataSourcesImplementation(QList<AMDataSource *>() << (AMDataSource *)scan_->rawDataSources()->at(i) << (AMDataSource *)scan_->rawDataSources()->at(i+elements) << (AMDataSource *)scan_->rawDataSources()->at(i+2*elements));
			scan_->addAnalyzedDataSource(temp, true, false);
		}
	}

	if (elements > 1){

		AM1DSummingAB *corr = new AM1DSummingAB("corrSum");
		QList<AMDataSource *> list;
		for (int i = 0; i < scan_->analyzedDataSourceCount(); i++)
			list << (AMDataSource *)scan_->analyzedDataSources()->at(i);
		corr->setInputDataSourcesImplementation(list);
		scan_->addAnalyzedDataSource(corr, true, false);
	}
}

void SXRMBXRFScanController::onProgressUpdate()
{
	emit progress(detector_->elapsedTime(), detector_->acquisitionTime());
}

void SXRMBXRFScanController::onStatusChanged()
{
	if (!detector_->isAcquiring())
		onDetectorAcquisitionFinished();
}

bool SXRMBXRFScanController::startImplementation()
{
	if (!detector_->isConnected() || !detector_->isInitialized()) {
		QString errorMessage = QString("The bruker detector %1 is not connected or initialized.").arg(detector_->name());
		AMErrorMon::alert(this, ERR_SXRMB_XRF_DETECTOR_NOT_CONNECTED, errorMessage, true);
		return false;
	}

	connect(detector_, SIGNAL(acquisitionStateChanged(AMDetector::AcqusitionState)), this, SLOT(onStatusChanged()));
	connect(detector_, SIGNAL(elapsedTimeChanged(double)), this, SLOT(onProgressUpdate()));
	detector_->acquire();

	setStarted();

	return true;
}

void SXRMBXRFScanController::onDetectorAcquisitionFinished()
{
	disconnect(detector_, SIGNAL(acquisitionStateChanged(AMDetector::AcqusitionState)), this, SLOT(onStatusChanged()));
	disconnect(detector_, SIGNAL(elapsedTimeChanged(double)), this, SLOT(onProgressUpdate()));

	QVector<double> spectrum = QVector<double>(detector_->size(0));

	for (int i = 0, elements = detector_->elements(); i < elements; i++){

		detector_->rawSpectrumSources().at(i)->values(AMnDIndex(0), AMnDIndex(detector_->size(0)-1), spectrum.data());
		scan_->rawData()->setValue(AMnDIndex(), i, spectrum.constData());
		if (detector_->inputCountSources().size() > 0)
			scan_->rawData()->setValue(AMnDIndex(), i+elements, AMnDIndex(), detector_->inputCountSourceAt(i)->value(AMnDIndex()));

		if (detector_->outputCountSources().size() > 0)
			scan_->rawData()->setValue(AMnDIndex(), i+2*elements, AMnDIndex(), detector_->outputCountSourceAt(i)->value(AMnDIndex()));
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

void SXRMBXRFScanController::saveData()
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