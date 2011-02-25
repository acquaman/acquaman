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
  At this time, only the file name and the file path for where to write the data are common to all scan configurations.  This class inherits AMDbObject so that it can easily be recorded to the database (for example: to record the user's scan parameters associated with a scan).

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

/// Holds the name of the file where data will eventually be saved.
Q_PROPERTY(QString fileName READ fileName WRITE setFileName)
/// Holds the path to the directory where data will eventually be saved.
Q_PROPERTY(QString filePath READ filePath WRITE setFilePath)

public:
	/// Default Constructor
	explicit AMScanConfiguration(QObject *parent = 0);

	/// Empty Destructor
	virtual ~AMScanConfiguration() {}

	/// Returns the name of the file to save raw data in
	QString fileName() const { return fileName_;}
	/// Returns the path to save the raw data file to
	QString filePath() const { return filePath_; }

	/// A human-readable description of this scan configuration. Can be re-implemented to provide more details.
	virtual QString description() const {
		return QString();
	}

	// Virtual functions which must be re-implemented:

	/// Returns a pointer to a newly-created copy of this scan configuration.  (It takes the role of a copy constructor, but is virtual so that our high-level classes can copy a scan configuration without knowing exactly what kind it is.)
	virtual AMScanConfiguration* createCopy() const = 0;

	/// Returns a pointer to a newly-created AMScanController that is appropriate for executing this kind of scan configuration.  The controller should be initialized to use this scan configuration object as its scan configuration.  Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanController* createController() = 0;


public slots:
	/// Sets the file name
	void setFileName(const QString &fileName) { fileName_ = fileName; setModified(true); }
	/// Sets the file path
	void setFilePath(const QString &filePath) { filePath_ = filePath; setModified(true); }


protected:
	/// File name
	QString fileName_;
	/// File path
	QString filePath_;

};

#endif // ACQMAN_SCANCONFIGURATION_H
