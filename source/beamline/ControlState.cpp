#include "ControlState.h"

ControlState::ControlState(Control *ctrl, QObject *parent) :
    QObject(parent)
{
    name_ = ctrl->objectName();
    searchChildren(ctrl);
}

bool ControlState::searchChildren(Control *ctrl)
{
    QString tmpName = "";
    int tmpCan = 0;
    int tmpShould = 0;
    double tmpValue = -1;
    double tmpTolerance = 0;
    Control *tmpCtrl = NULL;
    ControlStatus *tmpStatus = NULL;
    for(int x = 0; x < ctrl->numChildren(); x++){
        tmpCtrl = ctrl->child(x);
        tmpName = tmpCtrl->objectName();
        tmpValue = tmpCtrl->value();
        tmpTolerance = tmpCtrl->tolerance();
        tmpCan = tmpCtrl->state();
        tmpShould = tmpCtrl->shouldMeasure() ? (tmpCtrl->shouldMove() ? 2 : 1) : 0;
        tmpStatus = new ControlStatus(tmpName, tmpCan, tmpShould, tmpValue, tmpTolerance, this);
        state_.append( tmpStatus );
        if(tmpCtrl->numChildren() > 0 ){
            ControlState *ctrlSt = new ControlState(tmpCtrl, this);
            subState_.append( QPair<int, ControlState*>(x, ctrlSt));
        }
    }
/*
    QString tmpStr = "";
    for(int x = 0; x < ctrl->numChildren(); x++){
        tmpStr.setNum(ctrl->child(x)->value());
        state_.append(QPair<QString, QString>(ctrl->child(x)->objectName(), tmpStr));
        if(ctrl->child(x)->numChildren() > 0 ){
            ControlState *ctrlSt = new ControlState(ctrl->child(x), this);
            subState_.append( QPair<int, ControlState*>(x, ctrlSt));
        }
    }
*/
    if(ctrl->numChildren() > 0)
        return TRUE;
    return FALSE;
}

void ControlState::vomit()
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

bool ControlState::restore(Control *ctrl)
{
    if(ctrl->objectName() != name_)
        return FALSE;
    for(int x = 0; x < state_.count(); x++){
        if( (state_.at(x)->should() == 2) && (ctrl->child(x)->canMove()) && ( fabs(state_.at(x)->value()-ctrl->child(x)->value()) > state_.at(x)->tolerance() ) ){
            qDebug() << "I want to restore " << state_.at(x)->name() << " from " << ctrl->child(x)->value() << " to " << state_.at(x)->value() << " exceeding tolerance " << state_.at(x)->tolerance();
        }
        for(int y = 0; y < subState_.count(); y++){
            if(subState_.at(y).first == x){
                qDebug() << "||\nV";
                subState_.at(y).second->restore(ctrl->child(x));
                qDebug() << "";
            }
        }
    }
    return TRUE;
}
