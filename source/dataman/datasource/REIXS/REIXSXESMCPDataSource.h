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


#ifndef REIXSXESMCPDATASOURCE_H
#define REIXSXESMCPDATASOURCE_H

#include "dataman/datasource/AMDataSource.h"

class AMReadOnlyPVControl;
class AMSinglePVControl;

/// This class exposes an array process variable as an AMDataSource, for use with REIXSXESMCPDetector. The first (0) axis is the energy axis, and the second (1) axis is the vertical detector axis.
/*! \todo Should there be an abstract class above this one, that doesn't tie this to an EPICS PV implementation? */
class REIXSXESMCPDataSource : public QObject, public AMDataSource {
	Q_OBJECT
public:
	/// Constructor. \c name is the AMDataSource::name() for this source, and imagePV is the process variable containing the image data. We also need the PVs which describe the
	virtual ~REIXSXESMCPDataSource();
	REIXSXESMCPDataSource(const QString& name, AMReadOnlyPVControl* imagePV, AMReadOnlyPVControl* resolutionXPV, AMReadOnlyPVControl* resolutionYPV, QObject* parent = 0);

	/// Human-readable description of the type of data source this is (ex: "One-dimensional math expression").  Subclasses should re-implement this.
	virtual QString typeDescription() const { return "XES Imaging Detector"; }

	/// Returns an OR-combination of StateFlags describing the current state of the data. We return InvalidFlag before we're connected and ProcessingFlag afterwards (because the data might be changing.)
	virtual int state() const { return isConnected_ ? ProcessingFlag : InvalidFlag; }

	// Axis Information
	//////////////////////
	/// Returns axis information for all axes
	virtual QList<AMAxisInfo> axes() const { return axes_; }
	/// Returns the rank (number of dimensions) of this data set: 2 (image)
	virtual int rank() const { return 2; }
	/// Returns the size of (ie: count along) each dimension
	virtual AMnDIndex size() const { return AMnDIndex(pixelsX_, pixelsY_); }
	/// Returns the size along a single axis \c axisNumber. This should be fast. \c axisNumber is assumed to be between 0 and rank()-1.
	virtual int size(int axisNumber) const { return axes_.at(axisNumber).size; }
	/// Returns a bunch of information about a particular axis. \c axisNumber is assumed to be between 0 and rank()-1.
	virtual AMAxisInfo axisInfoAt(int axisNumber) const { return axes_.at(axisNumber); }
	/// Returns the id of an axis, by name. (By id, we mean the index of the axis. We called it id to avoid ambiguity with indexes <i>into</i> axes.) This could be slow, so users shouldn't call it repeatedly. Returns -1 if not found.
	virtual int idOfAxis(const QString& axisName) const;

	// Data value access
	////////////////////////////

	/// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are insuffient or any are out of range, or if the data is not ready.
	virtual AMNumber value(const AMnDIndex& indexes) const;

	virtual bool values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const;

	/// When the independent values along an axis is not simply the axis index, this returns the independent value along an axis (specified by axis number and index)
	virtual AMNumber axisValue(int axisNumber, int index) const;
	/// Performance optimization of axisValue():  instead of a single value, copies a block of values from \c startIndex to \c endIndex in \c outputValues.  The provided pointer must contain enough space for all the requested values.
    virtual bool axisValues(int axisNumber, int startIndex, int endIndex, double *outputValues) const;

protected slots:
	/// Called whenever the connection state of any PV changes. Emits valuesChanged(), sizeChanged(), and stateChanged() as required.
	void onConnectionStateChanged();
	/// Called when the image PV changes. emits valuesChanged().
	void onImageValuesChanged();

protected:
	/// Retains axis information
	QList<AMAxisInfo> axes_;
	/// PVs to access detector data
	AMReadOnlyPVControl* imageControl_, *resolutionXControl_, *resolutionYControl_;
	/// Connection state: true if all 3 PVs are connected
	bool isConnected_;
	/// Caches the size of the image, for speed. Same as axes_.at(0).size and axes_.at(1).size
	int pixelsX_, pixelsY_;
};

#endif // REIXSXESMCPDATASOURCE_H
