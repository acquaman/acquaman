#include "WaterValve.h"
#include <QDebug>
WaterValve::WaterValve(const QString &name, const QList<QString> &s_pvlist, QObject *parent) :
    QObject(parent)
{

    nameOfBeamline_ = name;

    for (int i = 0; i < s_pvlist.size(); i++){

        qDebug() << "Constructor";
        qDebug() << s_pvlist[i];
        AMProcessVariable *temp = new AMProcessVariable(s_pvlist[i], true, this);
        pvList_.append(temp);
        connect( pvList_[i], SIGNAL(valueChanged()), this, SLOT(updateValveStatus()));

    }


}


void WaterValve::updateValveStatus(){

    bool valvesOn = true;

    for(int i = 0; i < pvList_.size(); i++){

        statuslist_.append(pvList_[i]->getInt());

        qDebug() << "nameOfBeamline_" << nameOfBeamline_;

        qDebug() << "i: " << i;

        qDebug() << "updateValveStatus pvList_[i]->getInt(): " << pvList_[i]->getInt();
        qDebug() << "updateValveStatus pvList_[i]->getStatus(): " << pvList_[i]->getStatus();
        qDebug() << "updateValveStatus pvList_[i]->getString(): " << pvList_[i]->getString();

        qDebug() << "updateValveStatus statuslist[i]" <<  statuslist_[i];


        if ( statuslist_[i] == -1 )
            valvesOn = false;

    }

    qDebug() << "valvesOn: " << valvesOn;
    emit valveStatusChanged(valvesOn);

}

