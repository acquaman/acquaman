#include "ControlState.h"

ControlState::ControlState(Control *ctrl, QObject *parent) :
    QObject(parent)
{
    searchChildren(ctrl);
}

bool ControlState::searchChildren(Control *ctrl)
{
    QString tmpStr = "";
    for(int x = 0; x < ctrl->numChildren(); x++){
        tmpStr.setNum(ctrl->child(x)->value());
        state_.append(QPair<QString, QString>(ctrl->child(x)->objectName(), tmpStr));
        if(ctrl->child(x)->numChildren() > 0 ){
            ControlState *ctrlSt = new ControlState(ctrl->child(x), this);
            subState_.append( QPair<int, ControlState*>(x, ctrlSt));
        }
    }
    /*
    for(int x = 0; x < ctrl->numChildren(); x++){
        state_.append(QPair<QString, QString>(ctrl->child(x)->objectName(), QString().setNum(ctrl->child(x)->value()) ));
        searchChildren(ctrl->child(x));
    }
    */
    /*
    for(int x = 0; x < ctrl->numChildren(); x++){
        qDebug() << "Has child named " << ctrl->child(x)->objectName() << " with value " << ctrl->child(x)->value();
        if(searchChildren(ctrl->children().at(x)))
            qDebug() << "";
    }
    */
    if(ctrl->numChildren() > 0)
        return TRUE;
    return FALSE;
}

void ControlState::vomit()
{
    for(int x = 0; x < state_.count(); x++){
        qDebug() << "Name: " << state_.at(x).first << " Value: " << state_.at(x).second;
        for(int y = 0; y < subState_.count(); y++){
            if(subState_.at(y).first == x){
                qDebug() << "||\nV";
                subState_.at(y).second->vomit();
                qDebug() << "";
            }
        }
    }
}
