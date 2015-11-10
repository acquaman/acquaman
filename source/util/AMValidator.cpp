#include "AMValidator.h"

AMValidator::AMValidator(QObject *parent) :
    QObject(parent)
{

}

void AMValidator::updateValidity(const QString &failMessage, bool failureCriteria)
{
	bool startValidState = isValid();
	int startFailCount = failureCount();

	if(failureCriteria) {
		failureMessages_.insert(failMessage);
	} else {
		failureMessages_.remove(failMessage);
	}

	if(startValidState != isValid()) {
		emit validStateChanged(isValid());
	}

	if(startFailCount != failureCount()) {
		emit failureCountChanged(failureCount());
		if(failureCriteria) {
			emit failureAdded(failMessage);
		} else {
			emit failureRemoved(failMessage);
		}
	}
}

bool AMValidator::isValid() const
{
	return failureCount() == 0;
}

int AMValidator::failureCount() const
{
	int totalFailureCount = failureMessages_.count();

	foreach(AMValidator* childValidator, childValidators_) {

		totalFailureCount += childValidator->failureCount();
	}

	return totalFailureCount;
}

QStringList AMValidator::failureMessages() const
{
	QStringList totalFailureMessages = failureMessages_.toList();

	foreach(AMValidator* childValidator, childValidators_) {

		totalFailureMessages.append(childValidator->failureMessages());
	}

	return totalFailureMessages;
}

QString AMValidator::fullFailureMessage(const QString &delimiter) const
{
	QString fullMessage;
	QStringList failMessagesList = failureMessages();

	for (int iMessage = 0, messageCount = failMessagesList.count();
	     iMessage < messageCount;
	     ++iMessage) {

		fullMessage.append(failMessagesList.at(iMessage));

		if(iMessage != (messageCount - 1)) {
			fullMessage.append(delimiter);
		}
	}

	return fullMessage;
}

void AMValidator::addChildValidator(AMValidator *validator)
{
	if(validator) {

		bool startValidState = isValid();
		int startFailCount = failureCount();

		childValidators_.append(validator);
		connect(validator, SIGNAL(destroyed(QObject*)), this, SLOT(onChildValidatorDestroyed(QObject*)));
		connect(validator, SIGNAL(failureCountChanged(int)), this, SLOT(onChildFailureCountChanged(int)));

		if(startValidState != isValid()) {

			emit validStateChanged(isValid());
		}

		if(startFailCount != failureCount()) {

			emit failureCountChanged(failureCount());
		}
	}
}

void AMValidator::onChildValidatorDestroyed(QObject *qObject)
{
	AMValidator* childValidator = qobject_cast<AMValidator*>(qObject);
	if(childValidator) {
		bool startValidState = isValid();
		int startFailCount = failureCount();

		disconnect(childValidator, SIGNAL(destroyed()), this, SLOT(onChildValidatorDestroyed(QObject*)));
		disconnect(childValidator, SIGNAL(failureCountChanged(int)), this, SLOT(onChildFailureCountChanged(int)));
		childValidators_.removeOne(childValidator);

		if(startValidState != isValid()) {

			emit validStateChanged(isValid());
		}

		if(startFailCount != failureCount()) {

			emit failureCountChanged(failureCount());
		}
	}
}

void AMValidator::onChildFailureCountChanged(int)
{

	emit failureCountChanged(failureCount());
}
