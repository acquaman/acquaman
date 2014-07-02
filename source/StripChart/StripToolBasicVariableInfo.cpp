/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


//#include "StripToolBasicVariableInfo.h"

//#include <QColor>
//#include <QDateTime>
//#include <QDebug>

//StripToolBasicVariableInfo::StripToolBasicVariableInfo(QObject *parent) :
//    StripToolAbstractVariableInfo(parent)
//{
//    setDescription("");
//    setUnits("");
//    setCheckState(Qt::Checked);
//    setSelectionState(false);
//}



//QString StripToolBasicVariableInfo::creationDateTime() const {
//    if (hasCreationDateTime())
//        return creationDateTime_;
//    else
//        return "";
//}



//QString StripToolBasicVariableInfo::units() const {
//    if (hasUnits())
//        return units_;
//    else
//        return "";
//}



//Qt::CheckState StripToolBasicVariableInfo::checkState() const {
//    if (hasCheckState()) {
//        return checkState_;

//    } else {
//        qDebug() << "StripToolBasicVariableInfo :: attempting to retrieve checkstate information for" << name() << "that is not valid!! Big problem!!";
//        return Qt::Unchecked;
//    }
//}



//bool StripToolBasicVariableInfo::selectionState() const {
//    if (hasSelectionState())
//        return selectionState_;
//    else
//        return false;
//}



//void StripToolBasicVariableInfo::setCreationDateTime(const QString &newCreationInfo) {
//    if (creationDateTime_ == newCreationInfo) {
//        qDebug() << "StripToolBasicVariableInfo :: Attempting to set a new creation date/time for" << name() << "that is identical to old one. No change made.";
//        return;
//    }

//    if (creationDateTime_ != "" && newCreationInfo == "")
//        hasCreationDateTime_ = false;
//    else
//        hasCreationDateTime_ = true;

//    creationDateTime_ = newCreationInfo;
//    emit infoChanged();
//}



//void StripToolBasicVariableInfo::setUnits(const QString &newUnits) {
//    hasUnits_ = true;

//    if (units() == newUnits) {
//        qDebug() << "StripToolBasicVariableInfo :: Attempting to set new units for" << name() << "that are identical to old ones. No change made.";
//        return;
//    }

//    units_ = newUnits;
//    emit infoChanged();
//}



//void StripToolBasicVariableInfo::setCheckState(Qt::CheckState isChecked) {
//    if (hasCheckState() && checkState() == isChecked) {
//        qDebug() << "StripToolBasicVariableInfo :: attempting to set a new checkstate for" << name() << "that is identical to the old one. No change made.";
//        return;
//    }

//    checkState_ = isChecked;
//    hasCheckState_ = true;
//}



//void StripToolBasicVariableInfo::setSelectionState(bool isSelected) {
//    if (hasSelectionState() && selectionState() == isSelected) {
//        qDebug() << "StripToolBasicVariableInfo :: attempting to set a new selection state for" << name() << "that is identical to the old one. No change made.";
//        return;
//    }

//    selectionState_ = isSelected;
//    hasSelectionState_ = true;
//}
