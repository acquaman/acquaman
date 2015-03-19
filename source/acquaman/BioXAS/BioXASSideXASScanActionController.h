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

#ifndef BIOXASXASSCANACTIONCONTROLLER_H
#define BIOXASXASSCANACTIONCONTROLLER_H

#include "acquaman/AMStepScanActionController.h"

class BioXASSideXASScanConfiguration;

class BioXASSideXASScanActionController : public AMStepScanActionController
{
	Q_OBJECT

public:
	/// Constructor takes a BioXASXAS scan configuration
	explicit BioXASSideXASScanActionController(BioXASSideXASScanConfiguration *configuration, QObject *parent = 0);

	/// Destructor
	virtual ~BioXASSideXASScanActionController();

signals:

public slots:

protected:
	/// Provides a string of beamline settings for the scan notes.
	QString beamlineSettings();
	/// Reimplemented to provide actions that will setupd the beamine for optimzed operation of the XAS scan.
	AMAction3* createInitializationActions();
	/// Reimplemented to put the beamline in a good state after a scan has been completed.
	AMAction3* createCleanupActions();

	/// Sets the scan axis and adds anything extra.
	virtual void buildScanControllerImplementation();
	/// Reimplemented for EXAFS capabilities.  Creates the scan assembler that builds all the actions used to run the scan.
	virtual void createScanAssembler();


protected:
	BioXASSideXASScanConfiguration *configuration_;

};

#endif // BIOXASSIDEXASSCANACTIONCONTROLLER_H
