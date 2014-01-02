#include "AMRegionScanActionControllerBasicFileWriter.h"

#include <QTimer>
#include <QFile>
#include <QFileInfo>
#include "dataman/AMTextStream.h"

 AMRegionScanActionControllerBasicFileWriter::~AMRegionScanActionControllerBasicFileWriter(){}
AMRegionScanActionControllerBasicFileWriter::AMRegionScanActionControllerBasicFileWriter(const QString &filePath, bool hasSpectraData, QObject *parent)
	: QObject(parent)
{
	filePath_ = filePath;
	hasSpectraData_ = hasSpectraData;

	QFileInfo dataFileInfo(filePath+".dat");

	if(dataFileInfo.exists())
		emitError(AMRegionScanActionControllerBasicFileWriter::AlreadyExistsError);

	dataFile_ = new QFile(dataFileInfo.filePath());

	if(!dataFile_->open(QIODevice::WriteOnly | QIODevice::Text))
		emitError(AMRegionScanActionControllerBasicFileWriter::CouldNotOpenError);

	else
		QTimer::singleShot(0, this, SLOT(emitFileWriterIsBusy()));

	spectraFile_ = 0; //NULL

	if(hasSpectraData_){

		QFileInfo spectraFileInfo(filePath+"_spectra.dat");

		if(spectraFileInfo.exists())
			emitError(AMRegionScanActionControllerBasicFileWriter::AlreadyExistsError);

		spectraFile_ = new QFile(spectraFileInfo.filePath());

		if(!spectraFile_->open(QIODevice::WriteOnly | QIODevice::Text))
			emitError(AMRegionScanActionControllerBasicFileWriter::CouldNotOpenError);
	}
}

void AMRegionScanActionControllerBasicFileWriter::writeToFile(int fileRank, const QString &textToWrite)
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

void AMRegionScanActionControllerBasicFileWriter::finishWriting()
{
	dataFile_->close();

	if(hasSpectraData_)
		spectraFile_->close();

	emit fileWriterIsBusy(false);
}

void AMRegionScanActionControllerBasicFileWriter::emitError(AMRegionScanActionControllerBasicFileWriter::FileWriterError error)
{
	errorsList_.append(error);
	QTimer::singleShot(0, this, SLOT(emitErrors()));
}

void AMRegionScanActionControllerBasicFileWriter::emitErrors()
{
	while(!errorsList_.isEmpty())
		emit fileWriterError(errorsList_.takeFirst());
}

void AMRegionScanActionControllerBasicFileWriter::emitFileWriterIsBusy()
{
	emit fileWriterIsBusy(true);
}

