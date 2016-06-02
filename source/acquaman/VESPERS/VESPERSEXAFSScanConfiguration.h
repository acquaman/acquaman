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


#ifndef VESPERSEXAFSSCANCONFIGURATION_H
#define VESPERSEXAFSSCANCONFIGURATION_H

#include "acquaman/AMStepScanConfiguration.h"
#include "application/VESPERS/VESPERS.h"
#include "acquaman/VESPERS/VESPERSScanConfiguration.h"

/// This class is the VESPERS specific EXAFS scan configuration.
/*!
	This class extends the AMEXAFSScanConfiguration to configure any kind
	of XAS scan for the VESPERS beamline.  It is an energy scan that can
	be configured to run over just the edge of a given element (XANES) or it
	can be configured to run over the entire range of a given edge (EXAFS).
	Both types of scans have some subtle differences between how the scans are
	setup and these are accommodated.  You have the flexibility to choose which
	ion chamber you wish to use for I0 and It (for transmission mode) and also
	the choice of fluorescence detector that the scan will use if collecting
	the partial fluorescence yield.  It saves the edge, the energy, and a position
	that the scan will move to in the event there are different hot spots that
	you want to queue up.
  */

class VESPERSEXAFSScanConfiguration : public AMStepScanConfiguration, public VESPERSScanConfiguration
{
	Q_OBJECT

	Q_PROPERTY(AMDbObject* configurationDbObject READ dbReadScanConfigurationDbObject WRITE dbWriteScanConfigurationDbObject)
	Q_PROPERTY(QString edge READ edge WRITE setEdge)
	Q_PROPERTY(double edgeEnergy READ energy WRITE setEnergy)
	Q_PROPERTY(bool goToPosition READ goToPosition WRITE setGoToPosition)
	Q_PROPERTY(double xPosition READ x WRITE setX)
	Q_PROPERTY(double yPosition READ y WRITE setY)
	Q_PROPERTY(QString header READ headerText WRITE setHeaderText)
	Q_PROPERTY(bool useFixedTime READ useFixedTime WRITE setUseFixedTime)

	Q_CLASSINFO("useFixedTime", "upgradeDefault=false")

	Q_CLASSINFO("AMDbObject_Attributes", "description=VESPERS EXAFS Scan Configuration")

public:
	/// Constructor.
	Q_INVOKABLE VESPERSEXAFSScanConfiguration(QObject *parent = 0);
	/// Copy constructor.
	VESPERSEXAFSScanConfiguration(const VESPERSEXAFSScanConfiguration &original);
	/// Destructor.
	virtual ~VESPERSEXAFSScanConfiguration();

	/// Returns a pointer to a newly-created copy of this scan configuration.  (It takes the role of a copy constructor, but is virtual so that our high-level classes can copy a scan configuration without knowing exactly what kind it is.)
	virtual AMScanConfiguration* createCopy() const;

	/// Returns a pointer to a newly-created AMScanController that is appropriate for executing this kind of scan configuration.  The controller should be initialized to use this scan configuration object as its scan configuration.  Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanController* createController();

	/// Returns a pointer to a newly-created AMScanConfigurationView that is appropriate for viewing and editing this kind of scan configuration. Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanConfigurationView* createView();

	/// Returns the technique string.
	virtual QString technique() const;
	/// A human-readable description of this scan configuration. Can be re-implemented to provide more details. Used by scan action to set the title for the action view.
	virtual QString description() const;
	/// A human-readable synopsis of this scan configuration. Can be re-implemented to proved more details. Used by scan action to set the main text in the action view.
	virtual QString detailedDescription() const;
	/// Returns whether this is an XAS technique scan
	virtual bool isXASScan() const { return true; }

	/// Returns whether we are going to export the spectra data sources or not.
	bool exportSpectraSources() const { return exportSpectraSources_; }
	/// Returns whether we are exporting the spectra in rows or columns.
	bool exportSpectraInRows() const { return exportSpectraInRows_; }

	/// Returns the name of the current edge.
	QString edge() const { return edge_; }
	/// Returns the edge energy for the scan.
	double energy() const { return energy_; }

	/// Returns the scan should move to a new position before starting the scan.
	bool goToPosition() const { return goToPosition_; }
	/// Returns the position that the scan should move to.
	QPointF position() const { return position_; }
	/// Returns the x coordinate of the scan position.
	double x() const { return position_.x(); }
	/// Returns the y coordinate of the scan position.
	double y() const { return position_.y(); }

	/// Returns whether the scan should use fixed or variable integration time.  The default is to use the variable integration time.
	bool useFixedTime() const { return useFixedTime_; }

	/// Get a nice looking string that contains all the standard information in an XAS scan.   Used when exporting.
	QString headerText() const;

signals:
	/// Notifier that the edge name has changed.
	void edgeChanged(QString);
	/// Notifier that the edge energy has changed.
	void energyChanged(double);
	/// Notifier about whether the scan will move somewhere specific before scanning.
	void gotoPositionChanged(bool);
	/// Notifier that the x coordinate of the position has changed.
	void xPositionChanged(double);
	/// Notifier that the y coordinate of the position has changed.
	void yPositionChanged(double);
	/// Notifier about whether the scan should use fixed or variabled integration time.
	void useFixedTimeChanged(bool);
	/// Notifier that the total time estimate has changed.
	void totalTimeChanged(double);

public slots:
	/// Sets the current edge for the scan.
	void setEdge(QString edgeName);
	/// Sets the edge energy.
	void setEnergy(double edgeEnergy);

	/// Sets whether the scan should move to a new position before starting.
	void setGoToPosition(bool state);
	/// Sets the position the scan should move to before starting.
	void setPosition(const QPointF &pos);
	/// Overloaded.  Takes the x and y position explicitly.
	void setPosition(double xPos, double yPos) { setPosition(QPointF(xPos, yPos)); }
	/// Sets the x coordinate of the starting position of the scan.
	void setX(double xPos);
	/// Sets the y coordinate of the starting position of the scan.
	void setY(double yPos);

	/// Sets whether the scan should use fixed or variable integration time for EXAFS.
	void setUseFixedTime(bool fixed);

	/// Sets whether we export the scan with the spectra included or not.
	void setExportSpectraSources(bool exportSpectra);
	/// Sets whether we are exporting the spectra in rows or columns.
	void setExportSpectraInRows(bool exportInRows);

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
	/// Bool used to determine if the scan should go to a new location or stay wherever the current position is.
	bool goToPosition_;
	/// The position that the scan should go to when goToPosition_ is true.  \note Implementation detail: this currently assumes we are using the pseudomotor sample stage.
	QPointF position_;
	/// Flag holding whether we are exporting the spectra data sources or not.
	bool exportSpectraSources_;
	/// Flag holding whether we are exporting the spectra in rows or columns.
	bool exportSpectraInRows_;
};

#endif // VESPERSEXAFSSCANCONFIGURATION_H
