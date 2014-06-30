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


#ifndef AMUSER_H
#define AMUSER_H

#include "dataman/database/AMDbObject.h"

class AMUser : public AMDbObject
{
Q_OBJECT
	Q_PROPERTY(int currentRunId READ currentRunId WRITE setCurrentRunId)
	Q_PROPERTY(QString lastExportDestination READ lastExportDestination WRITE setLastExportDestination)

public:

	/// Singleton class accessor method.  Use this the retrieve the application-wide instance of the user object
	static AMUser* user() {
		if(instance_ == 0)
			instance_ = new AMUser();
		return instance_;
	}

	/// Release and delete the singleton instance
	static void releaseUser() {
		if(instance_) {
			delete instance_;
			instance_ = 0;
		}
	}


	// User settings and meta-data
	/////////////////////////

	/// Returns the id of the run the user is currently working inside
	int currentRunId() const {
		return currentRunId_;
	}

	/// Returns the last folder that the user used as an export destination (Provided as default next time)
	QString lastExportDestination() const { return lastExportDestination_; }




signals:

public slots:
	/// Set the current run id. There is no error-checking or validity checking here, so please make sure it's a valid run.
	void setCurrentRunId(int id) {
		currentRunId_ = id;
		setModified(true);
	}


	/// Set the folder that the user last used as an export destination (Provided as default next time)
	void setLastExportDestination(const QString& exportDestinationFolder) {
		lastExportDestination_ = exportDestinationFolder;
		setModified(true);
	}

protected:

	/// Protected constructor. Access the singleton instance with AMUser::user().
 	virtual ~AMUser();
	explicit AMUser();

	/// singleton instance variable:
	static AMUser* instance_;

	/// the id of the run the user is currently working inside of
	int currentRunId_;
	/// The last folder that the user used as an export destination (Provided as default next time)
	QString lastExportDestination_;

};

#endif // AMUSER_H
