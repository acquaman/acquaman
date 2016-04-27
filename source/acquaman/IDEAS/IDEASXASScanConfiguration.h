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
#include "application/IDEAS/IDEAS.h"
#include "acquaman/IDEAS/IDEASScanConfiguration.h"

class IDEASXASScanConfiguration : public AMStepScanConfiguration, public IDEASScanConfiguration
{
	Q_OBJECT

	Q_PROPERTY(AMDbObject* configurationDbObject READ dbReadScanConfigurationDbObject WRITE dbWriteScanConfigurationDbObject)
	Q_PROPERTY(QString edge READ edge WRITE setEdge)
	Q_PROPERTY(bool usingTransmission READ usingTransmission WRITE setUsingTransmission)
	Q_PROPERTY(bool usingReference READ usingReference WRITE setUsingReference)
	Q_PROPERTY(QString header READ headerText WRITE setHeaderText)

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

	/// Returns the technique string.
	virtual QString technique() const;
	/// A human-readable description of this scan configuration. Can be re-implemented to provide more details. Used by scan action to set the title for the action view.
	virtual QString description() const;
	/// A human-readable synopsis of this scan configuration. Can be re-implemented to proved more details. Used by scan action to set the main text in the action view.
	virtual QString detailedDescription() const;
	/// Returns whether this is an XAS technique scan
	virtual bool isXASScan() const { return true; }

	// Scan configuration properties
	/////////////////////////

	/// Returns the name of the current edge.
	QString edge() const { return edge_; }
	/// Returns whether scan is collecting transmission data
	bool usingTransmission() const { return usingTransmission_; }
	/// Returns whether scan is measuring a reference sample during a transmission data
	bool usingReference() const { return usingReference_; }

	/// Get a nice looking string that contains all the standard information in an XAS scan.   Used when exporting.
	QString headerText() const;

signals:
	/// Notifier that the edge name has changed.
	void edgeChanged(QString);
	/// Notifier that the total time estimate has changed.
	void totalTimeChanged(double);

public slots:
	/// Sets the current edge for the scan.
	void setEdge(QString edgeName);
	/// Sets whether this is a transmission scan or not.  Builds appropriate analysis blocks.
	void setUsingTransmission(bool usingTransmission);
	/// Sets whether this scan is using a reference or not.  Builds appropriate analysis blocks.
	void setUsingReference(bool usingReference);

protected slots:
	/// Helper slot that connects the new region to the computeTotalTime slot.
	void onRegionAdded(AMScanAxisRegion *region);
	/// Helper slot that disconnects the region from the computTotalTime slot.
	void onRegionRemoved(AMScanAxisRegion *region);
	/// Computes the total time any time the regions list changes.
	void computeTotalTime() { computeTotalTimeImplementation(); }

protected:
	/// Computes the total estimated time for the scan.
	virtual void computeTotalTimeImplementation();

	/// The edge being scanned.
	QString edge_;
	/// Flag used for determining if transmission scan related analysis blocks need to be created.
	bool usingTransmission_;
	/// Flag used for determining if reference sample related analysis blocks need to be created.
	bool usingReference_;
};

#endif // IDEASXASSCANCONFIGURATION_H
