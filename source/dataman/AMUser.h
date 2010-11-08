#ifndef AMUSER_H
#define AMUSER_H

#include "dataman/AMDbObject.h"

class AMUser : public AMDbObject
{
Q_OBJECT
	Q_PROPERTY(int currentRunId READ currentRunId WRITE setCurrentRunId)

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


	// Convenient access methods for our meta-data:
	/////////////////////////

	/// Returns the id of the run the user is currently working inside
	int currentRunId() const {
		return currentRunId_;
	}



signals:

public slots:
	/// Set the current run id. There is no error-checking or validity checking here, so please make sure it's a valid run.
	void setCurrentRunId(int id) {
		currentRunId_ = id;
		setModified(true);
	}


protected:

	/// Protected constructor. Access the singleton instance with AMUser::user().
	explicit AMUser();

	/// singleton instance variable:
	static AMUser* instance_;

	/// the id of the run the user is currently working inside of
	int currentRunId_;

};

#endif // AMUSER_H
