#include "AMCurrentAmplifier.h"

AMCurrentAmplifier::AMCurrentAmplifier(const QString &name, ValueType valueType, QObject *parent) :
    QObject(parent)
{
    name_ = name;
    connected_ = false;
    valueType_ = valueType;
}

AMCurrentAmplifier::~AMCurrentAmplifier()
{

}

void AMCurrentAmplifier::setName(const QString &newName)
{
    name_ = newName;
    emit nameChanged(name_);
}
