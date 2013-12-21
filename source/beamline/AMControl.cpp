/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "AMControl.h"

 AMControl::~AMControl(){}
AMControl::AMControl(const QString& name, const QString& units, QObject* parent, const QString description) : QObject(parent), units_(units), description_(description)
{
	setObjectName(name);
	tolerance_ = AMCONTROL_TOLERANCE_DONT_CARE;
	attemptMoveWhenWithinTolerance_ = true;
	allowsMovesWhileMoving_ = false;
	displayPrecision_ = 3;
}

//// Set the control object's children (and grandchildren, etc) based on a QMap of QString and double pairs
//// QString is the name of the child (as in child's objectName) and value is the desired move position
//// errorLevel specifies what constitutes an error (shouldn't move it, can't move it, can't find it)
//// any error causes NO MOVEMENTS to occur
//bool AMControl::setState(const QMap<QString, double> controlList, unsigned int errorLevel){

//	if(errorLevel & 0x4)
//		qdebug() << "Fail on shouldn't";
//	if(errorLevel & 0x2)
//		qdebug() << "Fail on can't";
//	if(errorLevel & 0x1)
//		qdebug() << "Fail on unfound";

//	// Copy of the QMap to pass around, remove found instances from it
//	QMap<QString, double> *tmpList = new QMap<QString, double>(controlList);
//	// New list of name and pointer to Control to add found instances to
//	QMap<QString, AMControl*> *executeList = new QMap<QString, AMControl*>();
//	// Start search
//	if(!searchSetChildren(tmpList, executeList, errorLevel))
//		return FALSE;
//	// Check if there are any unfound instances and flag as error if necessary
//	if(tmpList->count() > 0 && (errorLevel & 0x1) )
//		return FALSE;
//	// Run through original list. If it's not in "unfounds" list and if it's outside the tolerance, move it to the specified position
//	QMap<QString, double>::const_iterator i = controlList.constBegin();
//	while (i != controlList.constEnd()) {
//		if(!tmpList->contains(i.key()) && fabs(i.value() - executeList->value(i.key())->value()) > executeList->value(i.key())->tolerance() )
//			executeList->value(i.key())->move(i.value());
//		//             qdebug() << "Name is " << i.key() << " value should be " << i.value() << " value is " << executeList->value(i.key())->value();
//		++i;
//	}
//	return TRUE;
//}

//// Used internally by setStateList, called recursively.
//bool AMControl::searchSetChildren(QMap<QString, double> *controlList, QMap<QString, AMControl*> *executeList, unsigned int errorLevel){
//	AMControl* tmpCtrl = NULL;
//	// Run through all the children, check for shouldn't move and can't move errors
//	// Insert in the list of executable instances, remove from the "unfounds" list
//	for(int x = 0; x < childControlCount(); x++){
//		tmpCtrl = childControlAt(x);
//		qdebug() << "Inspecting " << tmpCtrl->objectName();
//		if(controlList->contains(tmpCtrl->objectName())){
//			qdebug() << "Checking against " << tmpCtrl->objectName() << " should " << tmpCtrl->shouldMove() << " can " << tmpCtrl->canMove();
//			if(!tmpCtrl->shouldMove() && (errorLevel & 0x4) )
//				return false;
//			if(!tmpCtrl->canMove() && tmpCtrl->shouldMove() && (errorLevel & 0x2) )
//				return false;
//			if(tmpCtrl->shouldMove() && tmpCtrl->canMove()){
//				executeList->insert(tmpCtrl->objectName(), tmpCtrl);
//				controlList->remove(tmpCtrl->objectName());
//			}
//		}
//		// Call recursively on all grandchildren (CHECK TO SEE IF NO CHILDREN?, SAVE A CALL)
//		//       for(int y = 0; y < tmpCtrl->numChildren(); y++){
//		if(tmpCtrl->childControlCount() > 0)
//			if(!tmpCtrl->searchSetChildren(controlList, executeList, errorLevel))
//				return false;
//		//       }
//	}
//	return true;
//}
