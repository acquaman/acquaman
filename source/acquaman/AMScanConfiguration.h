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


#ifndef AM_SCANCONFIGURATION_H
#define AM_SCANCONFIGURATION_H

#include "dataman/database/AMDbObject.h"
#include "dataman/info/AMDetectorInfoSet.h"
#include "dataman/info/AMControlInfoList.h"

/// Forward declaration of AMScanController.  See note on circular coupling in AMScanConfiguration
class AMScanController;

/// Foward declaration of AMScanConfigurationView
class AMScanConfigurationView;

/// An AMScanConfiguration is the abstract parent class of all scan configurations.
/*!
  The AMScanConfiguration class acts as the abstract parent class from which all scan configuration classes descend.
  Algorithmically, data acquisition has two distinct phases: configuring a scan and running a scan.
  Although the distinction may be subtle it is a key observation. This can be noted directly as a user could configure a scan that they never decided to run.
  Furthermore, programatically, there is a separation between what information needs to be collected to specify the particular scan to run and the way in which the scan is actually executed.
  That is, with proper interfacing, two different scanning libraries could perform the same scan - and collect, essentially, the same data - by using the same scan configuration.

  Additionally, there will be many scan configuration implementation classes. Each technique, on each beamline will require a different implementation.
  For example, one might consider the SGM and REIXS beamlines, where both are capable of doing absorption spectroscopy (XAS) and emission spectroscopy (XES).
  Therefore, there could be four implementation classes: SGM's XAS Scan Configuration, SGM's XES Scan Configuration, REIXS' XAS Scan Configuration, and REIXS' XES Scan Configuration.  For each kind of scan configuration, there is an associated kind of scan controller, which takes care of actually executing the scan according to the configuration's parameters (when it comes time to do so).  For example, REIXSXESScanConfigurations are executed by a REIXSXESScanController.  (See AMScanController for details.)


  A scan configuration needs to record configuration values but not move the controls in anyway; therefore, a copy of the beamline object or any other control is not sufficient.

  As AMScanConfiguration is the base class from which all other scan configuration classes are derived, only information common to all scan configurations is kept here.
  This class inherits AMDbObject so that it can easily be recorded to the database (for example: to record the user's scan parameters associated with a scan).

  To implement a scan configuration object that works within the acquisition system, you need to provide at least two virtual functions:
  - createCopy() returns a pointer to a newly-created copy of this scan configuration.  (It takes the role of a copy constructor, but is virtual so that our high-level classes can copy a scan configuration without knowing exactly what kind it is.)
  - createController() returns a pointer to a newly-created AMScanController that is appropriate for executing this scan configuration.

\note On Circular Coupling and programming practice:
The createController() function introduces a circular coupling: Configurations create their Controller, and Controllers have a Configuration that they execute.

In theory, these relationships should normally be avoided.  However, at this point in time, the Configuration is the only class that has exact knowledge of its type (and hence knows what type of Controller needs to be created), so it actually introduces the least external coupling if we do it here.  In practice, this coupling is not a problem, and works well, because we always expect a one-to-one relationship between Controller and Configuration types.  Eventually this could be removed by a registration system that links configuration types to controller types.)
  */
class AMScanConfiguration : public AMDbObject
{
	Q_OBJECT

	Q_PROPERTY(AMDbObject* detectorConfigurations READ dbReadDetectorConfigurations WRITE dbLoadDetectorConfigurations)
	Q_PROPERTY(AMDbObject* axisControlInfos READ dbReadAxisControlInfos WRITE dbLoadAxisControlInfos)

	Q_CLASSINFO("AMDbObject_Attributes", "description=Generic Scan Configuration")

public:
	/// Default Constructor
	explicit AMScanConfiguration(QObject *parent = 0);
	/// Copy constructor.
	AMScanConfiguration(const AMScanConfiguration &original);
	/// Empty Destructor
	virtual ~AMScanConfiguration() {}

	virtual const QMetaObject* getMetaObject();

	/// A human-readable description of this scan configuration. Can be re-implemented to provide more details. Used by scan action to set the title for the action view.
	virtual QString description() const;

	/// A human-readable synopsis of this scan configuration. Can be re-implemented to provide more details. Used by scan action to set the main text in the action view.
	virtual QString detailedDescription() const;

	/// The auto-generated scan name. Can be re-implemented to customize for each scan type.
	virtual QString autoScanName() const;

	/// The user-defined name for this scan. If left blank, the auto-generated version will be used
	QString userScanName() const;

	/// The user-defined export name for this scan.
	QString userExportName() const;

	/// The scientific technique this configuration is for
	virtual QString technique() const;

	/// Returns whether or not this scan configuration expects to be automatically exported (true by default)
	bool autoExportEnabled() const;

	/// Returns whether or not this scan configuration can convert a property from an enum into a useful string. For use with the export systems.
	virtual bool canEnumConvert(const QString &enumName) const;

	/// Returns the useful string version for enum convertable properites. Will return [??] if the property cannot be converted by this scan configuration.
	virtual QString enumConvert(const QString &enumName, int enumValue) const;

	/// Returns the expected duration of the scan in seconds.
	double expectedDuration() const { return expectedDuration_; }

	// Virtual functions which must be re-implemented:
	/// Returns a pointer to a newly-created copy of this scan configuration.  (It takes the role of a copy constructor, but is virtual so that our high-level classes can copy a scan configuration without knowing exactly what kind it is.)
	virtual AMScanConfiguration* createCopy() const = 0;

	/// Returns a pointer to a newly-created AMScanController that is appropriate for executing this kind of scan configuration.  The controller should be initialized to use this scan configuration object as its scan configuration.  Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanController* createController() = 0;

	/// Creates a new copy of the default view. (The caller responsible for memory management). Returns 0 if no default view is provided.  Note that the newly-created view is expected to build itself to display, and take memory ownership, of this scan configuration instance.
	virtual AMScanConfigurationView* createView(){
		return 0;
	}

	/// Returns the set of detector infos that we wish to use in this scan
	AMDetectorInfoSet detectorConfigurations() const { return detectorConfigurations_; }
	/// Returns the list of control infos that we use to associate with the axes of the scan.
	AMControlInfoList axisControlInfos() const { return axisControlInfos_; }

	/// Returns a string with any warnings that occured during the load from database phase. Can be overridden by subclasses. Empty string implies no warnings.
	virtual QString dbLoadWarnings() const { return QString(); }

public slots:
	/// Sets the user-defined scan name. If set to an empty string, the auto-generated scan name will be used.
	void setUserScanName(const QString &userScanName);

	/// Sets the user-defined export name
	void setUserExportNmae(const QString &userExportName);

	/// Sets whether or not this scan configuration expects to be automatically exported
	void setAutoExportEnabled(bool autoExportEnabled);

	/// Sets the expected duration of the scan.  Input is expected in seconds.
	void setExpectedDuration(double duration);

	/// Sets the full list of detector infos
	void setDetectorConfigurations(const AMDetectorInfoSet &detectorConfigurations);
	/// Sets the full list for axis control infos.
	void setAxisControlInfos(const AMControlInfoList &axisControlInfos);

signals:
	/// General signal that something about the configuration has changed
	void configurationChanged();
	/// Signal that the user-defined scan name has changed
	void userScanNameChanged(const QString &userScanName);
	/// Signal that the user-defined export name has changed
	void userExportNameChanged(const QString &userExportName);
	/// Signal that the autoExport flag has been changed
	void autoExportEnabledChanged(bool autoExportEnabled);
	/// Notifier that the expected duration has changed.
	void expectedDurationChanged(double);
	/// Notifier that the detector configuration list has changed (the list has changed, not the internal states of the items)
	void detectorConfigurationsChanged();
	/// Notifier that the axis control info list has changed (the list has changed, not the internal states of the items within the list).
	void axisControlInfosChanged();

protected:
	/// Used to write the detector configurations to the database
	AMDbObject* dbReadDetectorConfigurations() { return &detectorConfigurations_;}
	/// For database loading (never called)
	void dbLoadDetectorConfigurations(AMDbObject*) {} //Never called, detectorConfigurations_ is always valid
	/// Used to write the axis control info's to the database.
	AMDbObject *dbReadAxisControlInfos() { return &axisControlInfos_; }
	/// For database loading (never called)
	void dbLoadAxisControlInfos(AMDbObject *) {} // Never called, axisControlInfos_ is always valid.

protected:
	/// A user-defined name for this scan. If left blank an auto-generated name will be used.
	QString userScanName_;
	/// A user-defined export name for this scan.
	QString userExportName_;

	/// Defines whether of not this configuration expects to be automatically exported when run (true by default)
	bool autoExportEnabled_;
	/// The expected duration.
	double expectedDuration_;

	/// A set of detector infos for this scan configuration (acts as both the detectors we wish to use and their configurations)
	AMDetectorInfoSet detectorConfigurations_;
	/// A set of control infos for this scan configuration (acts as the axis list).
	AMControlInfoList axisControlInfos_;
};

#endif // AM_SCANCONFIGURATION_H
