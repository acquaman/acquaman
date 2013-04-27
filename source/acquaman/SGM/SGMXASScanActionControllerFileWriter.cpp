#include "SGMXASScanActionControllerFileWriter.h"

#include <QTimer>
#include <QFile>
#include <QFileInfo>
#include "dataman/AMTextStream.h"

SGMXASScanActionControllerFileWriter::SGMXASScanActionControllerFileWriter(const QString &filePath, bool hasRank2Data, QObject *parent) :
	QObject(parent)
{
	filePath_ = filePath;
	hasRank2Data_ = hasRank2Data;

	QFileInfo rank1FileInfo(filePath+".dat");
	if(rank1FileInfo.exists())
		emitError(SGMXASScanActionControllerFileWriter::AlreadyExistsError);

	rank1File_ = new QFile(rank1FileInfo.filePath());
	if(!rank1File_->open(QIODevice::WriteOnly | QIODevice::Text))
		emitError(SGMXASScanActionControllerFileWriter::CouldNotOpenError);
	else
		QTimer::singleShot(0, this, SLOT(emitFileWriterIsBusy()));

	rank2File_ = 0; //NULL
	if(hasRank2Data_){
		QFileInfo rank2FileInfo(filePath+"_spectra.dat");
		if(rank2FileInfo.exists())
			emitError(SGMXASScanActionControllerFileWriter::AlreadyExistsError);

		rank2File_ = new QFile(rank2FileInfo.filePath());
		if(!rank2File_->open(QIODevice::WriteOnly | QIODevice::Text))
			emitError(SGMXASScanActionControllerFileWriter::CouldNotOpenError);
	}
}

void SGMXASScanActionControllerFileWriter::writeToFile(int fileRank, const QString &textToWrite){
	switch(fileRank){
	case 1:{
		/* Stress testing
		QTime startTime = QTime::currentTime();
		AMTextStream rank1Stream(rank1File_);
		rank1Stream << textToWrite;
		sleep(2);
		QTime endTime = QTime::currentTime();
		qDebug() << "Stream (1) writing differential " << startTime.msecsTo(endTime) << " for size " << textToWrite.size();
		*/

		AMTextStream rank1Stream(rank1File_);
		rank1Stream << textToWrite;
		break;}
	case 2:{
		if(hasRank2Data_){
			/* Stress testing
			QTime startTime = QTime::currentTime();
			AMTextStream rank2Stream(rank2File_);
			rank2Stream << textToWrite;
			sleep(2);
			QTime endTime = QTime::currentTime();
			qDebug() << "Stream (2) writing differential " << startTime.msecsTo(endTime) << " for size " << textToWrite.size();
			*/

			AMTextStream rank2Stream(rank2File_);
			rank2Stream << textToWrite;
		}
		break;}
	default:
		break;
	}
}

void SGMXASScanActionControllerFileWriter::finishWriting(){
	rank1File_->close();
	if(hasRank2Data_)
		rank2File_->close();
	emit fileWriterIsBusy(false);
}

void SGMXASScanActionControllerFileWriter::emitError(SGMXASScanActionControllerFileWriter::FileWriterError error){
	errorsList_.append(error);
	QTimer::singleShot(0, this, SLOT(emitErrors()));
}

void SGMXASScanActionControllerFileWriter::emitErrors(){
	while(errorsList_.count() > 0)
		emit fileWriterError(errorsList_.takeFirst());
}

void SGMXASScanActionControllerFileWriter::emitFileWriterIsBusy(){
	emit fileWriterIsBusy(true);
}

