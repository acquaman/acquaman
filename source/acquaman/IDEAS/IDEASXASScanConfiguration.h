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


#ifndef IDEASXASSCANCONFIGURATION_H
#define IDEASXASSCANCONFIGURATION_H

#include "acquaman/AMStepScanConfiguration.h"

class IDEASXASScanConfiguration : public AMStepScanConfiguration
{
	Q_OBJECT

	Q_PROPERTY(QString edge READ edge WRITE setEdge)
	Q_PROPERTY(double edgeEnergy READ energy WRITE setEnergy)
	Q_PROPERTY(bool useFixedTime READ useFixedTime WRITE setUseFixedTime)
	Q_PROPERTY(int numberOfScans READ numberOfScans WRITE setNumberOfScans)

	Q_PROPERTY(bool isXRFScan READ isXRFScan WRITE setIsXRFScan)
	Q_PROPERTY(bool isTransScan READ isTransScan WRITE setIsTransScan)
	Q_PROPERTY(bool useRef READ useRef WRITE setUseRef)

	Q_CLASSINFO("AMDbObject_Attributes", "description=IDEAS XAS Scan Configuration")

public:
	/// Constructor
	Q_INVOKABLE IDEASXASScanConfiguration(QObject *parent = 0);
	/// Copy Constructor
	IDEASXASScanConfiguration(const IDEASXASScanConfiguration &original);
	/// Destructor.
	virtual ~IDEASXASScanConfiguration();

	/// Returns a pointer to a newly-created copy of this scan configuration.  (It takes the role of a copy constructor, but is virtual so that our high-level classes can copy a scan configuration without knowing exactly what kind it is.)
	virtual AMScanConfiguration* createCopy() const;

	/// Returns a pointer to a newly-created AMScanController that is appropriate for executing this kind of scan configuration.  The controller should be initialized to use this scan configuration object as its scan configuration.  Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanController* createController();

	/// Returns a pointer to a newly-created AMScanConfigurationView that is appropriate for viewing and editing this kind of scan configuration. Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanConfigurationView* createView();

	/// A human-readable synopsis of this scan configuration. Can be re-implemented to proved more details. Used by scan action to set the main text in the action view.
	virtual QString detailedDescription() const;

	/// Returns the current total estimated time for a scan to complete.
	double totalTime() const { return totalTime_; }
	/// Overloaded.  Returns the current total estimated time but also specifies whether the time should be recomputed first.
	double totalTime(bool recompute) { if (recompute) computeTotalTimeImplementation(); return totalTime_; }
	/// Returns the current total points in the scan.
	double totalPoints() const { return totalPoints_; }
	/// Returns the starting energy for the scan.
	double minEnergy() const { return minEnergy_; }
	/// Returns the end energy for the scan.
	double maxEnergy() const { return maxEnergy_; }
	/// Returns the time offset.
	double timeOffset() const { return timeOffset_; }

	/// Sets the time offset used for estimating the scan time.
	void setTimeOffset(double offset) { timeOffset_ = offset; computeTotalTimeImplementation(); }

	// Scan configuration properties
	/////////////////////////

	/// Returns whether scan is collecting XRF data
	bool isXRFScan() const { return isXRFScan_; }
	/// Returns whether scan is collecting transmission data
	bool isTransScan() const { return isTransScan_; }
	/// Returns whether scan is measuring a reference sample during a transmission data
	bool useRef() const { return useRef_; }


	/// Returns the name of the current edge.
	QString edge() const { return edge_; }
	/// Returns the edge energy for the scan.
	double energy() const { return energy_; }

	/// Returns whether the scan should use fixed or variable integration time.  The default is to use the variable integration time.
	bool useFixedTime() const { return useFixedTime_; }
	/// Returns the number of times this scan will be run.
	int numberOfScans() const { return numberOfScans_; }

	/// Get a nice looking string that contains all the standard information in an XAS scan.   Used when exporting.
	QString headerText() const;

signals:
	/// Notifier that the edge name has changed.
	void edgeChanged(QString);
	/// Notifier that the edge energy has changed.
	void energyChanged(double);
	/// Notifier about whether the scan should use fixed or variabled integration time.
	void useFixedTimeChanged(bool);
	/// Notifier that the total time estimate has changed.
	void totalTimeChanged(double);
	/// Notifier that the number of scans has changed.
	void numberOfScansChanged(int);

public slots:
	/// Sets whether the analysis blocks for XRF scans are automatically generated.
	void setIsXRFScan(bool isXRFScan);
	/// Sets whether this is a transmission scan or not.  Builds appropriate analysis blocks.
	void setIsTransScan(bool isTransScan);
	/// Sets whether this scan is using a reference or not.  Builds appropriate analysis blocks.
	void setUseRef(bool useRef);

	/// Sets the current edge for the scan.
	void setEdge(QString edgeName);
	/// Sets the edge energy.
	void setEnergy(double edgeEnergy);

	/// Sets whether the scan should use fixed or variable integration time for EXAFS.
	void setUseFixedTime(bool fixed);
	/// Sets the number of times this scan should be repeated.
	void setNumberOfScans(int num);


protected slots:
	/// Computes the total time any time the regions list changes.
	void computeTotalTime() { computeTotalTimeImplementation(); }
	/// Helper slot that connects the new region to the computeTotalTime slot.
	void onRegionAdded(AMScanAxisRegion *region);
	/// Helper slot that disconnects the region from the computTotalTime slot.
	void onRegionRemoved(AMScanAxisRegion *region);

protected:
	/// Computes the total estimated time for the scan.
	virtual void computeTotalTimeImplementation();

	/// The edge being scanned.
	QString edge_;
	/// The edge energy for the scan.
	double energy_;
	/// Holds whether the EXAFS scan will use fixed or variable integration time.
	bool useFixedTime_;
	/// Holds the number of times this scan should be repeated.
	int numberOfScans_;
	/// Holds the offset per point of extra time when doing a scan.
	double timeOffset_;
	/// Holds the total time in seconds that the scan is estimated to take.
	double totalTime_;
	/// Holds the total number of points in the scan.
	double totalPoints_;
	/// Holds the starting energy for the scan.
	double minEnergy_;
	/// Holds the finishing energy for the scan.
	double maxEnergy_;
	/// Flag used for determining if XRF related analysis blocks need to be created.
	bool isXRFScan_;
	/// Flag used for determining if transmission scan related analysis blocks need to be created.
	bool isTransScan_;
	/// Flag used for determining if reference sample related analysis blocks need to be created.
	bool useRef_;
};

#endif // IDEASXASSCANCONFIGURATION_H
