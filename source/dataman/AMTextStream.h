#ifndef AMTEXTSTREAM_H
#define AMTEXTSTREAM_H

#include <QTextStream>

#include "dataman/AMMeasurementInfo.h"

class AMTextStream : public QTextStream
{
public:
	AMTextStream();
	explicit AMTextStream(QIODevice *device);
	explicit AMTextStream(FILE *fileHandle, QIODevice::OpenMode openMode = QIODevice::ReadWrite);
	explicit AMTextStream(QString *string, QIODevice::OpenMode openMode = QIODevice::ReadWrite);
	explicit AMTextStream(QByteArray *array, QIODevice::OpenMode openMode = QIODevice::ReadWrite);
	explicit AMTextStream(const QByteArray &array, QIODevice::OpenMode openMode = QIODevice::ReadOnly);
	virtual ~AMTextStream();

	void write(const AMAxisInfo &axisInfo);
	void write(const AMMeasurementInfo &measurementInfo);

	/// If the axis info was not readable, then the passed in reference is left unchanged.
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
