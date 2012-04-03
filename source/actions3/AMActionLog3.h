#ifndef AMACTIONLOG3_H
#define AMACTIONLOG3_H

#include "dataman/database/AMDbObject.h"
#include "actions3/AMAction3.h"
#include <QDateTime>

/// This database object is used to log a user's completed actions (AMAction) in the Acquaman Workflow/Action system.  It contains the start and end times of the action, the final state of the action (Succeeded, Failed, or Cancelled), and a child database object to store the corresponding AMActionInfo.
/*! The database table is indexed by endDateTime for fast searching/sorting by when actions finished.

In the database table, we also duplicate and store the AMActionInfo's shortDescription() (as name()), longDescription, and iconFileName so that we can access them quickly without having to load the whole AMActionInfo.
*/
class AMActionLog3 : public AMDbObject
{
    Q_OBJECT

	Q_PROPERTY(QDateTime startDateTime READ startDateTime WRITE dbLoadStartDateTime)
	Q_PROPERTY(QDateTime endDateTime READ endDateTime WRITE dbLoadEndDateTime)
	Q_PROPERTY(int finalState READ finalState WRITE dbLoadFinalState)
	Q_PROPERTY(AMDbObject* info READ info WRITE dbLoadInfo)
	// The associated ActionInfo's longDescription and iconFileName are duplicated in the database table for quick access, but they don't need to be member variables of this object since we can always retrieve them from the info().
	Q_PROPERTY(QString longDescription READ longDescription)
	Q_PROPERTY(QString iconFileName READ iconFileName)

    Q_CLASSINFO("AMDbObject_Attributes", "description=Completed Action Log3")
	// Index by completion time (This is how we will sort/present them in order.)
	Q_CLASSINFO("endDateTime", "createIndex=true")


public:
    /// Default constructor (an invalid AMActionLog)
    explicit AMActionLog3(QObject *parent = 0);
    /// To construct a log from an existing action. (The log makes a copy of the action's info()... and does not take ownership of either the action or the action's info.  You can do whatever you want with the action afterwards.)  If the action is not in a final state, this ignores it and constructs an invalid AMActionLog3.
    explicit AMActionLog3(const AMAction3* completedAction, QObject* parent = 0);
	/// Copy constructor
    AMActionLog3(const AMActionLog3& other);
	/// Destructor: deletes the info
    virtual ~AMActionLog3();

	// Static functions:
	////////////////////////////
	/// Call this function to log a completed action to the database. Returns false and does nothing if the action is still running.  Returns true if the action was successfully logged, and false if there was a problem accessing or storing in the database.
    static bool logCompletedAction(const AMAction3* completedAction, AMDatabase* database = AMDatabase::database("user"));

	// Public accessors
	////////////////////////////
	/// Returns true of this is a valid log (ie: it has been created from a finished action)
	bool isValid() const { return info_ != 0; }

    /// Access the AMActionInfo associated with the logged action. Returns 0 if this is a null log.
    const AMActionInfo3* info() const { return info_; }
	/// Access the AMActionInfo associated with the logged action. Returns 0 if this is a null log.
    AMActionInfo3* info() { return info_; }

	/// The dateTime when the action was started
	QDateTime startDateTime() const { return startDateTime_; }
	/// The dateTime when the action finished
	QDateTime endDateTime() const { return endDateTime_; }
	/// The final state of the action: will be either AMAction::Succeeded, AMAction::Cancelled, or AMAction::Failed.
	int finalState() const { return finalState_; }

	/// Convenience function to get the info()'s longDescription
	QString longDescription() const { if(info_) return info_->longDescription(); else return QString(); }
	/// Convenience function to get the info()'s iconFileName
	QString iconFileName() const { if(info_) return info_->iconFileName(); else return QString(); }

	// Public setters:
	/////////////////////////
	/// Set up this log based on the given \c completedAction. If the action is not in a final state, does nothing and returns false.
    bool setFromAction(const AMAction3* completedAction);

	// For use by the database system ONLY, during loadFromDb():
	//////////////////////////

	void dbLoadStartDateTime(const QDateTime& startDateTime);
	void dbLoadEndDateTime(const QDateTime& endDateTime);
	void dbLoadFinalState(int finalState);
	void dbLoadInfo(AMDbObject* newInfo);

signals:

public slots:

protected:
    AMActionInfo3* info_;
	int finalState_;
	QDateTime startDateTime_, endDateTime_;
};

#endif // AMACTIONLOG3_H
