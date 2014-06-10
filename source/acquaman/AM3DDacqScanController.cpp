#include "AM3DDacqScanController.h"

//#include "dataman/datastore/AMCDFDataStore.h"
//#include "dataman/datastore/AMInMemoryDataStore.h"

//#include <QDir>

//AM3DDacqScanController::AM3DDacqScanController(AM3DScanConfiguration *config, QObject *parent)
//	: AMDacqScanController(config, parent)
//{
//	internal3DConfig_ = config;
//	initializeStartPositions_ = true;
//	positions_ << 0 << 0 << 0;
//	firstPriorityAxisStartPosition_ = 0;
//	secondPriorityAxisStartPosition_ = 0;
//	useDwellTimes_ = false;
//	stopAtEndOfAxis_ = -1;

//	axisPriorities_ << 0;

//	if (internal3DConfig_->yPriority() > internal3DConfig_->xPriority())
//		axisPriorities_.prepend(1);

//	else
//		axisPriorities_ << 1;

//	if (internal3DConfig_->zPriority() > axisPriorities_.first())
//		axisPriorities_.prepend(2);

//	else if (internal3DConfig_->zPriority() > axisPriorities_.last())
//		axisPriorities_.insert(1, 2);

//	else
//		axisPriorities_ << 2;
//	qDebug() << axisPriorities_;
//}

//bool AM3DDacqScanController::startImplementation()
//{
//	acqBaseOutput *abop = acqOutputHandlerFactory::new_acqOutput("AMScanSpectrum", "File");
//	if( abop)
//	{
//		if (!setScanAxesControl()){

//			AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, AM3DDACQSCANCONTROLLER_DACQ_INITIALIZATION_FAILED, "AM3DDacqScanController: could not set the x, y, and z axes."));
//			return false;
//		}

//		acqEvent_t *ev;
//		ev = first_acqEvent(advAcq_->getMaster());

//		if(!ev || !ev->numPvList ){

//			AMErrorMon::alert(0, AM3DDACQSCANCONTROLLER_DACQ_INITIALIZATION_FAILED, "AM3DDacqScanController: dacq initialization was unsuccessful.");
//			return false;
//		}

//		if(ev->numPvList < 1){

//			AMErrorMon::alert(0, AM3DDACQSCANCONTROLLER_NO_X_COLUMN, "AM3DDacqScanController: no implied x-column set.");
//			return false;
//		}

//		if (ev->numPvList < 2){

//			AMErrorMon::alert(0, AM3DDACQSCANCONTROLLER_NO_Y_COLUMN, "AM3DDacqScanController: no implied y-column set.");
//			return false;
//		}

//		if (ev->numPvList < 3){

//			AMErrorMon::alert(0, AM3DDACQSCANCONTROLLER_NO_Z_COLUMN, "AM3DDacqScanController: no implied z-column set.");
//			return false;
//		}
//		// Ensure that the dacq hasn't misloaded our x-column and set it to "NoRecord = true"
//		if(ev->pvList[0].noRecord == 1)
//			ev->pvList[0].noRecord = 0;

//		// Ensure that the dacq hasn't misloaded our y-column and set it to "NoRecord = true"
//		if (ev->pvList[1].noRecord == 1)
//			ev->pvList[1].noRecord = 0;

//		// Ensure that the dacq hasn't misloaded our z-column and set it to "NoRecord = true"
//		if (ev->pvList[2].noRecord == 1)
//			ev->pvList[2].noRecord = 0;

//		if(useDwellTimes_)
//			connect(dwellTimeTrigger_, SIGNAL(valueChanged(double)), this, SLOT(onDwellTimeTriggerChanged(double)));

//		prefillScanPoints();

//		acqRegisterOutputHandler( advAcq_->getMaster(), (acqKey_t) abop, &abop->handler);                // register the handler with the acquisition

//		QFileInfo fullPath(AMUserSettings::defaultRelativePathForScan(QDateTime::currentDateTime()));	// ex: 2010/09/Mon_03_12_24_48_0000   (Relative, and with no extension)

//		QString path = fullPath.path();// just the path, not the file name. Still relative.
//		QString file = fullPath.fileName() + ".dat"; // just the file name, now with an extension

//		abop->setProperty( "File Template", file.toStdString());
//		abop->setProperty( "File Path", (AMUserSettings::userDataFolder + "/" + path).toStdString());	// given an absolute path here

//		// If using AMCDFDataStore then this will already have been set properly.
//		if (qobject_cast<AMInMemoryDataStore *>(scan_->rawData())){

//			QFileInfo fullPath(AMUserSettings::defaultRelativePathForScan(QDateTime::currentDateTime()));	// ex: 2010/09/Mon_03_12_24_48_0000   (Relative, and with no extension)

//			QString path = fullPath.path();// just the path, not the file name. Still relative.
//			QString file = fullPath.fileName() + ".dat"; // just the file name, now with an extension

//			abop->setProperty( "File Template", file.toStdString());
//			abop->setProperty( "File Path", (AMUserSettings::userDataFolder + "/" + path).toStdString());	// given an absolute path here

//			scan_->setFilePath(fullPath.filePath()+".dat");	// relative path and extension (is what the database wants)

//			if(usingSpectraDotDatFile_){
//				// qdebug() << "dacq scan controller: setting additional file paths: " << (QStringList() << fullPath.filePath()+"_spectra.dat");
//				scan_->setAdditionalFilePaths( QStringList() << fullPath.filePath()+"_spectra.dat" );
//				((AMAcqScanSpectrumOutput*)abop)->setExpectsSpectrumFromScanController(true);
//			}
//		}

//		// Synchronizing the .dat and _spectra.dat to match the cdf name.
//		else if (qobject_cast<AMCDFDataStore *>(scan_->rawData())){

//			QFileInfo fullPath(scan_->filePath());	// ex: 2010/09/Mon_03_12_24_48_0000   (Relative, and with no extension)

//			QString path = fullPath.path();// just the path, not the file name. Still relative.
//			QString file = fullPath.fileName().remove(".cdf") + ".dat"; // just the file name, now with an extension

//			abop->setProperty( "File Template", file.toStdString());
//			abop->setProperty( "File Path", (AMUserSettings::userDataFolder + "/" + path).toStdString());	// given an absolute path here
//			((AMAcqScanSpectrumOutput*)abop)->setExpectsSpectrumFromScanController(usingSpectraDotDatFile_);

//			flushToDiskTimer_.setInterval(300000);
//			connect(this, SIGNAL(started()), &flushToDiskTimer_, SLOT(start()));
//			connect(this, SIGNAL(cancelled()), &flushToDiskTimer_, SLOT(stop()));
//			connect(this, SIGNAL(paused()), &flushToDiskTimer_, SLOT(stop()));
//			connect(this, SIGNAL(resumed()), &flushToDiskTimer_, SLOT(start()));
//			connect(this, SIGNAL(failed()), &flushToDiskTimer_, SLOT(stop()));
//			connect(this, SIGNAL(finished()), &flushToDiskTimer_, SLOT(stop()));
//			connect(&flushToDiskTimer_, SIGNAL(timeout()), this, SLOT(flushCDFDataStoreToDisk()));
//			flushToDiskTimer_.start();
//		}

//		((AMAcqScanSpectrumOutput*)abop)->setScan(scan_);
//		((AMAcqScanSpectrumOutput*)abop)->setScanController(this);

//		advAcq_->Start();
//		return true;
//	}
//	else{
//		AMErrorMon::alert(0, AM3DDACQSCANCONTROLLER_CANT_CREATE_OUTPUTHANDLER, "AM3DDacqScanController: could not create output handler.");
//		return false;
//	}
//}

//bool AM3DDacqScanController::event(QEvent *e)
//{
//	// This makes sure that the data is put in the appropriate location.
//	if(e->type() == (QEvent::Type)AM::AcqEvent){

//		QMap<int, double> aeData = ((AMAcqEvent*)e)->dataPackage_;
//		QMap<int, QList<double> > aeSpectra = ((AMAcqEvent*)e)->spectraPackage_;
//		QMap<int, double>::const_iterator i = aeData.constBegin();
//		QMap<int, QList<double> >::const_iterator j = aeSpectra.constBegin();

//		if(i.key() == 0 && aeData.count() > 1){

//			AMnDIndex insertIndex = toScanIndex(aeData);qDebug() << insertIndex.i() << insertIndex.j() << insertIndex.k();
//			// MB: Modified May 13 2012 for changes to AMDataStore. Assumes data store already has sufficient space for scan axes beyond the first axis.
////			scan_->rawData()->beginInsertRowsAsNecessaryForScanPoint(insertIndex);
//			if(insertIndex.i() >= scan_->rawData()->scanSize(0))
//				scan_->rawData()->beginInsertRows(insertIndex.i()-scan_->rawData()->scanSize(0)+1, -1);
//			////////////////

//			// Because this is a 3D specific scan controller, I am forcing the format to be a certain way.
//			scan_->rawData()->setAxisValue(0, insertIndex.i(), i.value());
//			++i;
//			scan_->rawData()->setAxisValue(1, insertIndex.j(), i.value());
//			++i;
//			scan_->rawData()->setAxisValue(2, insertIndex.k(), i.value());
//			++i;

//			while(i != aeData.constEnd()){
//				scan_->rawData()->setValue(insertIndex, i.key()-3, AMnDIndex(), i.value());
//				++i;
//			}

//			while(j != aeSpectra.constEnd()){

//				QVector<double> data = j.value().toVector();
//				scan_->rawData()->setValue(insertIndex, j.key()-3, data.constData());
//				++j;
//			}

//			scan_->rawData()->endInsertRows();

//			if (stopImmediately_){

//				// Make sure that the AMScanController knows that the scan has NOT been cancelled.
//				dacqCancelled_ = false;
//				advAcq_->Stop();
//			}

//			else if (stopAtEndOfAxis_ != -1 && atEndOfAxis(stopAtEndOfAxis_, aeData)){

//				// Make sure that the AMScanController knows that the scan has NOT been cancelled.  This way the scan will still be auto-exported.
//				dacqCancelled_ = false;
//				advAcq_->Stop();
//			}

//		}

//		e->accept();

//		return true;
//	}

//	else if(e->type() == (QEvent::Type)AM::AcqErrorEvent){

//		int errorCode = ((AMAcqErrorEvent*)e)->errorCode_;
//		QString errorExplanation = ((AMAcqErrorEvent*)e)->errorExplanation_;
//		AMErrorMon::alert(0, AM3DDACQSCANCONTROLLER_NO_SPECTRUM_FILE, QString("%1 %2 %3").arg("AM3DDacqScanController:").arg(errorExplanation).arg(errorCode));
//		cancelImplementation();

//		return true;
//	}
//	else
//		return AMScanController::event(e);
//}

//AMnDIndex AM3DDacqScanController::toScanIndex(QMap<int, double> aeData)
//{
//	// Convenience members that hold the axes in priority order for code readability.
//	int first = axisPriorities_.at(0);
//	int second = axisPriorities_.at(1);
//	int third = axisPriorities_.at(2);
//	qDebug() << axisPriorities_;
//	// First point.  Sets the start positions for the fastest and second fastest axes.
//	if (initializeStartPositions_){

//		initializeStartPositions_ = false;
//		firstPriorityAxisStartPosition_ = aeData.value(first);
//		secondPriorityAxisStartPosition_ = aeData.value(second);
//	}

//	// After reaching the end of the fastest axis, we need to reset it's position to zero and increment the second fastest axis.
//	else if (firstPriorityAxisStartPosition_ == aeData.value(first)){

//		positions_[first] = 0;

//		// If we are also at the end of the second fastest axis, then set its position to zero and increment the slowest axis.
//		if (secondPriorityAxisStartPosition_ == aeData.value(second)){

//			positions_[second] = 0;
//			positions_[third]++;
//		}

//		else
//			positions_[second]++;
//	}

//	// Regular point.  Increment the fastest axis.
//	else
//		positions_[first]++;

//	return AMnDIndex(positions_.at(0), positions_.at(1), positions_.at(2));
//}

//bool AM3DDacqScanController::atEndOfAxis(int axis, const QMap<int, double> &aeData) const
//{
//	bool retVal = false;

//	// Switch from priority to a real axis.
//	switch (axisPriorities_.at(axis)){

//	case 0:	// X
//		retVal = internal3DConfig_->xEnd() - aeData.value(0) < internal3DConfig_->xStep()/2;
//		break;

//	case 1:	// Y
//		retVal = internal3DConfig_->yEnd() - aeData.value(1) < internal3DConfig_->yStep()/2;
//		break;

//	case 2:	// Z
//		retVal = internal3DConfig_->zEnd() - aeData.value(2) < internal3DConfig_->zStep()/2;
//		break;
//	}

//	return retVal;
//}

//void AM3DDacqScanController::prefillScanPoints()
//{
//	AMnDIndex insertIndex;
//	double xStart = internal3DConfig_->xStart();
//	double xStep = internal3DConfig_->xStep();
//	double xEnd = internal3DConfig_->xEnd();
//	double yStart = internal3DConfig_->yStart();
//	double yStep = internal3DConfig_->yStep();
//	double yEnd = internal3DConfig_->yEnd();
//	double zStart = internal3DConfig_->zStart();
//	double zStep = internal3DConfig_->zStep();
//	double zEnd = internal3DConfig_->zEnd();
//	int xCount = int((xEnd-xStart)/xStep);
//	int yCount = int((yEnd-yStart)/yStep);
//	int zCount = int((zEnd-zStart)/zStep);

//	if ((xEnd-xStart-(xCount+0.01)*xStep) < 0)
//		xCount += 1;
//	else
//		xCount += 2;

//	if ((yEnd-yStart-(yCount+0.01)*yStep) < 0)
//		yCount += 1;
//	else
//		yCount += 2;

//	if ((zEnd-zStart-(zCount+0.01)*zStep) < 0)
//		zCount += 1;
//	else
//		zCount += 2;

//	scan_->rawData()->beginInsertRows(xCount, -1);

//	for (int z = 0; z < zCount; z++){

//		for (int y = 0; y < yCount; y++){

//			for (int x = 0; x < xCount; x++){

//				insertIndex = AMnDIndex(x, y, z);
//				qDebug() << "Index:" << insertIndex.i() << insertIndex.j() << insertIndex.k() << "Values:" << xStart+x*xStep << yStart+y*yStep << zStart+z*zStep;
//				scan_->rawData()->setAxisValue(0, insertIndex.i(), xStart + x*xStep);
//				scan_->rawData()->setAxisValue(1, insertIndex.j(), yStart + y*yStep);
//				scan_->rawData()->setAxisValue(2, insertIndex.k(), zStart + z*zStep);

//				for (int di = 0; di < scan_->rawDataSourceCount(); di++){

//					if (scan_->rawDataSources()->at(di)->rank() == 0)
//						scan_->rawData()->setValue(insertIndex, di, AMnDIndex(), -1);

//					else if (scan_->rawDataSources()->at(di)->rank() == 1){

//						QVector<int> data = QVector<int>(scan_->rawDataSources()->at(di)->size(0), -1);
//						scan_->rawData()->setValue(insertIndex, di, data.constData());
//					}

//					else if (scan_->rawDataSources()->at(di)->rank() == 2){

//						QVector<int> data = QVector<int>(scan_->rawDataSources()->at(di)->size(1), -1);
//						scan_->rawData()->setValue(insertIndex, di, data.constData());
//					}

//					else if (scan_->rawDataSources()->at(di)->rank() == 3){

//						QVector<int> data = QVector<int>(scan_->rawDataSources()->at(di)->size(2), -1);
//						scan_->rawData()->setValue(insertIndex, di, data.constData());
//					}

//					else if (scan_->rawDataSources()->at(di)->rank() == 4){

//						QVector<int> data = QVector<int>(scan_->rawDataSources()->at(di)->size(3), -1);
//						scan_->rawData()->setValue(insertIndex, di, data.constData());
//					}
//				}
//			}
//		}
//	}

//	scan_->rawData()->endInsertRows();
//}

//bool AM3DDacqScanController::setConfigFile(const QString &filename)
//{
//	filename_ = filename;
//	return advAcq_->setConfigFile(filename);
//}

//bool AM3DDacqScanController::setScanAxesControl()
//{
//	if (xAxisPVName().isEmpty() || yAxisPVName().isEmpty() || zAxisPVName().isEmpty())
//		return false;

//	// Stage 1:  Save the current state of the config file.
//	QString filename = filename_;
//	filename = filename.left(filename.lastIndexOf("/"));
//	filename.append("/interim.cfg");
//	advAcq_->saveConfigFile(filename);

//	// Stage 2:  Setup the slow axis and fast axis strings.
//	QString firstPriorityAxis = getControlStringFromAxis(0);
//	QString secondPriorityAxis = getControlStringFromAxis(1);
//	QString thirdPriorityAxis = getControlStringFromAxis(2);

//	// Stage 3:  Load up the file through a text stream and change the controls for all three scan axes.
//	QFile inputFile(filename);
//	if(!inputFile.open(QIODevice::ReadOnly)) {
//		AMErrorMon::error(0, -1, "AM3DDacqScanController:  could not open the config file.");
//		return false;
//	}

//	QTextStream in(&inputFile);
//	QString current;
//	QStringList completeFile;

//	while (!(current = in.readLine()).startsWith("# Control")){

//		completeFile << current;
//	}

//	completeFile << thirdPriorityAxis;

//	while (!(current = in.readLine()).startsWith("# Control")){

//		completeFile << current;
//	}

//	completeFile << secondPriorityAxis;

//	while (!(current = in.readLine()).startsWith("# Control")){

//		completeFile << current;
//	}

//	completeFile << firstPriorityAxis;

//	while (!(current = in.readLine()).isNull()){

//		completeFile << current;
//	}

//	inputFile.close();

//	// Stage 4:  Save the file back as interim.cfg.
//	QFile outputFile(filename);
//	if(!outputFile.open(QIODevice::WriteOnly)) {
//		AMErrorMon::error(0, -1, "AM3DDacqScanController:  could not open the config file.");
//		return false;
//	}

//	QTextStream out(&outputFile);

//	for (int i = 0; i < completeFile.size(); i++)
//		out << completeFile.at(i) << "\n";

//	outputFile.close();
//	completeFile.clear();

//	// Stage 5:  Reload the config file.
//	return advAcq_->setConfigFile(filename);
//}

//QString AM3DDacqScanController::getControlStringFromAxis(int axis) const
//{
//	QString controlString = "";

//	// Generate the control string.
//	switch(axisPriorities_.at(axis)){

//	case 0:	// X

//		controlString = QString("# Control \"%1\" start:%2 delta: %3 final:%4 active: 7")
//				.arg(xAxisPVName())
//				.arg(internal3DConfig_->xStart())
//				.arg(internal3DConfig_->xStep())
//				.arg(internal3DConfig_->xEnd());
//		break;

//	case 1:	// Y

//		controlString = QString("# Control \"%1\" start:%2 delta: %3 final:%4 active: 7")
//				.arg(yAxisPVName())
//				.arg(internal3DConfig_->yStart())
//				.arg(internal3DConfig_->yStep())
//				.arg(internal3DConfig_->yEnd());
//		break;

//	case 2:	// Z

//		controlString = QString("# Control \"%1\" start:%2 delta: %3 final:%4 active: 7")
//				.arg(zAxisPVName())
//				.arg(internal3DConfig_->zStart())
//				.arg(internal3DConfig_->zStep())
//				.arg(internal3DConfig_->zEnd());
//		break;
//	}

//	return controlString;
//}
