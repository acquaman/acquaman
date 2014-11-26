#include "REIXSScanActionControllerMCPFileWriter.h"

#include <QTimer>
#include <QFile>
#include <QFileInfo>

#include "dataman/AMTextStream.h"

REIXSScanActionControllerMCPFileWriter::REIXSScanActionControllerMCPFileWriter(const QString &filePath, QObject *parent)
	: AMScanActionControllerBasicFileWriter(filePath, false, parent)
{
	isWritingToFile_ = false;
}

REIXSScanActionControllerMCPFileWriter::~REIXSScanActionControllerMCPFileWriter()
{

}

void REIXSScanActionControllerMCPFileWriter::writeToFile(int fileRank, const QString &textToWrite)
{
	switch(fileRank){

	case 2:{
		if (!isWritingToFile_)
		{
			isWritingToFile_ = true;
			AMTextStream dataStream(dataFile_);
			dataStream.seek(0);
			dataStream << textToWrite;
			isWritingToFile_ = false;
		}
		break;
	}

	default:
		break;
	}
}
