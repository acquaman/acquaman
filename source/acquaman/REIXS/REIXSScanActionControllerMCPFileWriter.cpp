#include "REIXSScanActionControllerMCPFileWriter.h"

#include <QTimer>
#include <QFile>
#include <QFileInfo>

#include "dataman/AMTextStream.h"

REIXSScanActionControllerMCPFileWriter::REIXSScanActionControllerMCPFileWriter(const QString &filePath, QObject *parent)
	: AMScanActionControllerBasicFileWriter(filePath, false, parent)
{
	isWritingToFile = false;
}

REIXSScanActionControllerMCPFileWriter::~REIXSScanActionControllerMCPFileWriter()
{

}

void REIXSScanActionControllerMCPFileWriter::writeToFile(int fileRank, const QString &textToWrite)
{
	switch(fileRank){

	case 2:{
		if (!isWritingToFile)
		{
			isWritingToFile = true;
			AMTextStream dataStream(dataFile_);
			dataStream.seek(0);
			dataStream << textToWrite;
			isWritingToFile = false;
		}
		break;
	}

	default:
		break;
	}
}
