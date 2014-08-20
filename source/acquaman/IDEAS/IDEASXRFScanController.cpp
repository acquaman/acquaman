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


#include "IDEASXRFScanController.h"
#include "dataman/AMUser.h"
#include "dataman/datasource/AMRawDataSource.h"
#include "beamline/IDEAS/IDEASBeamline.h"
#include "analysis/AMDeadTimeAB.h"
#include "analysis/AM1DSummingAB.h"
#include "ui/CLS/CLSSIS3820ScalerView.h"
#include "beamline/CLS/CLSSIS3820Scaler.h"


#include <QDir>

IDEASXRFScanController::IDEASXRFScanController(IDEASXRFScanConfiguration *scanConfig, QObject *parent) :
	AMScanController(scanConfig, parent)
{
	detector_ = IDEASBeamline::ideas()->ketek();
	configuration_ = scanConfig;

	configuration_->setDetectorInfo(detector_->toInfo());


	scan_ = new AMScan;
	scan_->setScanConfiguration(configuration_);
	scan_->setName(QString("XRF Scan"));

	scan_->setFilePath(AMUserSettings::defaultRelativePathForScan(QDateTime::currentDateTime()) + ".dat");
	scan_->setFileFormat("vespers2011XRF");
	scan_->setRunId(AMUser::user()->currentRunId());

	if(configuration_->userScanName() == "") scan_->setName(QString("XRF Scan"));
	else scan_->setName(configuration_->userScanName());


	scan_->setNumber(configuration_->scanNumber());
	scan_->setNotes(configuration_->scanNotes());

	scan_->rawData()->addMeasurement(AMMeasurementInfo(QString("raw"), QString("RAW XRF Data"), "eV", detector_->axes()));
	scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), 0), true, false);

	scan_->rawData()->addMeasurement(AMMeasurementInfo(QString("icr"), QString("Input count rate"), "%", QList<AMAxisInfo>()));
	scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), 1), false, true);

	scan_->rawData()->addMeasurement(AMMeasurementInfo(QString("ocr"), QString("Output count rate"), "%", QList<AMAxisInfo>()));
	scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), 2), false, true);

	AMDeadTimeAB *temp = new AMDeadTimeAB(QString("Corrected"));
	temp->setInputDataSources(QList<AMDataSource *>() << (AMDataSource *)scan_->rawDataSources()->at(0) << (AMDataSource *)scan_->rawDataSources()->at(1) << (AMDataSource *)scan_->rawDataSources()->at(2));
	scan_->addAnalyzedDataSource(temp, true, false);





}

IDEASXRFScanController::~IDEASXRFScanController(){}

bool IDEASXRFScanController::startImplementation()
{
		//detector_->start();

		setStarted();
		QVector<double> data = QVector<double>(2048);
		detector_->data(data.data());
		scan_->rawData()->setValue(AMnDIndex(), 0, data.constData());
		scan_->rawData()->setValue(AMnDIndex(), 1, AMnDIndex(), detector_->inputCountSourceAt(0)->value(AMnDIndex()));
		scan_->rawData()->setValue(AMnDIndex(), 2, AMnDIndex(), detector_->outputCountSourceAt(0)->value(AMnDIndex()));

		scan_->setScanInitialConditions(configuration_->positions());

		scan()->setScanController(0);

		saveData();
		if(scan()->database())
				scan()->storeToDb(scan()->database());
		else
				scan()->storeToDb(AMDatabase::database("user"));

		setFinished();

		return true;
}

void IDEASXRFScanController::onDetectorAcquisitionFinished()
{



	    setFinished();
}

void IDEASXRFScanController::saveData()
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
