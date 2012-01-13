#ifndef SGMSETTINGS_H
#define SGMSETTINGS_H

#include <QSettings>
#include <QString>
#include <QReadWriteLock>
#include <QMutex>
#include <QMultiMap>

class SGMSettings
{
public:
	/// SGMSettings is a singleton class. Access the only instance of it using SGMSettings::s()
	static SGMSettings* s();


	/// SGM settings storage root folder
	QString SGMDataFolder() const;
	/// Name of SGM Beamline database
	QString SGMDatabaseFilename() const;

	/// Load settings from disk
	void load();
	/// Save settings to disk
	void save();

protected:
	/// This is a singleton class, so the constructor is protected
	SGMSettings() : mutex_(QReadWriteLock::Recursive) {}

	QString SGMDataFolder_;
	QString SGMDatabaseFilename_;

	// thread saftey
	mutable QReadWriteLock mutex_;

	// singleton instance
	static SGMSettings *instance_;
	static QMutex instanceMutex_;
};

#endif // SGMSETTINGS_H
