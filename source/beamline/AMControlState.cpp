#include "AMControlState.h"

/// Constructor for AMControlState.
/// Sets the name to the AMControl's objectName() function output.
/// Calls searchChildren() on the control, which is called recursively.
AMControlState::AMControlState(AMControl *ctrl, QObject *parent) :
	QObject(parent)
{
	name_ = ctrl->objectName();
	searchChildren(ctrl);
}

/// Loops through the children of the control, makes an AMControlStatus for each, and appends it to the list.
/// If the child has children, then an AMControlState is made for the child, added to the index paired list.
/// By creating the AMControlState, the searchChildren is effectively called recursively.
bool AMControlState::searchChildren(AMControl *ctrl)
{
	QString tmpName = "";
	int tmpCan = 0;
	int tmpShould = 0;
	double tmpValue = -1;
	double tmpTolerance = 0;
	AMControl *tmpCtrl = NULL;
	AMControlStatus *tmpStatus = NULL;
	for(int x = 0; x < ctrl->numChildren(); x++){
		tmpCtrl = ctrl->child(x);
		tmpName = tmpCtrl->objectName();
		tmpValue = tmpCtrl->value();
		tmpTolerance = tmpCtrl->tolerance();
		tmpCan = (tmpCtrl->canMeasure() & 1) | (tmpCtrl->canMove() & 2);	// TODO TODO TODO
//        tmpShould = tmpCtrl->shouldMeasure() ? (tmpCtrl->shouldMove() ? 2 : 1) : 0;
		tmpShould = tmpCtrl->shouldMeasure() ? 1 : 0;
		tmpShould |= tmpCtrl->shouldMove() ? 2 : 0;
		tmpStatus = new AMControlStatus(tmpName, tmpCan, tmpShould, tmpValue, tmpTolerance, this);
		state_.append( tmpStatus );
		if(tmpCtrl->numChildren() > 0 ){
			AMControlState *ctrlSt = new AMControlState(tmpCtrl, this);
			subState_.append( QPair<int, AMControlState*>(x, ctrlSt));
		}
	}
/*
	QString tmpStr = "";
	for(int x = 0; x < ctrl->numChildren(); x++){
		tmpStr.setNum(ctrl->child(x)->value());
		state_.append(QPair<QString, QString>(ctrl->child(x)->objectName(), tmpStr));
		if(ctrl->child(x)->numChildren() > 0 ){
			AMControlState *ctrlSt = new AMControlState(ctrl->child(x), this);
			subState_.append( QPair<int, AMControlState*>(x, ctrlSt));
		}
	}
*/
	if(ctrl->numChildren() > 0)
		return TRUE;
	return FALSE;
}

void AMControlState::vomit()
{
	qDebug() << "My name is: " << name_;
	for(int x = 0; x < state_.count(); x++){
		qDebug() << "Name: " << state_.at(x)->name() << " Can: " << state_.at(x)->can() << " Should: " << state_.at(x)->should() << " Value: " << state_.at(x)->value() << " Tolerance: " << state_.at(x)->tolerance();
		for(int y = 0; y < subState_.count(); y++){
			if(subState_.at(y).first == x){
				qDebug() << "||\nV";
				subState_.at(y).second->vomit();
				qDebug() << "";
			}
		}
	}
}

/// The restore() function calls the restoreListInternal() function with only the AMControl as a parameter.
/// This means that there is no mask list used, so all children are restored.
bool AMControlState::restore(AMControl *ctrl){
	return restoreListInternal(ctrl);
}

/// The restoreList() function calls the restoreListInternal() function with the full set of parameters.
/// This causes only the children named in the list to be restored.
bool AMControlState::restoreList(AMControl *ctrl, QList<QString> *maskList){
	return restoreListInternal(ctrl, maskList, TRUE);
}

/// The restoreListInternal() function takes the AMControl in question, a list of names for the children to be restored, and a boolean value to determine whether the list is used or not.
/// Loops through the children of the control and checks if they need to be moved.
/// Progresses to that child's children and calls recursively.
/// Only return false if incorrect AMControl pointer used
/// \todo Ditch reference to AMControl pointer?
/// \todo Add error condition/state like setState() in AMControl?
bool AMControlState::restoreListInternal(AMControl *ctrl, QList<QString> *maskList, bool useList)
{
	if(ctrl->objectName() != name_)
		return FALSE;
	for(int x = 0; x < state_.count(); x++){
		if( (!useList || maskList->contains(state_.at(x)->name())) && (state_.at(x)->should() == 3) && (ctrl->child(x)->canMove()) && ( fabs(state_.at(x)->value()-ctrl->child(x)->value()) > state_.at(x)->tolerance() ) ){
			qDebug() << "I want to restore " << state_.at(x)->name() << " from " << ctrl->child(x)->value() << " to " << state_.at(x)->value() << " exceeding tolerance " << state_.at(x)->tolerance();
			qDebug() << "So, do it...";
			ctrl->child(x)->move(state_.at(x)->value());
		}
		for(int y = 0; y < subState_.count(); y++){
			if(subState_.at(y).first == x){
				qDebug() << "||\nV";
				subState_.at(y).second->restoreListInternal(ctrl->child(x), maskList, useList);
				qDebug() << "";
			}
		}
	}
	return TRUE;
}
