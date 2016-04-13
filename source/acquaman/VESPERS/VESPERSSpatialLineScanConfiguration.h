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


#ifndef VESPERSSPATIALLINESCANCONFIGURATION_H
#define VESPERSSPATIALLINESCANCONFIGURATION_H

#include "acquaman/AMStepScanConfiguration.h"
#include "application/VESPERS/VESPERS.h"
#include "acquaman/VESPERS/VESPERSScanConfiguration.h"

/// This class is the VESPERS specific spatial line scan configuration.
/*!
	This class extends the AMRegionsScanConfiguration to configure a scan
	using only one motor to move the sample translationally.  Currently,
	there are four choices of motor to use:

		- horizontally using the H pseudo motor
		- horizontally using the x real motor
		- vertically using the V pseudo motor
		- vertically using the z real motor.

	It then operates in exactly the same way as a 2D map, define a start
	point and end point with an appropriate delta.  It generates a pixmap
	with the number of pixels = 1.  All of the rest of the parameters set
	are the same as a 2D map.  It also has the ability to take CCD images
	at the same time (identical to the 2D map).
  */

class VESPERSSpatialLineScanConfiguration : public AMStepScanConfiguration, public VESPERSScanConfiguration
{
	Q_OBJECT

	Q_PROPERTY(AMDbObject* configurationDbObject READ dbReadScanConfigurationDbObject WRITE dbWriteScanConfigurationDbObject)
	Q_PROPERTY(double start READ start WRITE setStart)
	Q_PROPERTY(double end READ end WRITE setEnd)
	Q_PROPERTY(double step READ step WRITE setStep)
	Q_PROPERTY(double time READ time WRITE setTime)
	Q_PROPERTY(double otherPosition READ otherPosition WRITE setOtherPosition)
	Q_PROPERTY(QString header READ headerText WRITE setHeaderText)

	Q_CLASSINFO("otherPosition", "upgradeDefault=-123456789.0")

	Q_CLASSINFO("AMDbObject_Attributes", "description=VESPERS Spatial Line Scan Configuration")

public:
	/// Constructor.
	Q_INVOKABLE VESPERSSpatialLineScanConfiguration(QObject *parent = 0);
	/// Copy constructor.
	VESPERSSpatialLineScanConfiguration(const VESPERSSpatialLineScanConfiguration &original);
	/// Destructor.
	virtual ~VESPERSSpatialLineScanConfiguration();

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

	/// Returns whether we are going to export the spectra data sources or not.
	bool exportSpectraSources() const { return exportSpectraSources_; }
	/// Returns whether we are exporting the spectra in rows or columns.
	bool exportSpectraInRows() const { return exportSpectraInRows_; }

	/// Get a nice looking string that contains all the standard information in an XAS scan.   Used when exporting.
	QString headerText() const;

	// Convience getters.
	/// Returns the start position for the region.
	double start() const { return scanAxisAt(0)->regionAt(0)->regionStart(); }
	/// Returns the end position for the region.
	double end() const { return scanAxisAt(0)->regionAt(0)->regionEnd(); }
	/// Returns the step size for the region.
	double step() const { return scanAxisAt(0)->regionAt(0)->regionStep(); }
	/// Returns the time for the region.
	double time() const { return scanAxisAt(0)->regionAt(0)->regionTime(); }
	/// Returns the other position.
	double otherPosition() const { return otherPosition_; }
	/// Returns whether the other position is valid or not.
	bool hasOtherPosition() const { return otherPosition_ != -123456789.0; }
	/// Returns the other motor that corresponds to the provided motor.  Ie: if \param motor is H the this returns V.
	VESPERS::Motors otherMotor(VESPERS::Motors motor) const;
	/// Returns a string of the other motor.
	QString otherMotorString(VESPERS::Motors motor) const;
	/// Returns whether the region is valid.
	bool validAxis() const { return scanAxisAt(0)->axisValid(); }

signals:
	/// Notifier that the total time estimate has changed.
	void totalTimeChanged(double);
	/// Notifier that the start position has changed.
	void startChanged(double);
	/// Notifier that the end position has changed.
	void endChanged(double);
	/// Notifier that the step size has changed.
	void stepChanged(double);
	/// Notifier that the time has changed.
	void timeChanged(double);
	/// Notifier that the other time has changed.
	void otherPositionChanged(double);

public slots:
	// Convience setters.
	/// Sets the start position.
	void setStart(double position);
	/// Sets the end position.
	void setEnd(double position);
	/// Sets the step size.
	void setStep(double position);
	/// Sets the time.
	void setTime(double position);
	/// Sets the other position.
	void setOtherPosition(double position);

	/// Sets whether we export the scan with the spectra included or not.
	void setExportSpectraSources(bool exportSpectra);
	/// Sets whether we are exporting the spectra in rows or columns.
	void setExportSpectraInRows(bool exportInRows);

protected slots:
	/// Computes the total time any time the regions list changes.
	void computeTotalTime() { computeTotalTimeImplementation(); }

protected:
	/// Method that does all the calculations for calculating the estimated scan time.
	virtual void computeTotalTimeImplementation();

	/// Flag holding whether we are exporting the spectra data sources or not.
	bool exportSpectraSources_;
	/// Flag holding whether we are exporting the spectra in rows or columns.
	bool exportSpectraInRows_;
	/// Variable that holds the position of the 'other' coordinate.  An example would be: if scanning the H motor, the other motor would be V.  Without this information the line scan can not be used in the workflow.
	double otherPosition_;
};

#endif // VESPERSSPATIALLINESCANCONFIGURATION_H
