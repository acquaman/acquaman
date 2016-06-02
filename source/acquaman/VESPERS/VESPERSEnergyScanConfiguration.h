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


#ifndef VESPERSENERGYSCANCONFIGURATION_H
#define VESPERSENERGYSCANCONFIGURATION_H

#include "acquaman/AMStepScanConfiguration.h"
#include "application/VESPERS/VESPERS.h"
#include "acquaman/VESPERS/VESPERSScanConfiguration.h"

/// This class is a VESPERS specific extension of a basic XAS scan configuration.
/*!
	Unlike a typical XAS scan, this scan configuration is not meant to reproduce
	scans in a XANES fashion.  It is meant to do crude energy scans over a large
	portion of the beamline's energy spectrum.  It is also primarily focused for
	use with the Roper CCD and (in the future) the MarCCD.  It is meant to automate
	the process of collecting a single image in a range such as:

		Start: 10 keV, Delta: 1 keV, End: 20 keV.

	Because of this, the fluorescence detectors are not a part of this scan type.
  */
class VESPERSEnergyScanConfiguration : public AMStepScanConfiguration, public VESPERSScanConfiguration
{
	Q_OBJECT

	Q_PROPERTY(AMDbObject* configurationDbObject READ dbReadScanConfigurationDbObject WRITE dbWriteScanConfigurationDbObject)
	Q_PROPERTY(bool goToPosition READ goToPosition WRITE setGoToPosition)
	Q_PROPERTY(double xPosition READ x WRITE setX)
	Q_PROPERTY(double yPosition READ y WRITE setY)
	Q_PROPERTY(QString header READ headerText WRITE setHeaderText)

	Q_CLASSINFO("AMDbObject_Attributes", "description=VESPERS Energy Scan Configuration")

public:
	/// Constructor.
	Q_INVOKABLE VESPERSEnergyScanConfiguration(QObject *parent = 0);
	/// Copy constructor.
	VESPERSEnergyScanConfiguration(const VESPERSEnergyScanConfiguration &original);
	/// Destructor.
	virtual ~VESPERSEnergyScanConfiguration();

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

	/// Returns the scan should move to a new position before starting the scan.
	bool goToPosition() const { return goToPosition_; }
	/// Returns the position that the scan should move to.
	QPointF position() const { return position_; }
	/// Returns the x coordinate of the scan position.
	double x() const { return position_.x(); }
	/// Returns the y coordinate of the scan position.
	double y() const { return position_.y(); }

	/// Get a nice looking string that contains all the standard information in an XAS scan.   Used when exporting.
	QString headerText() const;

signals:
	/// Notifier about whether the scan will move somewhere specific before scanning.
	void gotoPositionChanged(bool);
	/// Notifier that the x coordinate of the position has changed.
	void xPositionChanged(double);
	/// Notifier that the y coordinate of the position has changed.
	void yPositionChanged(double);
	/// Notifier that the total time estimate has changed.
	void totalTimeChanged(double);

public slots:
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

protected slots:
	/// Computes the total time any time the regions list changes.
	void computeTotalTime() { computeTotalTimeImplementation(); }
	/// Helper slot that connects the new region to the computeTotalTime slot.
	void onRegionAdded(AMScanAxisRegion *region);
	/// Helper slot that disconnects the region from the computTotalTime slot.
	void onRegionRemoved(AMScanAxisRegion *region);

protected:
	/// Method that does all the calculations for calculating the estimated scan time.
	virtual void computeTotalTimeImplementation();

	/// Bool used to determine if the scan should go to a new location or stay wherever the current position is.
	bool goToPosition_;
	/// The position that the scan should go to when goToPosition_ is true.  \note Implementation detail: this currently assumes we are using the pseudomotor sample stage.
	QPointF position_;
};

#endif // VESPERSENERGYSCANCONFIGURATION_H
