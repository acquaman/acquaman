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


#include "SGMSettings.h"

#include <QMutexLocker>
#include <QReadLocker>
#include <QWriteLocker>

SGMSettings::~SGMSettings(){}

void SGMSettings::load(){
	QWriteLocker wl(&mutex_);

	// changing to NativeFormat, IniFormat is broken on Mac OS X Lion for SystemScope.  Filed Qt Bug: https://bugreports.qt.nokia.com/browse/QTBUG-21744
#ifdef Q_WS_MAC
	QSettings settings(QSettings::NativeFormat, QSettings::SystemScope, "SGMAcquaman", "SGMAcquaman");
#else
	QSettings settings(QSettings::IniFormat, QSettings::SystemScope, "SGMAcquaman", "SGMAcquaman");
#endif

	// All settings variables are loaded here from disk. Default values must be provided -- they will be used if the particular setting doesn't exist yet.
	// Don't forget to add here if you add new user options.

	// variable = settings.value(key, defaultValue).toType()

#ifdef Q_WS_MAC
	SGMDataFolder_ = settings.value("SGMDataFolder", "/Users/fawkes/Documents/CLS/SGM/ACQUAMANDATA").toString();
#else
	SGMDataFolder_ = settings.value("SGMDataFolder", "/home/sgm/beamline/programming").toString();
#endif
	SGMDatabaseFilename_ = settings.value("SGMDatabaseFilename", "sgmdata.db").toString();
	SGMPublicDatabaseFilename_ = settings.value("SGMPublicDatabaseFilename", "sgmpublic.db").toString();
}

void SGMSettings::save(){
	QReadLocker rl(&mutex_);

	// changing to NativeFormat, IniFormat is broken on Mac OS X Lion for SystemScope.  Filed Qt Bug: https://bugreports.qt.nokia.com/browse/QTBUG-21744
#ifdef Q_WS_MAC
	QSettings settings(QSettings::NativeFormat, QSettings::SystemScope, "SGMAcquaman", "SGMAcquaman");
#else
	QSettings settings(QSettings::IniFormat, QSettings::SystemScope, "SGMAcquaman", "SGMAcquaman");
#endif

	// All settings variables are saved here to the system-wide file.
	// Don't forget to add here if you add new system-wide options.

	settings.setValue("SGMDataFolder", SGMDataFolder_);
	settings.setValue("SGMDatabaseFilename", SGMDatabaseFilename_);
	settings.setValue("SGMPublicDatabaseFilename", SGMPublicDatabaseFilename_);
}


// Singleton variables:
QMutex SGMSettings::instanceMutex_(QMutex::Recursive);
SGMSettings *SGMSettings::instance_ = 0;

SGMSettings* SGMSettings::s()
{
	QMutexLocker ml(&instanceMutex_);

	if(!instance_){
		instance_ = new SGMSettings();
	}
	return instance_;
}

QString SGMSettings::SGMDataFolder() const{
	QReadLocker rl(&mutex_);
	return SGMDataFolder_;
}

QString SGMSettings::SGMDatabaseFilename() const{
	QReadLocker rl(&mutex_);
	return SGMDatabaseFilename_;
}

QString SGMSettings::SGMPublicDatabaseFilename() const{
	QReadLocker rl(&mutex_);
	return SGMPublicDatabaseFilename_;
}
