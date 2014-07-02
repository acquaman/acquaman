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


//#include "StripToolAbstractVariableInfo.h"
//#include <QDebug>

//StripToolAbstractVariableInfo::StripToolAbstractVariableInfo(QObject *parent) :
//    QObject(parent)
//{
//    hasName_ = false;
//    hasCreationDateTime_ = false;
//    hasDescription_ = false;
//    hasUnits_ = false;
//    hasSelectionState_ = false;
//    hasCheckState_ = false;
//    hasGranularity_ = false;
//    hasColor_ = false;
//}



//bool StripToolAbstractVariableInfo::hasName() const {
//    return hasName_;
//}



//bool StripToolAbstractVariableInfo::hasCreationDateTime() const {
//    return hasCreationDateTime_;
//}



//bool StripToolAbstractVariableInfo::hasDescription() const {
//    return hasDescription_;
//}



//bool StripToolAbstractVariableInfo::hasUnits() const {
//    return hasUnits_;
//}



//bool StripToolAbstractVariableInfo::hasGranularity() const {
//    return hasGranularity_;
//}



//bool StripToolAbstractVariableInfo::hasColor() const {
//    return hasColor_;
//}


//bool StripToolAbstractVariableInfo::hasSelectionState() const {
//    return hasSelectionState_;
//}



//bool StripToolAbstractVariableInfo::hasCheckState() const {
//    return hasCheckState_;
//}



//bool StripToolAbstractVariableInfo::hasTimeUnits() const {
//    return hasTimeUnits_;
//}



//QString StripToolAbstractVariableInfo::name() const {
//    if (hasName())
//        return name_;
//    else
//        return "";
//}



//void StripToolAbstractVariableInfo::setName(const QString &sourceName) {
//    if (name_ == sourceName) {
//        qDebug() << "StripToolAbstractVariableInfo :: attempting to set a new name for" << name() << "that is identical to the old one. No change made.";
//        return;
//    }

//    if (name_ != "" && sourceName == "") {
//        qDebug() << "StripToolAbstractVariableInfo :: attempting to replace the old, non-blank name for" << name() << "with an empty string. No change made.";
//        return;
//    }

//    name_ = sourceName;
//    hasName_ = true;

//    emit infoChanged();

//}



//QString StripToolAbstractVariableInfo::description() const {
//    if (hasDescription())
//        return description_;
//    else
//        return "";
//}



//void StripToolAbstractVariableInfo::setDescription(const QString &newDescription) {
//    hasDescription_ = true;

//    if (newDescription != "") {
//        description_ = newDescription;
//        qDebug() << "StripToolAbstractVariableInfo :: setting new description for variable" << name() << ":" << description();

//    } else if (newDescription == "" && name() != "") {
//        description_ = name();
//        qDebug() << "StripToolAbstractVariableInfo :: description now set to variable name. Description : " << description();

//    } else {
//        qDebug() << "ERROR : unable to set description for this variable, its name is blank?!?!";
//    }

//    emit infoChanged();
//}
