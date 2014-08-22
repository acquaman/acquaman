#ifndef AMDIRECTORYSYNCHRONIZATIONACTION_H
#define AMDIRECTORYSYNCHRONIZATIONACTION_H

#include <QObject>
#include <QDateTime>
#include "actions3/AMAction3.h"
#include "actions3/AMActionInfo3.h"
#include "util/AMDirectorySynchronizer.h"
/// @brief A class representing a Directory Synchronization backup encapsulated with an Action3 which can be
/// run in the workflow. This action is not currently accessible. In order to safely run we need to find a way
/// to predictably lock access to the database while the action runs
class AMDirectorySynchronizationAction : public AMAction3
{
	Q_OBJECT
private:
	/// @brief The AMDirectorySynchronizer which will perform the sync
	AMDirectorySynchronizer* synchronizer_;
public:
	Q_INVOKABLE AMDirectorySynchronizationAction(AMActionInfo3* info, QObject *parent = 0);
protected:
	/// @brief Starts the synchronization running
	virtual void startImplementation();
	/// @brief Pausing is not possible, so implementation is blank
	virtual void pauseImplementation();
	/// @brief Resuming is not possible, so implementation is blank
	virtual void resumeImplementation();
	/// @brief Cancelling is not possible, so implementation is blank
	virtual void cancelImplementation();
	/// @brief Skipping is not possible, so implementation is blank
	virtual void skipImplementation(const QString &command);
	/// @brief Whether or not the AMDirectorySynchronizationAction has children, always false
	virtual bool hasChildren() const;
	/// @brief The number of children the AMDirectorySynchronizationAction, always 0
	virtual int numberOfChildren() const;
signals:
	
public slots:

protected slots:
	/// @brief Slot which handles the AMDirectorySynchronizer completing successfully. Changes the action
	///	status to Finished
	void onCopyCompleted();
	/// @brief Slot which handles the AMDirectorySynchronizer failing to complete. Changes the action status to
	/// Failed
	void onCopyFailed();
	/// @brief Slot which handles the AMDirectorySynchronizer progress changed notificiations. Updates the base
	/// AMAction3 class progress
	void onCopyPercentageProgressChanged(int);
	/// @brief Slot which handles the AMDirectorySynchronizer progress message notifications. Updates the status
	/// description of the base AMAction3 class
	void onCopyProgressMessageChanged(const QString&);
	
};

#endif // AMDIRECTORYSYNCHRONIZATIONACTION_H
