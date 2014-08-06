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


#ifndef SGMXASSCANCONFIGURATION2013_H
#define SGMXASSCANCONFIGURATION2013_H

#include "acquaman/AMStepScanConfiguration.h"
#include "SGMScanConfiguration2013.h"

class SGMXASScanConfiguration2013 : public AMStepScanConfiguration, public SGMScanConfiguration2013
{
Q_OBJECT

Q_PROPERTY(AMDbObject* configurationDbObject READ dbReadScanConfigurationDbObject WRITE dbWriteScanConfigurationDbObject)

Q_CLASSINFO("AMDbObject_Attributes", "description=SGM XAS Scan Configuration 2013")

public:
	/// Constructor
	Q_INVOKABLE SGMXASScanConfiguration2013(QObject *parent = 0);
	/// Copy Constructor
	SGMXASScanConfiguration2013(const SGMXASScanConfiguration2013 &original);
	virtual ~SGMXASScanConfiguration2013();

	/// Returns a pointer to a newly-created copy of this scan configuration.  (It takes the role of a copy constructor, but is virtual so that our high-level classes can copy a scan configuration without knowing exactly what kind it is.)
	virtual AMScanConfiguration* createCopy() const;

	/// Returns a pointer to a newly-created AMScanController that is appropriate for executing this kind of scan configuration.  The controller should be initialized to use this scan configuration object as its scan configuration.  Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanController* createController();

	/// Returns a pointer to a newly-created AMScanConfigurationView that is appropriate for viewing and editing this kind of scan configuration. Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanConfigurationView* createView();

	/// A human-readable description of this scan configuration. Can be re-implemented to provide more details. Used by scan action to set the title for the action view.
	virtual QString description() const;

	/// A human-readable synopsis of this scan configuration. Can be re-implemented to provide more details. Used by scan action to set the main text in the action view.
	virtual QString detailedDescription() const;

	/// The auto-generated scan name. Can be re-implemented to customize for each scan type.
	virtual QString autoScanName() const;

	/// The scientific technique this configuration is for
	virtual QString technique() const;

	/// Synchronizes this configuration with the beamline by copying settins from the beamline, namely:
	/// Exit Slit Gap, Grating, Harmonic, Undulator Tracking, Grating Tracking, Exit Slit Tracking
	void getSettingsFromBeamline();

	/// Returns the lastest known beamline status
	bool matchesCurrentBeamline();

protected slots:
	/// Checks whether the current beamline settings match that of the configuration, namely:
	/// Exit Slit Gap, Grating, Harmonic, Undulator Tracking, Grating Tracking, Exit Slit Tracking
	/// if the matching status has changed emits matchingBeamlineStatusCahanged()
	void checkIfMatchesBeamline();

signals:
	/// Fired when a change to the configuration or beamline means that their status has changed from matched to not matched or vice versa, namely:
	/// Exit Slit Gap, Grating, Harmonic, Undulator Tracking, Grating Tracking, Exit Slit Tracking
	void matchingBeamlineStatusChanged(bool matchesBeamline);

protected:
	/// Whether or not the current XASScanConfiguration matches that of the beamline, namely:
	/// Exit Slit Gap, Grating, Harmonic, Undulator Tracking, Grating Tracking, Exit Slit Tracking
	bool matchesCurrentBeamline_;
	/// Can't use qFuzzyCompare because need to specify epsilon for exitSlitGap
	inline bool floatCompare(float x, float y, float epsilon);
};

#endif // SGMXASSCANCONFIGURATION2013_H
