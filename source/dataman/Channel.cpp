#include "Channel.h"

// Forward declaration of Scan
#include "Scan.h"

Channel::Channel(Scan& parent, QString name) : QObject((QObject*)&parent)
{
    name_ = name;
    this->addToScan(parent);
}

/// Adds ourself to a scan
void Channel::addToScan(Scan& destination) {
    destination.ch_ << this;
}
