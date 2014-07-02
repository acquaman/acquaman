/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


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
