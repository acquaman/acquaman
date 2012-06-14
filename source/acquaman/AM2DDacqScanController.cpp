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


#include "AM2DDacqScanController.h"

#include <QDir>

AM2DDacqScanController::AM2DDacqScanController(AM2DScanConfiguration *cfg, QObject *parent)
	: AMDacqScanController(cfg, parent)
{
	internal2DConfig_ = cfg;
	xPosition_ = 0;
	yPosition_ = 0;
	fastAxisStartPosition_ = 0;
	useDwellTimes_ = false;
	stopAtEndOfLine_ = false;
	duplicateColumnsDetected_ = false;
}

bool AM2DDacqScanController::startImplementation()
{
	acqBaseOutput *abop = acqOutputHandlerFactory::new_acqOutput("AMScanSpectrum", "File");
	if( abop)
	{
		if (!setScanAxesControl()){

			AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, AM2DDACQSCANCONTROLLER_DACQ_INITIALIZATION_FAILED, "AM2DDacqScanController: could not set the x and y axes."));
			return false;
		}

		acqEvent_t *ev;
		ev = first_acqEvent(advAcq_->getMaster());
		if(!ev || !ev->numPvList ){
			AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, AM2DDACQSCANCONTROLLER_DACQ_INITIALIZATION_FAILED, "AM2DDacqScanController: dacq initialization was unsuccessful."));
			return false;
		}
		if(ev->numPvList < 1){
			AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, AM2DDACQSCANCONTROLLER_NO_X_COLUMN, "AM2DDacqScanController: no implied x-column set."));
			return false;
		}
		if (ev->numPvList < 2){

			AMErrorMon::alert(0, AM2DDACQSCANCONTROLLER_NO_Y_COLUMN, "AM2DDacqScanController: no implied y-column set.");
			return false;
		}
		// Ensure that the dacq hasn't misloaded our x-column and set it to "NoRecord = true"
		if(ev->pvList[0].noRecord == 1)
			ev->pvList[0].noRecord = 0;

		// Ensure that the dacq hasn't misloaded our y-column and set it to "NoRecord = true"
		if (ev->pvList[1].noRecord == 1)
			ev->pvList[1].noRecord = 0;

		if(useDwellTimes_)
			connect(dwellTimeTrigger_, SIGNAL(valueChanged(double)), this, SLOT(onDwellTimeTriggerChanged(double)));

		// Setting the fast axis position to -1 for counting purposes.
		if (internal2DConfig_->fastAxis() == AM2DScanConfiguration::X)
			xPosition_ = -1;
		else
			yPosition_ = -1;

		prefillScanPoints();

		acqRegisterOutputHandler( advAcq_->getMaster(), (acqKey_t) abop, &abop->handler);                // register the handler with the acquisition

		QFileInfo fullPath(AMUserSettings::defaultRelativePathForScan(QDateTime::currentDateTime()));	// ex: 2010/09/Mon_03_12_24_48_0000   (Relative, and with no extension)

		QString path = fullPath.path();// just the path, not the file name. Still relative.
		QString file = fullPath.fileName() + ".dat"; // just the file name, now with an extension

		abop->setProperty( "File Template", file.toStdString());
		abop->setProperty( "File Path", (AMUserSettings::userDataFolder + "/" + path).toStdString());	// given an absolute path here

		scan_->setFilePath(fullPath.filePath()+".dat");	// relative path and extension (is what the database wants)
		if(usingSpectraDotDatFile_){
			// qdebug() << "dacq scan controller: setting additional file paths: " << (QStringList() << fullPath.filePath()+"_spectra.dat");
			scan_->setAdditionalFilePaths( QStringList() << fullPath.filePath()+"_spectra.dat" );
			((AMAcqScanSpectrumOutput*)abop)->setExpectsSpectrumFromScanController(true);
		}
		else {
			// qdebug() << "dacq scan controller: not using spectraDotDat file.";
		}

		((AMAcqScanSpectrumOutput*)abop)->setScan(scan_);
		((AMAcqScanSpectrumOutput*)abop)->setScanController(this);

		advAcq_->Start();
		return true;
	}
	else{
		AMErrorMon::alert(0, AM2DDACQSCANCONTROLLER_CANT_CREATE_OUTPUTHANDLER, "AM2DDacqScanController: could not create output handler.");
		return false;
	}
}

/// Hackish for now.
#include <QMessageBox>

bool AM2DDacqScanController::event(QEvent *e)
{
	// This makes sure that the data is put in the appropriate location.
	if(e->type() == (QEvent::Type)AM::AcqEvent){

		QMap<int, double> aeData = ((AMAcqEvent*)e)->dataPackage_;
		QMap<int, QList<double> > aeSpectra = ((AMAcqEvent*)e)->spectraPackage_;
		QMap<int, double>::const_iterator i = aeData.constBegin();
		QMap<int, QList<double> >::const_iterator j = aeSpectra.constBegin();

		if(i.key() == 0 && aeData.count() > 1){

			AMnDIndex insertIndex = toScanIndex(aeData);
			scan_->rawData()->beginInsertRowsAsNecessaryForScanPoint(insertIndex);

			// Because this is a 2D specific scan controller, I am forcing the format to be a certain way.
			scan_->rawData()->setAxisValue(0, insertIndex.i(), i.value());
			++i;
			scan_->rawData()->setAxisValue(1, insertIndex.j(), i.value());
			++i;

			QList<double> temp = aeData.values();

			// This is too sensitive at the moment.
//			if (temp.size() > 5 && !duplicateColumnsDetected_)
//				for (int x = 5; x < temp.size(); x++)
//					if (temp.at(x) == temp.at(x-1)){

//						duplicateColumnsDetected_ = true;
//						AMErrorMon::alert(this, AM2DDACQSCANCONTROLLER_DUPLICATE_COLUMNS_DETECTED, QString("Duplicate columns detected at %1 and %2").arg(x).arg(x-1));
//						QMessageBox::warning(0, "Duplicate Columns Detected", "An error has occurred within the scan engine where data is not being saved correctly.  Please contact Darren at 290-5418.");
//					}

			while(i != aeData.constEnd()){
				scan_->rawData()->setValue(insertIndex, i.key()-2, AMnDIndex(), i.value());
				++i;
			}

			while(j != aeSpectra.constEnd()){
				for(int x = 0; x < j.value().count(); x++)
					scan_->rawData()->setValue(insertIndex, j.key()-1, AMnDIndex(x), j.value().at(x));
				++j;
			}

			scan_->rawData()->endInsertRows();

			if (stopAtEndOfLine_ && atEndOfLine(aeData)){

				// Make sure that the AMScanController knows that the scan has NOT been cancelled.  This way the scan will still be auto-exported.
				dacqCancelled_ = false;
				advAcq_->Stop();
			}

		}

		e->accept();

		return true;
	}

	else if(e->type() == (QEvent::Type)AM::AcqErrorEvent){

		int errorCode = ((AMAcqErrorEvent*)e)->errorCode_;
		QString errorExplanation = ((AMAcqErrorEvent*)e)->errorExplanation_;
		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, AM2DDACQSCANCONTROLLER_NO_SPECTRUM_FILE, QString("%1 %2 %3").arg("AM2DDacqScanController:").arg(errorExplanation).arg(errorCode)));
		cancelImplementation();

		return true;
	}
	else
		return AMScanController::event(e);
}

AMnDIndex AM2DDacqScanController::toScanIndex(QMap<int, double> aeData)
{
	// Increment the fast axis.  If the fast axis is at the end of the road, set it to 0 and increment the slow axis.
	switch(internal2DConfig_->fastAxis()){

		case AM2DScanConfiguration::X: {

			if (xPosition_ == -1 && yPosition_ == 0){

				fastAxisStartPosition_ = aeData.value(0);
				xPosition_++;
			}

			else if (fastAxisStartPosition_ == aeData.value(0)){

				xPosition_ = 0;
				yPosition_++;
			}

			else
				xPosition_++;

			break;
		}

		case AM2DScanConfiguration::Y: {

		if (yPosition_ == -1 && xPosition_ == 0){

			fastAxisStartPosition_ = aeData.value(1);
			yPosition_++;
		}

		else if (fastAxisStartPosition_ == aeData.value(1)){

			yPosition_ = 0;
			xPosition_++;
		}

		else
			yPosition_++;

			break;
		}
	}

	return AMnDIndex(xPosition_, yPosition_);
}

bool AM2DDacqScanController::atEndOfLine(QMap<int, double> aeData) const
{
	if (internal2DConfig_->fastAxis() == AM2DScanConfiguration::X && (internal2DConfig_->xEnd() - aeData.value(0) < internal2DConfig_->xStep()/2))
		return true;

	else if (internal2DConfig_->fastAxis() == AM2DScanConfiguration::Y && (internal2DConfig_->yEnd() - aeData.value(1) < internal2DConfig_->yStep()/2))
		return true;

	return false;
}

void AM2DDacqScanController::prefillScanPoints()
{
	switch(internal2DConfig_->fastAxis()){

		case AM2DScanConfiguration::X: {

			AMnDIndex insertIndex;
			int xCount = 0;
			int yCount = 0;
			double xStart = internal2DConfig_->xStart();
			double xStep = internal2DConfig_->xStep();
			double xEnd = internal2DConfig_->xEnd();
			double yStart = internal2DConfig_->yStart();
			double yStep = internal2DConfig_->yStep();
			double yEnd = internal2DConfig_->yEnd();
			double floatNumX = (xEnd-xStart)/xStep;
			double floatNumY = (yEnd-yStart)/yStep;

			if (fabs((floatNumX-int(floatNumX))) < 0.1*xStep)
				xCount = int(floatNumX) + 1;
			else
				xCount = int(floatNumX) + 2;

			if (fabs((floatNumY-int(floatNumY))) < 0.1*yStep)
				yCount = int(floatNumY) + 1;
			else
				yCount = int(floatNumY) + 2;

			for (int j = 0; j < yCount; j++){

				for (int i = 0; i < xCount; i++){

					insertIndex = AMnDIndex(i, j);
					scan_->rawData()->beginInsertRowsAsNecessaryForScanPoint(insertIndex);
					scan_->rawData()->setAxisValue(0, insertIndex.i(), xStart + i*xStep);
					scan_->rawData()->setAxisValue(1, insertIndex.j(), yStart + j*yStep);

					for (int di = 0; di < scan_->dataSourceCount(); di++)
						scan_->rawData()->setValue(insertIndex, di, AMnDIndex(), 0);

					scan_->rawData()->endInsertRows();
				}
			}

			break;
		}

		case AM2DScanConfiguration::Y: {

			AMnDIndex insertIndex;
			int xCount = 0;
			int yCount = 0;
			double xStart = internal2DConfig_->xStart();
			double xStep = internal2DConfig_->xStep();
			double xEnd = internal2DConfig_->xEnd();
			double yStart = internal2DConfig_->yStart();
			double yStep = internal2DConfig_->yStep();
			double yEnd = internal2DConfig_->yEnd();
			double floatNumX = (xEnd-xStart)/xStep;
			double floatNumY = (yEnd-yStart)/yStep;

			if ((floatNumX-int(floatNumX)) == 0)
				xCount = int(floatNumX) + 1;
			else
				xCount = int(floatNumX) + 2;

			if ((floatNumY-int(floatNumY)) == 0)
				yCount = int(floatNumY) + 1;
			else
				yCount = int(floatNumY) + 2;

			for (int i = 0; i < yCount; i++){

				for (int j = 0; j < xCount; j++){

					insertIndex = AMnDIndex(i, j);
					scan_->rawData()->beginInsertRowsAsNecessaryForScanPoint(insertIndex);
					scan_->rawData()->setAxisValue(0, insertIndex.i(), xStart + i*xStep);
					scan_->rawData()->setAxisValue(1, insertIndex.j(), yStart + j*yStep);

					for (int di = 0; di < scan_->dataSourceCount(); di++)
						scan_->rawData()->setValue(insertIndex, di, AMnDIndex(), 0);

					scan_->rawData()->endInsertRows();
				}
			}

			break;
		}
	}
}

bool AM2DDacqScanController::setConfigFile(const QString &filename)
{
	filename_ = filename;
	return advAcq_->setConfigFile(filename);
}

bool AM2DDacqScanController::setScanAxesControl()
{
	if (xAxisPVName().isEmpty() || yAxisPVName().isEmpty())
		return false;

	// Stage 1:  Save the current state of the config file.
	QString filename = filename_;
	filename = filename.left(filename.lastIndexOf("/"));
	filename.append("/interim.cfg");
	advAcq_->saveConfigFile(filename);

	// Stage 2:  Setup the slow axis and fast axis strings.
	QString fastAxis = "";
	QString slowAxis = "";

	// First is the slow axis.
	switch(internal2DConfig_->fastAxis()){

	case AM2DScanConfiguration::X:

		fastAxis = QString("# Control \"%1\" start:%2 delta: %3 final:%4 active: 7")
				.arg(xAxisPVName())
				.arg(internal2DConfig_->xStart())
				.arg(internal2DConfig_->xStep())
				.arg(internal2DConfig_->xEnd());

		slowAxis = QString("# Control \"%1\" start:%2 delta: %3 final:%4 active: 7")
				.arg(yAxisPVName())
				.arg(internal2DConfig_->yStart())
				.arg(internal2DConfig_->yStep())
				.arg(internal2DConfig_->yEnd());

		break;

	case AM2DScanConfiguration::Y:

		slowAxis = QString("# Control \"%1\" start:%2 delta: %3 final:%4 active: 7")
				.arg(xAxisPVName())
				.arg(internal2DConfig_->xStart())
				.arg(internal2DConfig_->xStep())
				.arg(internal2DConfig_->xEnd());

		fastAxis = QString("# Control \"%1\" start:%2 delta: %3 final:%4 active: 7")
				.arg(yAxisPVName())
				.arg(internal2DConfig_->yStart())
				.arg(internal2DConfig_->yStep())
				.arg(internal2DConfig_->yEnd());

		break;
	}

	// Stage 3:  Load up the file through a text stream and change the controls for Both scan axes.
	QFile inputFile(filename);
	if(!inputFile.open(QIODevice::ReadOnly)) {
		AMErrorMon::error(0, -1, "AM2DDacqScanController:  could not open the config file.");
		return false;
	}

	QTextStream in(&inputFile);
	QString current;
	QStringList completeFile;

	while (!(current = in.readLine()).startsWith("# Control")){

		completeFile << current;
	}

	completeFile << slowAxis;

	while (!(current = in.readLine()).startsWith("# Control")){

		completeFile << current;
	}

	completeFile << fastAxis;

	while (!(current = in.readLine()).isNull()){

		completeFile << current;
	}

	inputFile.close();

	// Stage 4:  Save the file back as interim.cfg.
	QFile outputFile(filename);
	if(!outputFile.open(QIODevice::WriteOnly)) {
		AMErrorMon::error(0, -1, "AM2DDacqScanController:  could not open the config file.");
		return false;
	}

	QTextStream out(&outputFile);

	for (int i = 0; i < completeFile.size(); i++)
		out << completeFile.at(i) << "\n";

	outputFile.close();
	completeFile.clear();

	// Stage 5:  Reload the config file.
	return advAcq_->setConfigFile(filename);
}
