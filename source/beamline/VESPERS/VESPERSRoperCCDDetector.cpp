/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "VESPERSRoperCCDDetector.h"

VESPERSRoperCCDDetector::VESPERSRoperCCDDetector(const QString &name, const QString &description, QObject *parent)
	: VESPERSCCDDetector(name, description, "IOC1607-003:det1", AMnDIndex(2084, 2084), parent)
{
}

#include <QTime>
void VESPERSRoperCCDDetector::loadImageFromFileImplementation(const QString &filename)
{
	Q_UNUSED(filename);

//	QFile file(fileName);
	QFile file("/mnt/aurora/ccd-calib/mar2013/si50.SPE");

	if (!file.open(QFile::ReadOnly))
		return;

	quint16 value;
	QDataStream in(&file);
	in.setByteOrder(QDataStream::LittleEndian);
	in.skipRawData(4100);

	int xSize = size().i();
	int ySize = size().j();

	// Loads the vector such that the image would appear correct using simple indexing.
	// If simple indexing provided a pixel at (x, y) then the correct image would be at
	// (y, x).
	QTime time;
	time.start();
	for (int i = 0, iSize = imageData_.size(); i < iSize; i++){

		in >> (quint16 &)value;
		imageData_[i/xSize + (i%xSize)*ySize] = int(value);
	}
	qDebug() << "Time to load is" << time.elapsed() << "ms";
	file.close();

	emit imageReady();
}
