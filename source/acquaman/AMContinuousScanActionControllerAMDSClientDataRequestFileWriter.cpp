#include "AMContinuousScanActionControllerAMDSClientDataRequestFileWriter.h"

#include <QFile>
#include <QFileInfo>
#include <QTimer>

#include "source/ClientRequest/AMDSClientDataRequest.h"
#include "source/ClientRequest/AMDSClientRequest.h"

AMContinuousScanActionControllerAMDSClientDataRequestFileWriter::AMContinuousScanActionControllerAMDSClientDataRequestFileWriter(const QString &filePath, QObject *parent) :
	QObject(parent)
{
	filePath_ = filePath;
	QFileInfo clientDataRequestFileInfo(filePath+".cdr");

	if(clientDataRequestFileInfo.exists())
		emitError(AMScanActionControllerBasicFileWriter::AlreadyExistsError);

	clientDataRequestFile_ = new QFile(clientDataRequestFileInfo.filePath());

	if(!clientDataRequestFile_->open(QIODevice::WriteOnly))
		emitError(AMScanActionControllerBasicFileWriter::CouldNotOpenError);

	else
		QTimer::singleShot(0, this, SLOT(emitFileWriterIsBusy()));
}

AMContinuousScanActionControllerAMDSClientDataRequestFileWriter::~AMContinuousScanActionControllerAMDSClientDataRequestFileWriter()
{

}

void AMContinuousScanActionControllerAMDSClientDataRequestFileWriter::writeToFile(const AMDSClientDataRequestMap &clientDataRequestMap)
{
	QDataStream dataStream(clientDataRequestFile_);

	quint16 numberOfRequests = clientDataRequestMap.count();
	dataStream << numberOfRequests;

	AMDSClientDataRequest *oneClientRequest;
	QMap<QString, AMDSClientDataRequest*>::const_iterator i = clientDataRequestMap.constBegin();
	while(i != clientDataRequestMap.constEnd()){
		oneClientRequest = i.value();
		AMDSClientRequest::encodeAndwriteClientRequest(&dataStream, oneClientRequest);
		i++;
	}

	QMap<QString, AMDSClientDataRequest*>::const_iterator j = clientDataRequestMap.constBegin();
	while(j != clientDataRequestMap.constEnd()){
		j.value()->deleteLater();
		j++;
	}

	finishWriting();
}

void AMContinuousScanActionControllerAMDSClientDataRequestFileWriter::finishWriting()
{
	clientDataRequestFile_->close();
	clientDataRequestFile_->deleteLater();

	emit fileWriterIsBusy(false);
}

void AMContinuousScanActionControllerAMDSClientDataRequestFileWriter::emitError(AMScanActionControllerBasicFileWriter::FileWriterError error)
{
	errorsList_.append(error);
	QTimer::singleShot(0, this, SLOT(emitErrors()));
}

void AMContinuousScanActionControllerAMDSClientDataRequestFileWriter::emitErrors()
{
	while(!errorsList_.isEmpty())
		emit fileWriterError(errorsList_.takeFirst());
}

void AMContinuousScanActionControllerAMDSClientDataRequestFileWriter::emitFileWriterIsBusy()
{
	emit fileWriterIsBusy(true);
}
