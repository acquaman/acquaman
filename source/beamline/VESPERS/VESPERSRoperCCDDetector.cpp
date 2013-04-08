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
#include <QThreadPool>
#include <QMutexLocker>

VESPERSRoperCCDDetector::VESPERSRoperCCDDetector(const QString &name, const QString &description, QObject *parent)
	: VESPERSCCDDetector(name, description, "IOC1607-003:det1", AMnDIndex(2084, 2084), parent)
{
	qRegisterMetaType<QVector<int> >("QVector<int>");
}

void VESPERSRoperCCDDetector::loadImageFromFileImplementation(const QString &filename)
{
	VESPERSRoperQRunnableImageLoader *runner = new VESPERSRoperQRunnableImageLoader(filename, imageData_.size());
	runner->setAutoDelete(true);
	QThreadPool *threads = QThreadPool::globalInstance();
	connect(runner, SIGNAL(imageData(QVector<int>)), this, SLOT(onImageDataChanged(QVector<int>)));
	threads->start(runner);
}

void VESPERSRoperCCDDetector::onImageDataChanged(const QVector<int> &data)
{
	imageData_ = data;
	emit imageReady();
}

// VESPERSRoperQRunnableImageLoader
///////////////////////////////////////////////////////

VESPERSRoperQRunnableImageLoader::VESPERSRoperQRunnableImageLoader(const QString &fileName, int size, QObject *parent)
	: QObject(parent), QRunnable()
{
	fileName_ = fileName;
	size_ = size;
}

void VESPERSRoperQRunnableImageLoader::run()
{
	QMutexLocker lock(&locker_);

	int size = size_;
//	QString fileName = fileName_;
//	QFile file(fileName);
	QFile file("/mnt/aurora/ccd-calib/mar2013/si50.SPE");

	if (!file.open(QFile::ReadOnly))
		return;

	quint16 value;
	QDataStream in(&file);
	in.setByteOrder(QDataStream::LittleEndian);
	in.skipRawData(4100);

	QVector<int> data = QVector<int>(size);
	// Load the image.
	for (int i = 0, iSize = data.size(); i < iSize; i++){

		in >> (quint16 &)value;
		data[i] = int(value);
	}

	file.close();

	emit imageData(data);
}
