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


#include "AMFirstTimeController.h"

#include <QApplication>

#include <dataman/AMXASScan.h>
#include <dataman/AMRun.h>
#include <dataman/AMSample.h>
#include <dataman/AMExperiment.h>
#include <dataman/AMControlInfoList.h>
#include <dataman/AMSamplePlate.h>
#include <dataman/AMDetectorInfo.h>
#include "dataman/AMUser.h"
#include "dataman/AMXESScan.h"

#include "analysis/AM1DExpressionAB.h"
#include "analysis/AM2DSummingAB.h"

#include "dataman/AMDbObjectSupport.h"

#include <QDebug>

bool AMFirstTimeController::isFirstTime() {

	bool isFirstTime = false;

	// check for missing user settings:
	QSettings s(QSettings::IniFormat, QSettings::UserScope, "Acquaman", "Acquaman");
	if(!s.contains("userDataFolder")) {
		isFirstTime = true;
	}

	else {

		// check for existence of user data folder:
		QDir userDataDir(AMUserSettings::userDataFolder);
		if(!userDataDir.exists()) {
			isFirstTime = true;
		}

		// check for existence of database:
		QString filename = AMUserSettings::userDataFolder + AMUserSettings::userDatabaseFilename;
		QFile dbFile(filename);
		if(!dbFile.exists()) {
			isFirstTime = true;
		}
	}

	return isFirstTime;
}




bool AMFirstTimeController::onFirstTime() {

	AM::registerTypes();

	AMFirstTimeWizard ftw;

	// We're pretty forceful here... The user needs to accept this dialog.
	if(ftw.exec() != QDialog::Accepted)
		return false;

	AMUserSettings::userDataFolder = ftw.field("userDataFolder").toString();
	if(!AMUserSettings::userDataFolder.endsWith('/'))
		AMUserSettings::userDataFolder.append("/");

	AMUserSettings::save();

	// Attempt to create user's data folder, only if it doesn't exist:
	QDir userDataDir(AMUserSettings::userDataFolder);
	if(!userDataDir.exists()) {
		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, 0, "Creating new user data folder: "  + AMUserSettings::userDataFolder));
		if(!userDataDir.mkpath(AMUserSettings::userDataFolder)) {
			AMErrorMon::report(AMErrorReport(0, AMErrorReport::Serious, 0, "Could not create user data folder " + AMUserSettings::userDataFolder));
			return false;
		}
	}

	/// Find out the user's name:
	AMUser::user()->setName( ftw.field("userName").toString() );

	/*
	QString filename = AMUserSettings::userDataFolder + AMUserSettings::userDatabaseFilename;
	QFile dbFile(filename);
	if(!dbFile.exists())
		return databaseInitialization();
	else
		return true;
		*/

	bool success = databaseInitialization(true);

	if(success) {

		/// On first time only: create facilities. \todo This is app-specific... should probably be moved out of here.
		AMFacility blank("", "[Other Facility]", ":/128x128/contents.png");
		blank.storeToDb(AMDatabase::userdb());
		AMFacility als801("8.0.1", "Advanced Light Source Beamline 8.0.1", ":/alsIcon.png");
		als801.storeToDb(AMDatabase::userdb());
		AMFacility sgm("SGM", "Canadian Light Source SGM Beamline", ":/clsIcon.png");
		sgm.storeToDb(AMDatabase::userdb());

		/// Usability tweak: the first time users launch the program, we can assume they're going to do some acquisition, so let's create an initial run. (\todo This is SGM-specific... should be moved elsewhere)
		AMRun newRun("SGM", sgm.id());
		newRun.storeToDb(AMDatabase::userdb());
	}

	return success;

}

bool AMFirstTimeController::onEveryTime() {

	AM::registerTypes();
	return databaseInitialization(false);
}



/// create structures and tables for a new user database, from scratch
bool AMFirstTimeController::databaseInitialization(bool newUser) {

	AMDbObjectSupport::registerDatabase(AMDatabase::userdb());

	AMDbObjectSupport::registerClass<AMDbObject>();
	AMDbObjectSupport::registerClass<AMScan>();
	AMDbObjectSupport::registerClass<AMXASScan>();

	AMDbObjectSupport::registerClass<AMRun>();
	AMDbObjectSupport::registerClass<AMExperiment>();
	AMDbObjectSupport::registerClass<AMSample>();
	AMDbObjectSupport::registerClass<AMFacility>();

	AMDbObjectSupport::registerClass<AMRawDataSource>();
	AMDbObjectSupport::registerClass<AMAnalysisBlock>();
	AMDbObjectSupport::registerClass<AM1DExpressionAB>();
	AMDbObjectSupport::registerClass<AM2DSummingAB>();


	AMDbObjectSupport::registerClass<AMDetectorInfo>();
	AMDbObjectSupport::registerClass<AMSpectralOutputDetectorInfo>();
	AMDbObjectSupport::registerClass<AMControlInfo>();
	AMDbObjectSupport::registerClass<AMControlInfoList>();
	AMDbObjectSupport::registerClass<AMSamplePlate>();


	AMDbObjectSupport::registerClass<AMUser>();

	if(newUser)
		AMUser::user()->storeToDb(AMDatabase::userdb());	// insert the user into the database, if new.
	else
		AMUser::user()->loadFromDb(AMDatabase::userdb(), 1);// otherwise load existing user settings

	/// \bug Better error checking. Complicated because some calls could fail even though the process completes successfully. (ie: creating db table columns that already exist will fail)

	return true;
}


