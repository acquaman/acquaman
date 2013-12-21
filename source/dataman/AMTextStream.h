#ifndef AMTEXTSTREAM_H
#define AMTEXTSTREAM_H

#include <QTextStream>

#include "dataman/AMMeasurementInfo.h"

class AMTextStream : public QTextStream
{
public:
 	virtual ~AMTextStream();
	AMTextStream();
	explicit AMTextStream(QIODevice *device);
	explicit AMTextStream(FILE *fileHandle, QIODevice::OpenMode openMode = QIODevice::ReadWrite);
	explicit AMTextStream(QString *string, QIODevice::OpenMode openMode = QIODevice::ReadWrite);
	explicit AMTextStream(QByteArray *array, QIODevice::OpenMode openMode = QIODevice::ReadWrite);
	explicit AMTextStream(const QByteArray &array, QIODevice::OpenMode openMode = QIODevice::ReadOnly);

	void write(const AMAxisInfo &axisInfo);
	void write(const AMMeasurementInfo &measurementInfo);

	void read(AMAxisInfo &axisInfo);
	void read(AMMeasurementInfo &measurementInfo);

protected:
	void initializeTextStream();

protected:
	QString separator_;
	QString terminator_;
	QString objectMarker_;
};

#endif // AMTEXTSTREAM_H
