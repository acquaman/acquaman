#include "AMTextStream.h"

#include <QStringList>

AMTextStream::AMTextStream() :
	QTextStream()
{
	initializeTextStream();
}

AMTextStream::AMTextStream(QIODevice *device) :
	QTextStream(device)
{
	initializeTextStream();
}

AMTextStream::AMTextStream(FILE *fileHandle, QIODevice::OpenMode openMode) :
	QTextStream(fileHandle, openMode)
{
	initializeTextStream();
}

AMTextStream::AMTextStream(QString *string, QIODevice::OpenMode openMode) :
	QTextStream(string, openMode)
{
	initializeTextStream();
}

AMTextStream::AMTextStream(QByteArray *array, QIODevice::OpenMode openMode) :
	QTextStream(array, openMode)
{
	initializeTextStream();
}

AMTextStream::AMTextStream(const QByteArray &array, QIODevice::OpenMode openMode) :
	QTextStream(array, openMode)
{
	initializeTextStream();
}

void AMTextStream::write(const AMAxisInfo &axisInfo){
	QTextStream::operator <<(axisInfo.name);
	QTextStream::operator <<(separator_);
	QTextStream::operator <<(axisInfo.size);
	QTextStream::operator <<(separator_);
	QTextStream::operator <<(axisInfo.description);
	QTextStream::operator <<(separator_);
	QTextStream::operator <<(axisInfo.units);
	QTextStream::operator <<(separator_);
	QTextStream::operator <<(axisInfo.isUniform);
	QTextStream::operator <<(separator_);
	QTextStream::operator <<((double)axisInfo.start);
	QTextStream::operator <<(separator_);
	QTextStream::operator <<((double)axisInfo.increment);
	QTextStream::operator <<(terminator_);
}

void AMTextStream::write(const AMMeasurementInfo &measurementInfo){
	QTextStream::operator <<(measurementInfo.rank());
	QTextStream::operator <<(separator_);
	QTextStream::operator <<(measurementInfo.name);
	QTextStream::operator <<(separator_);
	QTextStream::operator <<(measurementInfo.description);
	QTextStream::operator <<(separator_);
	QTextStream::operator <<(measurementInfo.units);
	QTextStream::operator <<(objectMarker_);

	if(measurementInfo.rank() > 0){
		for(int x = 0; x < measurementInfo.rank(); x++){
			AMAxisInfo oneAxisInfo = measurementInfo.axes.at(x);
			write(oneAxisInfo);
		}
	}
	QTextStream::operator <<(terminator_);
}

void AMTextStream::read(AMAxisInfo &axisInfo){
	QString fullString;
	QStringList allStrings;
	QChar oneChar;

	while(!fullString.endsWith(terminator_)){
		QTextStream::operator >>(oneChar);
		fullString.append(oneChar);
	}

	fullString.remove(terminator_);
	allStrings = fullString.split(separator_);

	axisInfo.name = allStrings.at(0);
	axisInfo.size = allStrings.at(1).toLong();
	axisInfo.description = allStrings.at(2);
	axisInfo.units = allStrings.at(3);
	if(allStrings.at(4).toInt() == 1)
		axisInfo.isUniform = true;
	else
		axisInfo.isUniform = false;
	axisInfo.start = allStrings.at(5).toDouble();
	axisInfo.increment = allStrings.at(6).toDouble();
}

void AMTextStream::read(AMMeasurementInfo &measurementInfo){
	QString partialString;
	QStringList measurementInfoStrings;
	QChar oneChar;

	while(!partialString.endsWith(objectMarker_)){
		QTextStream::operator >>(oneChar);
		partialString.append(oneChar);
	}

	partialString.remove(objectMarker_);
	measurementInfoStrings = partialString.split(separator_);

	int rank = measurementInfoStrings.at(0).toInt();
	measurementInfo.name = measurementInfoStrings.at(1);
	measurementInfo.description = measurementInfoStrings.at(2);
	measurementInfo.units = measurementInfoStrings.at(3);

	QList<AMAxisInfo> axes;
	for(int x = 0; x < rank; x++){
		AMAxisInfo tempAxisInfo(QString(), 0);
		read(tempAxisInfo);
		axes << tempAxisInfo;
	}
	measurementInfo.axes = axes;

	partialString.clear();
	while(!partialString.endsWith(terminator_)){
		QTextStream::operator >>(oneChar);
		partialString.append(oneChar);
	}
}

void AMTextStream::initializeTextStream(){
	separator_ = "|%|%|";
	terminator_ = "|@|@|";
	objectMarker_ = "|#|#|";
}
