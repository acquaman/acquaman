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


#ifndef VESPERSTIMESCANCONFIGURATION_H
#define VESPERSTIMESCANCONFIGURATION_H

#include "acquaman/AMTimedRegionScanConfiguration.h"
#include "acquaman/VESPERS/VESPERSScanConfiguration.h"

class VESPERSTimeScanConfiguration : public AMTimedRegionScanConfiguration, public VESPERSScanConfiguration
{
	Q_OBJECT

	Q_PROPERTY(AMDbObject* configurationDbObject READ dbReadScanConfigurationDbObject WRITE dbWriteScanConfigurationDbObject)
	Q_PROPERTY(QString header READ headerText WRITE setHeaderText)

	Q_CLASSINFO("AMDbObject_Attributes", "description=VESPERS Timed Scan Configuration")

public:
	Q_INVOKABLE VESPERSTimeScanConfiguration(QObject *parent = 0);
	VESPERSTimeScanConfiguration(const VESPERSTimeScanConfiguration &original);
	virtual ~VESPERSTimeScanConfiguration(){}

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

signals:
	/// Notifier that the total time estimate has changed.
	void timePerAcquisitionChanged(double);
	/// Notifier that the total time estimate has changed.
	void totalTimeChanged(double);

public slots:
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
};

#endif // VESPERSTIMESCANCONFIGURATION_H
