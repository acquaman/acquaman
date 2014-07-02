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


#ifndef VESPERSPILATUSCCDDETECTOR_H
#define VESPERSPILATUSCCDDETECTOR_H

#include "beamline/VESPERS/VESPERSCCDDetector.h"

#include <QProcess>
/*!
	This class encapsulates the Pilatus detector on the VESPERS beamline.  For now it will only
	extend the base class.  However, since this detector has many features that we will likely
	use in the future, this class will likely be extended a lot more.
  */
class VESPERSPilatusCCDDetector : public VESPERSCCDDetector
{
	Q_OBJECT

public:
	/// Constructor.
 	virtual ~VESPERSPilatusCCDDetector();
	VESPERSPilatusCCDDetector(const QString &name, const QString &description, QObject *parent = 0);

	/// Returns the CLS Synchronized Dwell Time trigger PV string, which acts as the key for the synchronized dwell time lookup system
	virtual QString synchronizedDwellKey() const;

	/// Set the acquisition dwell time for triggered (RequestRead) detectors
	virtual bool setAcquisitionTime(double seconds);

	/// Copies the images from location \param path and copies them into "XRD Images".  \warning This might take a LONG time.  \note It might be worthwhile to make this it's own thread.
	bool copyImages(const QString &path);

	/// Returns the occupied space on aurora.
	double occupiedAuroraSize() const { return occupiedAuroraSize_; }
	/// Returns the total occupied size on aurora.
	double totalAuroraSize() const { return totalAuroraSize_; }

signals:
	/// Notifier that the detector recently updated the file system information for the capacity of the aurora drive.
	void fileSystemInfoUpdated();

protected slots:
	/// Queries the OS (linux only) for the file size of the aurora drive.  This is necessary because the images from copyImages will go on to aurora.
	void updateAuroraSize();

protected:
	/// Currently a castrated function because the Pilatus comes with good viewing software.
	virtual void loadImageFromFileImplementation(const QString &filename) { Q_UNUSED(filename); }

	/// The process that is used to call 'df'.  This is necessary so that it using the process doesn't block the event loop.
	QProcess *dfProcess_;
	/// Holds the occupied size of aurora.
	double occupiedAuroraSize_;
	/// Holds the total size of aurora.
	double totalAuroraSize_;
	/// The PV that holds the exposure period.  This needs to be set to 2.3 ms higher than the exposure time.
	AMPVControl *exposurePeriodControl_;

private slots:
	/// Method that reads out the data from the process.  This is a helper slot because we don't want to hold up the event loop.
	void readOutProcess(int exitCode, QProcess::ExitStatus exitStatus);
};

#endif // VESPERSPILATUSCCDDETECTOR_H
