#include "StripToolBasicVariableInfo.h"

#include <QColor>
#include <QDateTime>
#include <QDebug>

StripToolBasicVariableInfo::StripToolBasicVariableInfo(QObject *parent) :
    StripToolAbstractVariableInfo(parent)
{
}



QString StripToolBasicVariableInfo::creationDateTime() const {
    if (hasCreationDateTime())
        return creationDateTime_;
    else
        return "";
}



QString StripToolBasicVariableInfo::description() const {
    if (hasDescription())
        return description_;
    else
        return "";
}



QString StripToolBasicVariableInfo::units() const {
    if (hasUnits())
        return units_;
    else
        return "";
}



Qt::CheckState StripToolBasicVariableInfo::checkState() const {
    if (hasCheckState()) {
        return checkState_;

    } else {
        qDebug() << "StripToolBasicVariableInfo :: attempting to retrieve checkstate information for" << name() << "that is not valid!! Big problem!!";
        return Qt::Unchecked;
    }
}



bool StripToolBasicVariableInfo::selectionState() const {
    if (hasSelectionState())
        return selectionState_;
    else
        return false;
}



void StripToolBasicVariableInfo::setCreationDateTime(const QString &newCreationInfo) {
    if (creationDateTime_ == newCreationInfo) {
        qDebug() << "StripToolBasicVariableInfo :: Attempting to set a new creation date/time for" << name() << "that is identical to old one. No change made.";
        return;
    }

    if (creationDateTime_ != "" && newCreationInfo == "")
        hasCreationDateTime_ = false;
    else
        hasCreationDateTime_ = true;

    creationDateTime_ = newCreationInfo;
    emit infoChanged();
}



void StripToolBasicVariableInfo::setDescription(const QString &newDescription) {
    if (description() == newDescription) {
        qDebug() << "StripToolBasicVariableInfo :: Attempting to set a new description for" << name() << "that is identical to old one. No change made.";
        return;
    }

    if (newDescription == "")
        description_ = name();
    else
        description_ = newDescription;

    hasDescription_ = true;
    emit infoChanged();

}



void StripToolBasicVariableInfo::setUnits(const QString &newUnits) {
    if (units() == newUnits) {
        qDebug() << "StripToolBasicVariableInfo :: Attempting to set new units for" << name() << "that are identical to old ones. No change made.";
        return;
    }

    if (newUnits == "")
        hasUnits_ = false;
    else
        hasUnits_ = true;

    units_ = newUnits;
    emit infoChanged();
}



void StripToolBasicVariableInfo::setCheckState(Qt::CheckState isChecked) {
    if (hasCheckState() && checkState() == isChecked) {
        qDebug() << "StripToolBasicVariableInfo :: attempting to set a new checkstate for" << name() << "that is identical to the old one. No change made.";
        return;
    }

    checkState_ = isChecked;
    hasCheckState_ = true;
}



void StripToolBasicVariableInfo::setSelectionState(bool isSelected) {
    if (hasSelectionState() && selectionState() == isSelected) {
        qDebug() << "StripToolBasicVariableInfo :: attempting to set a new selection state for" << name() << "that is identical to the old one. No change made.";
        return;
    }

    selectionState_ = isSelected;
    hasSelectionState_ = true;
}
