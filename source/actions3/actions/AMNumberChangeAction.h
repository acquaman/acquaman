#ifndef AMNUMBERCHANGEACTION_H
#define AMNUMBERCHANGEACTION_H

#include "actions3/AMAction3.h"
#include "actions3/actions/AMNumberChangeActionInfo.h"

#include <QList>
#include <QStringList>
#include <QStringListModel>
namespace AMNumberChangeActionSupport{
	extern QList<int> AMNumberChangeActionList_;
	extern QStringList AMNumberChangeActionListAsString_;
	extern QStringListModel *AMNumberChangeActionModel_;

	const QList<int>* AMNumberChangeActionList();
	void appendNumber(int number);
	bool replaceNumber(int index, int newNumber);
}

class AMNumberChangeAction : public AMAction3
{
Q_OBJECT

public:
	Q_INVOKABLE AMNumberChangeAction(AMNumberChangeActionInfo *info, QObject *parent = 0);
	AMNumberChangeAction(const AMNumberChangeAction &other);

	virtual AMAction3* createCopy() const;

	virtual bool canPause() const;

	virtual bool hasChildren() const;
	virtual int numberOfChildren() const;

protected slots:
	void goFail();
	void goSucceed();

protected:
	virtual void startImplementation();
	virtual void pauseImplementation();
	virtual void resumeImplementation();
	virtual void cancelImplementation();

	const AMNumberChangeActionInfo* numberChangeInfo() const;
	AMNumberChangeActionInfo* numberChangeInfo();


};

#endif // AMNUMBERCHANGEACTION_H
