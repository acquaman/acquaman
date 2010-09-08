#ifndef AMUSER_H
#define AMUSER_H

#include "dataman/AMDbObject.h"
#include "dataman/AMDatabaseDefinition.h"

class AMUser : public AMDbObject
{
Q_OBJECT
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

	// Implement the Meta-data system
	///////////////////////////

	/// We write this function to specify all of our unique pieces of meta-data (excluding those inherited from base classes -- ie: own only)
	static QList<AMMetaMetaData> metaDataUniqueKeys() {
		QList<AMMetaMetaData> rv;
		// The user's name is stored under the name() field in the base class AMDbObject

		// The id of the user's current run is stored here:
		rv << AMMetaMetaData(QVariant::Int, "currentRunId", true);

		return rv;
	}


	static QList<AMMetaMetaData> metaDataKeys() {
		return AMDbObject::metaDataKeys() << metaDataUniqueKeys();
	}

	virtual QList<AMMetaMetaData> metaDataAllKeys() const {
		return this->metaDataKeys();
	}

	/// Stores user information in a separate table.
	virtual QString databaseTableName() const {
		return AMDatabaseDefinition::userTableName();
	}



	// Convenient access methods for our meta-data:
	/////////////////////////

	/// Returns the id of the run the user is currently working inside
	int currentRunId() const {
		return metaData("currentRunId").toInt();
	}



signals:

public slots:
	/// Set the current run id. There is no error-checking or validity checking here, so please make sure it's a valid run.
	void setCurrentRunId(int id) {
		setMetaData("currentRunId", id);
	}


protected:

	/// Protected constructor. Access the singleton instance with AMUser::user().
	explicit AMUser();

	/// singleton instance variable:
	static AMUser* instance_;



};

#endif // AMUSER_H
