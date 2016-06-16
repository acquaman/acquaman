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


#ifndef AM_SCANCONFIGURATION_H
#define AM_SCANCONFIGURATION_H

#include "dataman/database/AMDbObject.h"
#include "dataman/info/AMControlInfo.h"
#include "dataman/info/AMDetectorInfoSet.h"
#include "dataman/info/AMControlInfoList.h"

/// Forward declaration of AMScanController.  See note on circular coupling in AMScanConfiguration
class AMScanController;

/// Foward declaration of AMScanConfigurationView
class AMScanConfigurationView;

/// An AMScanConfiguration is the abstract parent class of all scan configurations.
/*!
  The AMScanConfiguration class acts as the abstract parent class from which all
  scan configuration classes descend.

  Algorithmically, data acquisition has two distinct phases: configuring a scan
  and running a scan. Although the distinction may be subtle it is a key observation.
  This can be noted directly as a user could configure a scan that they never decided
  to run. Furthermore, programatically, there is a separation between what information
  needs to be collected to specify the particular scan to run and the way in which
  the scan is actually executed. That is, with proper interfacing, two different
  scanning libraries could perform the same scan - and collect, essentially, the
  same data - by using the same scan configuration.

  Additionally, there will be many scan configuration implementation classes. Each
  technique, on each beamline will require a different implementation. For example,
  one might consider the SGM and REIXS beamlines, where both are capable of doing
  absorption spectroscopy (XAS) and emission spectroscopy (XES). Therefore, there
  could be four implementation classes: SGM's XAS Scan Configuration, SGM's XES
  Scan Configuration, REIXS' XAS Scan Configuration, and REIXS' XES Scan Configuration.
  For each kind of scan configuration, there is an associated kind of scan controller,
  which takes care of actually executing the scan according to the configuration's
  parameters (when it comes time to do so).  For example, REIXSXESScanConfigurations
  are executed by a REIXSXESScanController.  (See AMScanController for details.)


  A scan configuration needs to record configuration values but not move the controls
  in anyway; therefore, a copy of the beamline object or any other control is not
  sufficient.

  As AMScanConfiguration is the base class from which all other scan configuration
  classes are derived, only information common to all scan configurations is kept
  here. This class inherits AMDbObject so that it can easily be recorded to the
  database (for example: to record the user's scan parameters associated with a
  scan).

  To implement a scan configuration object that works within the acquisition system,
  you need to provide at least two virtual functions:
  - createCopy() returns a pointer to a newly-created copy of this scan configuration.
	(It takes the role of a copy constructor, but is virtual so that our high-level
	classes can copy a scan configuration without knowing exactly what kind it is.)
  - createController() returns a pointer to a newly-created AMScanController that
	is appropriate for executing this scan configuration.

NOTE: On Circular Coupling and programming practice:
The createController() function introduces a circular coupling: Configurations create
their Controller, and Controllers have a Configuration that they execute. In theory,
these relationships should normally be avoided.  However, at this point in time,
the Configuration is the only class that has exact knowledge of its type (and hence
knows what type of Controller needs to be created), so it actually introduces the
least external coupling if we do it here.  In practice, this coupling is not a problem,
and works well, because we always expect a one-to-one relationship between Controller
and Configuration types.  Eventually this could be removed by a registration system
that links configuration types to controller types.)
  */
class AMScanConfiguration : public AMDbObject
{
	Q_OBJECT

	Q_PROPERTY(AMDbObject* detectorConfigurations READ dbReadDetectorConfigurations WRITE dbLoadDetectorConfigurations)
	Q_PROPERTY(AMDbObject* axisControlInfos READ dbReadAxisControlInfos WRITE dbLoadAxisControlInfos)
	Q_PROPERTY(bool automaticDirectionAssessment READ automaticDirectionAssessment WRITE setAutomaticDirectionAssessment)
	Q_PROPERTY(int direction READ direction WRITE setDirection)

	Q_CLASSINFO("automaticDirectionAssessment", "upgradeDefault=false")
	Q_CLASSINFO("direction", "upgradeDefault=0")

	Q_CLASSINFO("AMDbObject_Attributes", "description=Generic Scan Configuration")

public:
	/// Enum that holds the direction a scan should move if it is specified manually.
	enum Direction {

		Increase = 0,
		Decrease
	};
	/*!
	  * Default base AMScanConfiguration contructor. Creates an instance of a scan
	  * configuration with a blank name, no expected duration which will auto export.
	  */
	explicit AMScanConfiguration(QObject *parent = 0);

	/*!
	  * Base AMScanConfiguration copy contructor. Performs deep copy of the original
	  * scan's name, export name, axis control infos, detector infos, expected duration
	  * and whether the scan is set to auto export.
	  */
	AMScanConfiguration(const AMScanConfiguration &original);

	/*!
	  * Base AMScanConfiguration empty destructor.
	  */
	virtual ~AMScanConfiguration() {}

	/*!
	  * The Qt MetaObject for the AMScanConfiguration.
	  */
	virtual const QMetaObject* getMetaObject();

	/*!
	  * A human-readable description of the scan configuarion. Can be re-implemented
	  * to provide more details. Used by scan actions to set the title for the action
	  * view.
	  */
	virtual QString description() const;

	/*!
	  * A more lengthy human-readable synopsis of the scan configuration. Can be
	  * re-implemented to provide more details. Used by scan actions to set the
	  * main text in the action view.
	  */
	virtual QString detailedDescription() const;

	/*!
	  * The auto-generated scan name. Can be re-implemented to customize for each
	  * scan type.
	  */
	virtual QString autoScanName() const;

	/*!
	  * The user-defined name for this scan. If left blank, the auto-generated version
	  * will be used.
	  */
	QString userScanName() const;

	/*!
	  * The user-defined export name for this scan.
	  */
	QString userExportName() const;

	/*!
	  * The scientific technique this configuration is for.
	  */
	virtual QString technique() const;

	/*!
	  * Returns whether this is an XAS technique scan
	  */
	virtual bool isXASScan() const;

	/*!
	  * Whether this scan configuration expects to be automatically exported.
	  * (default is true).
	  */
	bool autoExportEnabled() const;

	/*!
	  * Whether the scan configuration can convert a property from an enum type
	  * into a useful string. For use with the export system.
	  * \param enumName ~ A string version of the enum name, the conversion ability
	  * of which will be returned.
	  */
	virtual bool canEnumConvert(const QString &enumName) const;

	/*!
	  * The string representation of the provided enumValue, converted based on
	  * the provided enumName.
	  * \param enumName ~ A string version of the enum name to use for conversion
	  * of the enumValue.
	  * \param enumValue ~ The enum value which is to be converted to a string.
	  * \returns The passed enum value converted to a string representation if
	  * canEnumConvert(enumName) returns true && enumValue is a valid value for
	  * that enum, otherwise "[??]"
	  */
	virtual QString enumConvert(const QString &enumName, int enumValue) const;

	/*!
	  * The expected duration of the scan, in seconds.
	  */
	double expectedDuration() const { return expectedDuration_; }

	// Virtual functions which must be re-implemented:
	//////////////////////////////////////////////////

	/*!
	  * Produces a newly created copy of the scan configuration and returns a reference
	  * to it. Provides a virtual means of copying a class, such that high-level
	  * classes can copy a class without knowing what type it is.
	  */
	virtual AMScanConfiguration* createCopy() const = 0;

	/*!
	  * Creates an AMScanController that is appropriate for executing this type
	  * of scan configuration, and returns a reference to it.
	  * NOTE: The created controller should be configured to use this scan configuration
	  * as its configuration.
	  * NOTE2: Ownership of the created controller is the resposibility of the caller.
	  */
	virtual AMScanController* createController() = 0;

	/*!
	  * Creates a default view for this scan configuration, and returns a reference
	  * to it.
	  * NOTE: The created view is expected to take ownership of this configuration.
	  * NOTE2: Ownership of the created view is the reponsibility of the caller.
	  * \returns A default view appropriate for this scan configuration type, or
	  * 0 if no default view type can be found.
	  */
	virtual AMScanConfigurationView* createView(){
		return 0;
	}

	/*!
	  * The set of detector infos which will be used in the scan.
	  */
	AMDetectorInfoSet detectorConfigurations() const { return detectorConfigurations_; }

	/*!
	  * The list of control infos which are associated with the axes of the scan.
	  */
	AMControlInfoList axisControlInfos() const { return axisControlInfos_; }

	/// returns the control info of a given axis
	AMControlInfo axisControlInfoAt(int axis) const;

	/// The flag that determines whether a scan should figure out the direction it should go.
	bool automaticDirectionAssessment() const { return automaticDirectionAssessment_; }
	/// Returns the direction the scan should go if it does not automatically determine it itself.
	Direction direction() const { return direction_; }

	/*!
	  * A warning string containing any messages created whilst loading the scan
	  * configuration from the database.
	  * \returns A string description of any warning which occured during the loading
	  * of the scan configuration from the database, or an empty string if no warning
	  * occured.
	  */
	virtual QString dbLoadWarnings() const { return QString(); }

public slots:
	/*!
	  * Sets the user-defined scan name. If no user-defined scan name is provided
	  * the auto-generated scan name will be used.
	  * \param userScanName ~ The new user-defined scan name.
	  */
	void setUserScanName(const QString &userScanName);

	/*!
	  * Sets the user-defined export name.
	  * \param userExportName ~ The new user-defined export name.
	  */
	void setUserExportNmae(const QString &userExportName);

	/*!
	  * Sets whether the scan configuration should be automatically exported.
	  * \param autoExportEnabled ~ If true the scan configuration will expect to
	  * be auto-exported, otherwise it will expect not to be auto-exported.
	  */
	void setAutoExportEnabled(bool autoExportEnabled);

	/*!
	  * Sets the expected duration of the scan, in seconds.
	  * \param duration ~ The total number of seconds the scan is expected to take.
	  */
	void setExpectedDuration(double duration);

	/*!
	  * Sets the full list of detector infos.
	  * \param detectorConfigurations ~ The new full list of detector infos.
	  */
	void setDetectorConfigurations(const AMDetectorInfoSet &detectorConfigurations);

	/*!
	  * Sets the full list of control infos which are associated with the axes of
	  * the scan.
	  * \param axisControlInfos ~ The new full list of control infos to associate
	  * with the axes of the scan.
	  */
	void setAxisControlInfos(const AMControlInfoList &axisControlInfos);
	/// Sets the automatic direction assessment flag.
	void setAutomaticDirectionAssessment(bool isAutomatic);
	/// Sets the direction.
	void setDirection(Direction newDirection);

signals:
	/*!
	  * Signal indicating that some aspect of the configuration has been altered.
	  */
	void configurationChanged();

	/*!
	  * Signal indicating that the user-defined scan name has been altered.
	  * \param userScanName ~ The new value which the user-defined scan name has
	  * been set to.
	  */
	void userScanNameChanged(const QString &userScanName);

	/*!
	  * Signal indicating that the user-defined export name has been altered.
	  * \param userExportName ~ The new value which the user-defined export name
	  * has been set to.
	  */
	void userExportNameChanged(const QString &userExportName);

	/*!
	  * Signal indicating that the automatic export state of the configuration
	  * has been altered.
	  * \param autoExportEnabled ~ Whether the configuration now expects to be
	  * auto-exported.
	  */
	void autoExportEnabledChanged(bool autoExportEnabled);

	/*!
	  * Signal indicating that the expected duration has been altered.
	  * \param expectedDuration ~ The new expected duration of the scan.
	  */
	void expectedDurationChanged(double expectedDuration);

	/*!
	  * Signal indicating that the detector configuration list has been altered.
	  * NOTE: This indicates that the list has changed, not the internal states
	  * of the items.
	  */
	void detectorConfigurationsChanged();

	/*!
	  * Signal indicating that the list of axis controlls has been altered.
	  * NOTE: This indicates that the list has changed, not the internal states
	  * of the items.
	  */
	void axisControlInfosChanged();
	/// Notifier that an axis control has been changed.
	void axisControlInfoChanged();
	/// Notifier that an axis control has been added to the list of axis control infos.
	void axisControlInfoAdded();
	/// Notifier that an axis control has been removed from the list of axis control infos.
	void axisControlInfoRemoved();
	/// Notifier that the automatic direction assessment has changed.
	void automaticDirectionAssessmentChanged(bool);
	/// Notifier that the direction has changed.
	void directionChanged(AMScanConfiguration::Direction);

protected:
	/// Used to write the detector configurations to the database
	AMDbObject* dbReadDetectorConfigurations() { return &detectorConfigurations_;}
	/// For database loading (never called)
	void dbLoadDetectorConfigurations(AMDbObject*) {} //Never called, detectorConfigurations_ is always valid
	/// Used to write the axis control info's to the database.
	AMDbObject *dbReadAxisControlInfos() { return &axisControlInfos_; }
	/// For database loading (never called)
	void dbLoadAxisControlInfos(AMDbObject *) {} // Never called, axisControlInfos_ is always valid.
	/// Used for database loading.
	void setDirection(int newDirection);

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

	/// Flag that holds whether a scan can scan in any direction (allows the scan assembler to figure it out).
	bool automaticDirectionAssessment_;
	/// Flag that holds the required direction of scanning if automatic direction assessment is disabled.
	Direction direction_;
};

#endif // AM_SCANCONFIGURATION_H
