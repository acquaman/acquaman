#include "AMValidator.h"

AMValidator::AMValidator(QObject *parent) :
    QObject(parent)
{
}

void AMValidator::updateValidity(const QString &failMessage, bool failureCriteria)
{
	bool startValidState = isValid();
	int startFailCount = failCount();

    if(failureCriteria) {
        failMessages_.insert(failMessage);
	} else {
        failMessages_.remove(failMessage);
	}

	if(startValidState != isValid()) {
		emit validStateChanged(isValid());
	}

	if(startFailCount != failCount()) {
		emit failCountChanged(failCount());
	}
}

bool AMValidator::isValid() const
{
	return failMessages_.isEmpty();
}

int AMValidator::failCount() const
{
	return failMessages_.count();
}

QStringList AMValidator::failureMessages() const
{
	return failMessages_.toList();
}
