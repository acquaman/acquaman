#ifndef AMNUMBERCHANGEACTIONINFO_H
#define AMNUMBERCHANGEACTIONINFO_H

#include "actions3/AMActionInfo3.h"

class AMNumberChangeActionInfo : public AMActionInfo3
{
Q_OBJECT
Q_PROPERTY(int indexToChange READ indexToChange WRITE setIndexToChange)
Q_PROPERTY(int newNumber READ newNumber WRITE setNewNumber)

public:
	Q_INVOKABLE AMNumberChangeActionInfo(int newNumber = 0, int indexToChange = -1, QObject *parent = 0);

	AMNumberChangeActionInfo(const AMNumberChangeActionInfo &other);

	virtual AMActionInfo3* createCopy() const;

	virtual QString typeDescription() const;

	int newNumber() const;
	int indexToChange() const;

public slots:
	void setNewNumber(int newNumber);
	void setIndexToChange(int indexToChange);

protected:
	void updateDescriptionText();

protected:
	int newNumber_;
	int indexToChange_;
};

#endif // AMNUMBERCHANGEACTIONINFO_H
