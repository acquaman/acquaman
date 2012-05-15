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


#ifndef XRFFREERUN_H
#define XRFFREERUN_H

#include <QObject>

#include "beamline/VESPERS/XRFDetector.h"
#include "beamline/VESPERS/VESPERSBeamline.h"
#include "util/VESPERS/XRFPeriodicTable.h"
#include "acquaman/VESPERS/VESPERSXRFScanConfiguration.h"

class XRFFreeRun : public QObject
{
	Q_OBJECT
public:
	/// Constructor.  Links the XRF periodic table model to the XRF detector.
	explicit XRFFreeRun(XRFDetector *detector, QObject *parent = 0);

	/// Returns the detector used in this configuration.
	XRFDetector *detector() const { return detector_; }
	/// Returns the XRF periodic table used by this configuration.
	XRFPeriodicTable *table() const { return xrfTable_; }
	/// Returns the XRF configuration that is part of this scan.
	VESPERSXRFScanConfiguration *configuration() const { return config_; }

signals:

public slots:
	/// Sets the detector.
	void setDetector(XRFDetector *detector) { detector_ = detector; }

protected slots:
	/// Handles what happens when the detector becomes connected.
	void onRoisHaveValues();
	/// Handles when the regions of interest change from an external source.
	void onExternalRegionsOfInterestChanged();

protected:
	/// Helper function that takes in a region of interest name and adds it to the XRFPeriodicTable.  Takes in the ROI name, finds the element and line it is associated with and adds it to the XRFPeriodicTable.
	void addRegionOfInterestToTable(QString name);

	/// The detector itself.  This has live beamline communications.
	XRFDetector *detector_;
	/// The periodic table that holds information about the regions of interest.
	XRFPeriodicTable *xrfTable_;
	/// The scan configuration.  Contains some configuration details about the scan such as integration time, minimum energy, maximum energy.
	VESPERSXRFScanConfiguration *config_;
};

#endif // XRFFREERUN_H
