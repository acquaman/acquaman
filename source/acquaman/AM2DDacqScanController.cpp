#include "AM2DDacqScanController.h"

#include <QDir>

AM2DDacqScanController::AM2DDacqScanController(AMScanConfiguration *cfg, QObject *parent)
	: AMDacqScanController(cfg, parent)
{
	xPosition_ = 0;
	yPosition_ = 0;
}

bool AM2DDacqScanController::startImplementation()
{
	acqBaseOutput *abop = acqOutputHandlerFactory::new_acqOutput("AMScanSpectrum", "File");
	if( abop)
	{
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

		acqRegisterOutputHandler( advAcq_->getMaster(), (acqKey_t) abop, &abop->handler);                // register the handler with the acquisition

		QFileInfo fullPath(AMUserSettings::defaultRelativePathForScan(QDateTime::currentDateTime()));	// ex: 2010/09/Mon_03_12_24_48_0000   (Relative, and with no extension)

		QString path = fullPath.path();// just the path, not the file name. Still relative.
		QString file = fullPath.fileName() + ".dat"; // just the file name, now with an extension

		abop->setProperty( "File Template", file.toStdString());
		abop->setProperty( "File Path", (AMUserSettings::userDataFolder + "/" + path).toStdString());	// given an absolute path here

		scan_->setFilePath(fullPath.filePath()+".dat");	// relative path and extension (is what the database wants)
		if(usingSpectraDotDatFile_){
			// qDebug() << "dacq scan controller: setting additional file paths: " << (QStringList() << fullPath.filePath()+"_spectra.dat");
			scan_->setAdditionalFilePaths( QStringList() << fullPath.filePath()+"_spectra.dat" );
			((AMAcqScanSpectrumOutput*)abop)->setExpectsSpectrumFromScanController(true);
		}
		else {
			// qDebug() << "dacq scan controller: not using spectraDotDat file.";
		}

		((AMAcqScanSpectrumOutput*)abop)->setScan(scan_);
		((AMAcqScanSpectrumOutput*)abop)->setScanController(this);
		advAcq_->Start();
		return true;
	}
	else{
		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, AM2DDACQSCANCONTROLLER_CANT_CREATE_OUTPUTHANDLER, "AM2DDacqScanController: could not create output handler."));
		return false;
	}
}

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

			while(i != aeData.constEnd()){
				scan_->rawData()->setValue(insertIndex, i.key()-1, AMnDIndex(), i.value());
				++i;
			}

			while(j != aeSpectra.constEnd()){
				for(int x = 0; x < j.value().count(); x++)
					scan_->rawData()->setValue(insertIndex, j.key()-1, AMnDIndex(x), j.value().at(x));
				++j;
			}

			scan_->rawData()->endInsertRows();
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
	Q_UNUSED(aeData);

	// Increment the fast axis (assumed to be the x axis).  If the fast axis is at the end of the road, set it to 0 and increment the slow axis.
	xPosition_++;

	if (xPosition_ == xAxisCount()){

		xPosition_ = 0;
		yPosition_++;
	}

	return AMnDIndex(xPosition_, yPosition_);
}
