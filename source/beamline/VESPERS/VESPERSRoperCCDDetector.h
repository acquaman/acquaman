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


#ifndef VESPERSROPERCCDDETECTOR_H
#define VESPERSROPERCCDDETECTOR_H

#include "beamline/VESPERS/VESPERSCCDDetector.h"

class VESPERSRoperQRunnableImageLoader;

/*!
  This class encapsulates the Roper CCD used on VESPERS.  Although currently not available, it will have the current image set available for viewing.
  What it will offer at the moment is the ability to control the important aspects of the detector, such as the temperature, accumulationn time,
  starting, stopping, etc.
  */
class VESPERSRoperCCDDetector : public VESPERSCCDDetector
{
	Q_OBJECT

public:
	/// Constructor.
	VESPERSRoperCCDDetector(const QString &name, const QString &description, QObject *parent = 0);

protected slots:
	/// Sets the new image data after receiving the signal from the QRunnable.
	void onImageDataChanged(const QVector<int> &data);

protected:
	/// The implementation method used for loading images.  Implementations must emit the imageReady() signal after loading data.
	virtual void loadImageFromFileImplementation(const QString &filename);
};

#include <QRunnable>
#include <QMutex>

/// This is a speicific QRunnable class that reads in the file of the CCD images.  This allows the task to be run in its own thread using QThreadPool.
class VESPERSRoperQRunnableImageLoader : public QObject, public QRunnable
{
	Q_OBJECT

public:
	/// Constructor.  Takes in the file path of the image to be loaded as well as the necessary size for the data.
	VESPERSRoperQRunnableImageLoader(const QString &fileName, int size, QObject *parent = 0);

	/// The method that does the work.  Needs to use proper thread safety.
	virtual void run();

signals:
	/// Signal that passes the data that was loaded.
	void imageData(const QVector<int> &data);

protected:
	/// The file name with the full path.
	QString fileName_;
	/// The size of the data that should be expected.
	int size_;
	/// The mutex used for locking the run method.
	QMutex locker_;
};

#endif // VESPERSROPERCCDDETECTOR_H
