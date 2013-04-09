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
/*
<<<<<<< HEAD
	: VESPERSRoperCCDDetectorInfo(name, description, parent), AMOldDetector(name)
{
	temperatureControl_ = new AMPVControl("Temperature", "IOC1607-003:det1:Temperature_RBV", "IOC1607-003:det1:Temperature", QString(), this, 1);
	imageModeControl_ = new AMPVControl("Image Mode", "IOC1607-003:det1:ImageMode_RBV", "IOC1607-003:det1:ImageMode", QString(), this, 0.1);
	triggerModeControl_ = new AMPVControl("Trigger Mode", "IOC1607-003:det1:TriggerMode_RBV", "IOC1607-003:det1:TriggerMode", QString(), this, 0.1);
	operationControl_ = new AMSinglePVControl("Operation", "IOC1607-003:det1:Acquire", this, 0.1);
	stateControl_ = new AMReadOnlyPVControl("State", "IOC1607-003:det1:DetectorState_RBV", this);
	acquireTimeControl_ = new AMSinglePVControl("Acquire Time", "IOC1607-003:det1:AcquireTime", this, 0.1);
	autoSaveControl_ = new AMPVControl("AutoSave", "IOC1607-003:det1:AutoSave_RBV", "IOC1607-003:det1:AutoSave", QString(), this, 0.1);
	saveFileControl_ = new AMPVwStatusControl("Save File", "IOC1607-003:det1:WriteFile", "IOC1607-003:det1:WriteFile", "IOC1607-003:det1:WriteFile_RBV", QString(), this, 0.1);

	// Various CCD file path PVs.
	ccdPath_ = new AMProcessVariable("IOC1607-003:det1:FilePath", true, this);
	ccdFile_ = new AMProcessVariable("IOC1607-003:det1:FileName", true, this);
	ccdNumber_ = new AMProcessVariable("IOC1607-003:det1:FileNumber", true, this);

	connect(signalSource(), SIGNAL(connected(bool)), this, SIGNAL(connected(bool)));
	connect(temperatureControl_, SIGNAL(valueChanged(double)), this, SIGNAL(temperatureChanged(double)));
	connect(temperatureControl_, SIGNAL(setpointChanged(double)), this, SLOT(onTemperatureSetpointChanged(double)));
	connect(imageModeControl_, SIGNAL(valueChanged(double)), this, SLOT(onImageModeChanged()));
	connect(triggerModeControl_, SIGNAL(valueChanged(double)), this, SLOT(onTriggerModeChanged()));
	connect(operationControl_, SIGNAL(valueChanged(double)), this, SLOT(onIsAcquiringChanged()));
	connect(stateControl_, SIGNAL(valueChanged(double)), this, SLOT(onStateChanged()));
	connect(acquireTimeControl_, SIGNAL(valueChanged(double)), this, SLOT(onAcquireTimeChanged(double)));
	connect(autoSaveControl_, SIGNAL(valueChanged(double)), this, SLOT(onAutoSaveEnabledChanged()));
	connect(saveFileControl_, SIGNAL(valueChanged(double)), this, SLOT(onSaveFileStateChanged()));

	connect(ccdPath_, SIGNAL(valueChanged()), this, SLOT(onCCDPathChanged()));
	connect(ccdFile_, SIGNAL(valueChanged()), this, SLOT(onCCDNameChanged()));
	connect(ccdNumber_, SIGNAL(valueChanged(int)), this, SIGNAL(ccdNumberChanged(int)));
}

VESPERSRoperCCDDetector::ImageMode VESPERSRoperCCDDetector::imageMode() const
=======
*/
	: VESPERSCCDDetector(name, description, "IOC1607-003:det1", AMnDIndex(2084, 2084), parent)
//>>>>>>> master
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

/*
<<<<<<< HEAD
bool VESPERSRoperCCDDetector::setFromInfo(const AMOldDetectorInfo *info)
{
	const VESPERSRoperCCDDetectorInfo *detectorInfo = qobject_cast<const VESPERSRoperCCDDetectorInfo *>(info);

	// Check to make sure the detector info was valid.  If it isn't, then don't do anything to the detector.
	if (!detectorInfo)
		return false;

	setTemperature(detectorInfo->temperature());
	setAcquireTime(detectorInfo->acquireTime());

	return true;
}
=======
*/
// VESPERSRoperQRunnableImageLoader
///////////////////////////////////////////////////////
//>>>>>>> master

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
