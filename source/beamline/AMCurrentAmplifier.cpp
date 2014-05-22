#include "AMCurrentAmplifier.h"

AMCurrentAmplifier::AMCurrentAmplifier(const QString &name, QObject *parent) :
    QObject(parent)
{
    name_ = name;
    connected_ = false;

    connect( this, SIGNAL(sensitivityChanged()), this, SIGNAL(gainChanged()) );
    connect( this, SIGNAL(gainChanged()), this, SIGNAL(sensitivityChanged()) );
}

AMCurrentAmplifier::~AMCurrentAmplifier()
{

}

void AMCurrentAmplifier::setName(const QString &newName)
{
    name_ = newName;
    emit nameChanged(name_);
}
