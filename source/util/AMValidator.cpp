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
        if(failureCriteria) {
            emit failureAdded(failMessage);
        } else {
            emit failureRemoved(failMessage);
        }
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

QString AMValidator::fullFailureMessage(const QString &delimiter) const
{
    QString fullMessage;
    QStringList failMessagesList = failMessages_.toList();

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
