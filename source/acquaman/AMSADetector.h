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


#ifndef AMSADETECTOR_H
#define AMSADETECTOR_H

#include <QObject>

#include "dataman/AMNumber.h"
#include "dataman/AMMeasurementInfo.h"

/// Prototype of a detector interface used with AMSA1DScanController.
/*! \todo More types: long-running acquisitions that update during acquisition; continuous acquisition
	\todo Merge with AMDetector?
  */
class AMSADetector : public QObject
{
    Q_OBJECT
public:
	enum Type { ImmediateRead, Triggered };

	explicit AMSADetector(const QString& name, const QString& description = QString(), QObject *parent = 0);
	virtual ~AMSADetector();

	/// Returns the type of the detector, which describes the process for getting it to acquire. The two possibilities for now are InstantRead (can be read immediately), and Triggered (where acquireNow() must be called to start the acquisition process.) Use setAcquisitionTime() to configure the dwell time for triggered detectors.
	virtual Type type() const = 0;

	/// Returns the name, description, rank, and size of the detector.
	const AMMeasurementInfo& measurementInfo() const { return measurementInfo_; }
	QString name() const { return measurementInfo_.name; }
	QString description() const { return measurementInfo_.description; }
	int rank() const { return measurementInfo_.rank(); }
	int size(int axisId) const { return measurementInfo_.size(axisId); }
	AMnDIndex size() const { return measurementInfo_.size(); }


	/// Call to prepare the detector for the beginning of a scan. Returns false immediately if the detector cannot be used now. Will emit initialized() later once complete (or failed).
	virtual bool init() = 0;

	/// Returns the acquisitionTime(); only relevant for a Triggered detector.
	virtual double acquisitionTime() const = 0;
	/// Set the acquisitionTime() for a triggered detector.
	virtual bool setAcquisitionTime(double seconds) = 0;

	/// Returns true if the detector is connected and ready to be init()ialized and used for acquisition.
	virtual bool isConnected() const = 0;

	/// Returns true if the process started by init() has run to completion.
	virtual bool initializationFinished() const = 0;
	/// Returns true if the process started by init() was successful.
	virtual bool initializationSucceeded() const = 0;

	/// Returns true if the last acquisition process (Started by calling acquireNow()) has run to completion.
	virtual bool lastAcquisitionFinished() const = 0;
	/// Returns true if the last acquisition (started by calling acquireNow()) was successful.
	virtual bool lastAcquisitionSucceeded() const = 0;

	/// Returns true if currently acquiring. This means that acquireNow() was called successfully, and acquisitionFinished() has not been emitted yet.
	virtual bool isAcquiring() const = 0;

	/// For triggered detectors, start the acquisition process. For InstantRead detectors, does nothing.
	virtual bool acquireNow() = 0;
	/// Abort the acquisition process. This should cause lastAcqiusitionFinished() to become true, but lastAcquisitionSucceeded() to be false.
	virtual void cancelAcquisition() = 0;

	/// Returns the value of the last acquisition, and \c index.  (Use a null AMnDIndex() for single-value detectors.)
	virtual AMNumber value(const AMnDIndex& index) const = 0;
	/// Returns what should be a valid pointer to the block of detector data, organized in row-major order (first axis varies the slowest).  We use this mechanism to quickly copy multi-dimensional data into the scan's data store.
	virtual const double* data() const = 0;

signals:
	/// Emit this when the connection state (isConnected()) changes.
	void connected(bool isConnected);
	/// Emit this when the init() process finishes (either successfully or unsuccessfully).
	void initialized(bool succeeded);
	/// Emit this when the acquireNow() process finishes (either successfully or unsuccessfully).
	void acquisitionFinished(bool succeeded);


signals:

public slots:
	/// Sets the name of this detector. Will be used to determine the name of the measurement in the data store; should be unique.
	void setName(const QString& name) { measurementInfo_.name = name; }
	/// Sets a human-readable description of the detector. Will become the description of the measurement in the data store.
	void setDescription(const QString& description) { measurementInfo_.description = description; }

protected:

	AMMeasurementInfo measurementInfo_;

};

#endif // AMSADETECTOR_H
