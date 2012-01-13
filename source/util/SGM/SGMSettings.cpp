#include "SGMSettings.h"

#include <QMutexLocker>
#include <QReadLocker>
#include <QWriteLocker>

void SGMSettings::load(){
	QWriteLocker wl(&mutex_);

	// changing to NativeFormat; IniFormat is broken on Mac OS X Lion for SystemScope.  Filed Qt Bug: https://bugreports.qt.nokia.com/browse/QTBUG-21744
#ifdef Q_WS_MAC
	QSettings settings(QSettings::NativeFormat, QSettings::SystemScope, "SGMAcquaman", "SGMAcquaman");
#else
	QSettings settings(QSettings::IniFormat, QSettings::SystemScope, "SGMAcquaman", "SGMAcquaman");
#endif

	// All settings variables are loaded here from disk. Default values must be provided -- they will be used if the particular setting doesn't exist yet.
	// Don't forget to add here if you add new user options.

	// variable = settings.value(key, defaultValue).toType();

#ifdef Q_WS_MAC
	SGMDataFolder_ = settings.value("SGMDataFolder", "/Users/fawkes/Documents/CLS/SGM/ACQUAMANDATA").toString();
#else
	SGMDataFolder_ = settings.value("SGMDataFolder", "/home/sgm/beamline/programming").toString();
#endif
	SGMDatabaseFilename_ = settings.value("SGMDatabaseFilename", "sgmdata.db").toString();
}

void SGMSettings::save(){
	QReadLocker rl(&mutex_);

	// changing to NativeFormat; IniFormat is broken on Mac OS X Lion for SystemScope.  Filed Qt Bug: https://bugreports.qt.nokia.com/browse/QTBUG-21744
#ifdef Q_WS_MAC
	QSettings settings(QSettings::NativeFormat, QSettings::SystemScope, "SGMAcquaman", "SGMAcquaman");
#else
	QSettings settings(QSettings::IniFormat, QSettings::SystemScope, "SGMAcquaman", "SGMAcquaman");
#endif

	// All settings variables are saved here to the system-wide file.
	// Don't forget to add here if you add new system-wide options.

	settings.setValue("SGMDataFolder", SGMDataFolder_);
	settings.setValue("SGMDatabaseFilename", SGMDatabaseFilename_);
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
