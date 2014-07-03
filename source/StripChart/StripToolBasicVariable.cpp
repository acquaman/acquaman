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


//#include "StripToolBasicVariable.h"
//#include <QDebug>

//StripToolBasicVariable::StripToolBasicVariable(StripToolBasicVariableInfo *info, AMDataSource *dataSource, QObject *parent) :
//    StripToolAbstractVariable(info, dataSource, parent)
//{
//    QString defaultCreationDateTime = QDateTime::currentDateTime().toString();
//    Qt::CheckState defaultCheckState = Qt::Checked;
//    bool defaultSelectionState = false;

//    info_ = info;

//    setCreationDateTime(defaultCreationDateTime);
//    setCheckState(defaultCheckState);
//    setSelectionState(defaultSelectionState);

//    qDebug() << "StripToolBasicVariable object created for" << name();
//}



//StripToolBasicVariableInfo* StripToolBasicVariable::info() const {
//    return info_;
//}



//void StripToolBasicVariable::setCreationDateTime(const QString &newCreationInfo) {
//    info()->setCreationDateTime(newCreationInfo);
//}



//QString StripToolBasicVariable::creationDateTime() const {
//    if (isValid() && info()->hasCreationDateTime())
//        return info()->creationDateTime();
//    else
//        return "";
//}



//void StripToolBasicVariable::setDescription(const QString &newDescription) {
//    if (isValid()) {
//        info()->setDescription(newDescription);
//        emit descriptionChanged(description());
//        emit infoChanged();
//    }
//}



//QString StripToolBasicVariable::description() const {
//    if (isValid() && info()->hasDescription())
//        return info()->description();
//    else
//        return "";
//}



//void StripToolBasicVariable::setUnits(const QString &newUnits) {
//    if (isValid()) {
//        info()->setUnits(newUnits);
//        emit unitsChanged(units());
//        emit infoChanged();
//    }
//}



//QString StripToolBasicVariable::units() const {
//    if (isValid() && info()->hasUnits())
//        return info()->units();
//    else
//        return "";
//}



//void StripToolBasicVariable::setCheckState(Qt::CheckState &newState) {
//    if (isValid()) {
//        info()->setCheckState(newState);
//    }

//    if (info()->hasCheckState())
//        emit checkStateChanged(info()->checkState());
//}



//Qt::CheckState StripToolBasicVariable::checkState() const {
//    if (isValid() && info()->hasCheckState()) {
//        return info()->checkState();

//    } else {
//        qDebug() << "StripToolBasicVariable :: attempting to retrieve checkstate information for" << name() << "that is not valid!! Big problem!!";
//        return Qt::Unchecked;
//    }
//}



//void StripToolBasicVariable::setSelectionState(bool isSelected)
//{
//    if (isValid())
//        info()->setSelectionState(isSelected);

//    if (info()->hasSelectionState())
//        emit selectionStateChanged(info()->selectionState());
//}



//bool StripToolBasicVariable::selectionState() const {
//    if (isValid() && info()->hasSelectionState()) {
//        return info()->selectionState();

//    } else {
//        qDebug() << "StripToolBasicVariable :: attempting to retrieve selection state information for" << name() << "that is not valid!! Big problem!!";
//        return false;
//    }
//}



//void StripToolBasicVariable::onInfoChanged() {
//    // TODO code for updating the pv meta data saved to file goes here.
//}




