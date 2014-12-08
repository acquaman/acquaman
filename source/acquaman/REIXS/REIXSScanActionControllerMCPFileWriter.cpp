#include "REIXSScanActionControllerMCPFileWriter.h"

#include <QTimer>
#include <QFile>
#include <QFileInfo>

#include "dataman/AMTextStream.h"

REIXSScanActionControllerMCPFileWriter::REIXSScanActionControllerMCPFileWriter(const QString &filePath, QObject *parent)
	: AMScanActionControllerBasicFileWriter(filePath, false, parent)
{
	isWritingToFile_ = false;
	closeAfterWrite_ = false;
}

REIXSScanActionControllerMCPFileWriter::~REIXSScanActionControllerMCPFileWriter()
{

}

void REIXSScanActionControllerMCPFileWriter::writeToFile(int fileRank, const QString &textToWrite)
{
	switch(fileRank){

	case 2:{
		if (!isWritingToFile_ && dataFile_ && dataFile_->isOpen())
		{
			isWritingToFile_ = true;
			AMTextStream dataStream(dataFile_);
			dataStream.seek(0);
			dataStream << textToWrite;
			isWritingToFile_ = false;
		}
		if(!isWritingToFile_ && closeAfterWrite_)
			finishWriting();
		break;
	}

	default:
		break;
	}
}

void REIXSScanActionControllerMCPFileWriter::finishWriting()
{
	if(!isWritingToFile_){
		dataFile_->close();
		dataFile_->deleteLater();
		dataFile_ = 0;

		if(hasSpectraData_){
			spectraFile_->close();
			spectraFile_->deleteLater();
			spectraFile_ = 0;
		}

		emit fileWriterIsBusy(false);
	}
	else{
		closeAfterWrite_ = true;
	}
}
