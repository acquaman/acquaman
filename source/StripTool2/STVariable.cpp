#include "STVariable.h"

STVariable::STVariable(STDataCollection *dataCollection, QObject *parent) :
    QObject(parent)
{
    dataCollection_ = dataCollection;
}



STVariable::~STVariable()
{

}



STDataCollection* STVariable::dataCollection()
{
    return dataCollection_;
}



void STVariable::setDataCollection(STDataCollection *newDataCollection)
{
    dataCollection_ = newDataCollection;
    emit dataCollectionChanged(dataCollection_);
}
