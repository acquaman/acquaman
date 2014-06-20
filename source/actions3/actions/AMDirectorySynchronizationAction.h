#ifndef AMDIRECTORYSYNCHRONIZATIONACTION_H
#define AMDIRECTORYSYNCHRONIZATIONACTION_H

#include <QObject>
#include <QDateTime>
#include "actions3/AMAction3.h"
#include "actions3/AMActionInfo3.h"
#include "util/AMDirectorySynchronizer.h"
class AMDirectorySynchronizationAction : public AMAction3
{
	Q_OBJECT
private:
	AMDirectorySynchronizer* synchronizer_;
public:
	Q_INVOKABLE AMDirectorySynchronizationAction(AMActionInfo3* info, QObject *parent = 0);
protected:
	virtual void startImplementation();
	virtual void pauseImplementation();
	virtual void resumeImplementation();
	virtual void cancelImplementation();
	virtual void skipImplementation(const QString &command);
	virtual bool hasChildren() const;
	virtual int numberOfChildren() const;
signals:
	
public slots:

protected slots:
	void onCopyCompleted();
	void onCopyFailed();
	void onCopyPercentageProgressChanged(int);
	void onCopyProgressMessageChanged(const QString&);
	
};

#endif // AMDIRECTORYSYNCHRONIZATIONACTION_H
