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
 	virtual ~SGMSettings();
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
