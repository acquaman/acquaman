#include "REIXSScanActionControllerMCPFileWriter.h"

#include <QTimer>
#include <QFile>
#include <QFileInfo>

#include "dataman/AMTextStream.h"

REIXSScanActionControllerMCPFileWriter::REIXSScanActionControllerMCPFileWriter(const QString &filePath, QObject *parent)
	: AMScanActionControllerBasicFileWriter(filePath, false, parent)
{
	dataPosition_ = -1;
}

REIXSScanActionControllerMCPFileWriter::~REIXSScanActionControllerMCPFileWriter()
{

}

void REIXSScanActionControllerMCPFileWriter::writeToFile(int fileRank, const QString &textToWrite)
{
	switch(fileRank){

	case 2:{

		AMTextStream dataStream(dataFile_);

		// Assumed that you'll add "End Info\n" to the header.
		if (textToWrite.contains("End Info")){

			dataStream << textToWrite;
			dataPosition_ = dataStream.pos();
		}

		else{

			dataStream.seek(dataPosition_);
			dataStream << textToWrite;
		}

		break;
	}

	default:
		break;
	}
}
