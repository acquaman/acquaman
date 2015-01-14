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


#include "AMScanActionControllerBasicFileWriter.h"

#include <QTimer>
#include <QFile>
#include <QFileInfo>

#include "dataman/AMTextStream.h"

AMScanActionControllerBasicFileWriter::~AMScanActionControllerBasicFileWriter(){}

AMScanActionControllerBasicFileWriter::AMScanActionControllerBasicFileWriter(const QString &filePath, bool hasSpectraData, QObject *parent)
	: QObject(parent)
{
	filePath_ = filePath;
	hasSpectraData_ = hasSpectraData;

	dataFileSizeCheckPoint_ = 0;
	spectraFileSizeCheckPoint_ = 0;

	QFileInfo dataFileInfo(filePath+".dat");

	if(dataFileInfo.exists())
		emitError(AMScanActionControllerBasicFileWriter::AlreadyExistsError);

	dataFile_ = new QFile(dataFileInfo.filePath());

	if(!dataFile_->open(QIODevice::WriteOnly | QIODevice::Text))
		emitError(AMScanActionControllerBasicFileWriter::CouldNotOpenError);

	else
		QTimer::singleShot(0, this, SLOT(emitFileWriterIsBusy()));

	spectraFile_ = 0; //NULL

	if(hasSpectraData_){

		QFileInfo spectraFileInfo(filePath+"_spectra.dat");

		if(spectraFileInfo.exists())
			emitError(AMScanActionControllerBasicFileWriter::AlreadyExistsError);

		spectraFile_ = new QFile(spectraFileInfo.filePath());

		if(!spectraFile_->open(QIODevice::WriteOnly | QIODevice::Text))
			emitError(AMScanActionControllerBasicFileWriter::CouldNotOpenError);
	}

	// start the timer (2mins) to check whether we are writing to the files consistently
	QTimer *dataFileSizeCheckTimer = new QTimer(this);
	dataFileSizeCheckTimer->setInterval(120*1000); // 2 mins
	connect(dataFileSizeCheckTimer, SIGNAL(timeout()), this, SLOT(onDataFileSizeCheckerTimerTimeout()));
	dataFileSizeCheckTimer->start();
}

void AMScanActionControllerBasicFileWriter::writeToFile(int fileRank, const QString &textToWrite)
{
	switch(fileRank){

	case 0:{
		/* Stress testing
		QTime startTime = QTime::currentTime();
		AMTextStream rank1Stream(rank1File_);
		rank1Stream << textToWrite;
		sleep(2);
		QTime endTime = QTime::currentTime();
		qDebug() << "Stream (1) writing differential " << startTime.msecsTo(endTime) << " for size " << textToWrite.size();
		*/
		AMTextStream dataStream(dataFile_);
		dataStream << textToWrite;

		break;}

	case 1:{

		if(hasSpectraData_){
			/* Stress testing
			QTime startTime = QTime::currentTime();
			AMTextStream rank2Stream(rank2File_);
			rank2Stream << textToWrite;
			sleep(2);
			QTime endTime = QTime::currentTime();
			qDebug() << "Stream (2) writing differential " << startTime.msecsTo(endTime) << " for size " << textToWrite.size();
			*/
			AMTextStream spectraStream(spectraFile_);
			spectraStream << textToWrite;
		}

		break;}

	default:
		break;
	}
}

#include <QDebug>
void AMScanActionControllerBasicFileWriter::finishWriting()
{
	dataFile_->close();
	dataFile_->deleteLater();

	if(hasSpectraData_){
		spectraFile_->close();
		spectraFile_->deleteLater();
	}

	emit fileWriterIsBusy(false);
}

void AMScanActionControllerBasicFileWriter::onDataFileSizeCheckTimerTimeout()
{
	QFileInfo dataFileInfo(filePath_+".dat");
	if (dataFileInfo.size() == dataFileSizeCheckPoint_) {
		emit fileWriterError(FailedToWriteFile);
	}

	dataFileSizeCheckPoint_ = dataFileInfo.size();

	if (hasSpectraData_) {
		QFileInfo spectraDataFileInfo(filePath_+"_spectra.dat");
		if (spectraDataFileInfo.size() == spectraFileSizeCheckPoint_) {
			emit fileWriterError(FailedToWriteFile);
		}

		spectraFileSizeCheckPoint_ = spectraDataFileInfo.size();
	}
}

void AMScanActionControllerBasicFileWriter::emitError(AMScanActionControllerBasicFileWriter::FileWriterError error)
{
	errorsList_.append(error);
	QTimer::singleShot(0, this, SLOT(emitErrors()));
}

void AMScanActionControllerBasicFileWriter::emitErrors()
{
	while(!errorsList_.isEmpty())
		emit fileWriterError(errorsList_.takeFirst());
}

void AMScanActionControllerBasicFileWriter::emitFileWriterIsBusy()
{
	emit fileWriterIsBusy(true);
}

