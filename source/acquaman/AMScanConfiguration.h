/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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


#ifndef ACQMAN_SCANCONFIGURATION_H
#define ACQMAN_SCANCONFIGURATION_H

#include "dataman/AMDbObject.h"

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

	Q_CLASSINFO("AMDbObject_Attributes", "description=Generic Scan Configuration")

public:
	/// Default Constructor
	explicit AMScanConfiguration(QObject *parent = 0);

	/// Empty Destructor
	virtual ~AMScanConfiguration() {}

	/// A human-readable description of this scan configuration. Can be re-implemented to provide more details. Used by AMBeamlineScanAction to set the title for the action view.
	virtual QString description() const {
		return "Generic Scan";
	}

	/// A human-readable synopsis of this scan configuration. Can be re-implemented to provide more details. Used by AMBeamlineScanAction to set the main text in the action view.
	virtual QString detailedDescription() const{
		return "Generic Scan Details";
	}

	/// The auto-generated scan name. Can be re-implemented to customize for each scan type.
	virtual QString autoScanName() const{
		return "Generic Scan";
	}

	/// The user-defined name for this scan. If left blank, the auto-generated version will be used
	QString userScanName() const{
		return userScanName_;
	}

	// Virtual functions which must be re-implemented:
	/// Returns a pointer to a newly-created copy of this scan configuration.  (It takes the role of a copy constructor, but is virtual so that our high-level classes can copy a scan configuration without knowing exactly what kind it is.)
	virtual AMScanConfiguration* createCopy() const = 0;

	/// Returns a pointer to a newly-created AMScanController that is appropriate for executing this kind of scan configuration.  The controller should be initialized to use this scan configuration object as its scan configuration.  Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanController* createController() = 0;

	/// Returns a new copy of the default view (caller responsible for memory). Returns 0 (NULL) if no default view.
	virtual AMScanConfigurationView* createView(){
		return 0; //NULL
	}

public slots:
	/// Sets the user-defined scan name. If set to an empty string, the auto-generated scan name will be used.
	void setUserScanName(const QString &userScanName){
		if(userScanName_ != userScanName){
			userScanName_ = userScanName;
			emit userScanNameChanged(userScanName_);
		}
	}

signals:
	/// General signal that something about the configuration has changed
	void configurationChanged();
	/// Signal that the user-defined scan name has changed
	void userScanNameChanged(const QString &userScanName);

protected:
	/// A user-defined name for this scan. If left blank an auto-generated name will be used.
	QString userScanName_;
};

#endif // ACQMAN_SCANCONFIGURATION_H
