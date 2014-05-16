#include "STDataCollection.h"

STDataCollection::STDataCollection(const QString &name, QObject *parent) :
    QObject(parent)
{
    name_ = name;
}



STDataCollection::~STDataCollection()
{

}



QString STDataCollection::name() const
{
    return name_;
}



void STDataCollection::setName(const QString &newName)
{
    name_ = newName;
    emit nameChanged(name_);
}
