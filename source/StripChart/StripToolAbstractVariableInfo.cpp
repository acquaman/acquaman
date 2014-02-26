#include "StripToolAbstractVariableInfo.h"
#include <QDebug>

StripToolAbstractVariableInfo::StripToolAbstractVariableInfo(QObject *parent) :
    QObject(parent)
{
    hasName_ = false;
    hasCreationDateTime_ = false;
    hasDescription_ = false;
    hasUnits_ = false;
    hasGranularity_ = false;
    hasColor_ = false;
    hasSelectionState_ = false;
    hasCheckState_ = false;
}



bool StripToolAbstractVariableInfo::hasName() const {
    return hasName_;
}



bool StripToolAbstractVariableInfo::hasCreationDateTime() const {
    return hasCreationDateTime_;
}



bool StripToolAbstractVariableInfo::hasDescription() const {
    return hasDescription_;
}



bool StripToolAbstractVariableInfo::hasUnits() const {
    return hasUnits_;
}



bool StripToolAbstractVariableInfo::hasGranularity() const {
    return hasGranularity_;
}



bool StripToolAbstractVariableInfo::hasColor() const {
    return hasColor_;
}


bool StripToolAbstractVariableInfo::hasSelectionState() const {
    return hasSelectionState_;
}



bool StripToolAbstractVariableInfo::hasCheckState() const {
    return hasCheckState_;
}



bool StripToolAbstractVariableInfo::hasTimeUnits() const {
    return hasTimeUnits_;
}



QString StripToolAbstractVariableInfo::name() const {
    if (hasName())
        return name_;
    else
        return "";
}



void StripToolAbstractVariableInfo::setName(const QString &sourceName) {
    if (name_ == sourceName) {
        qDebug() << "StripToolAbstractVariableInfo :: attempting to set a new name for" << name() << "that is identical to the old one. No change made.";
        return;
    }

    if (name_ != "" && sourceName == "") {
        qDebug() << "StripToolAbstractVariableInfo :: attempting to replace the old, non-blank name for" << name() << "with an empty string. No change made.";
        return;
    }

    name_ = sourceName;
    hasName_ = true;

    emit infoChanged();
}



QString StripToolAbstractVariableInfo::description() const {
    if (hasDescription())
        return description_;
    else
        return "";
}



void StripToolAbstractVariableInfo::setDescription(const QString &newDescription) {
    if (description_ == newDescription) {
        qDebug() << "StripToolAbstractVariableInfo :: attempting to set a new description for" << name() << "that is identical to the old one. No change made.";
        return;
    }

    if (newDescription == "") {
        setDescription(name());
        return;
    }

    description_ = newDescription;
    hasDescription_ = true;

    emit infoChanged();
}
