/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "VESPERSXRFScanController.h"
#include "dataman/AMUser.h"
#include "dataman/datasource/AMRawDataSource.h"
#include "beamline/VESPERS/VESPERSBeamline.h"
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
	scan_->setFileFormat("vespers2011XRF");
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


	//scan_->rawData()->setAxisValue();

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
	QFile file(AMUserSettings::userDataFolder + "/" + scan_->filePath());
	if(!file.open(QIODevice::WriteOnly)) {
		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Serious, -1, "Could not save XRF data."));
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
