#include "AMChannel.h"

// Forward declaration of Scan
#include "AMScan.h"

AMChannel::AMChannel(AMScan& parent, QString name) : QObject((QObject*)&parent)
{
    name_ = name;
    this->addToScan(parent);
}

/// Adds ourself to a scan
void AMChannel::addToScan(AMScan& destination) {
    destination.ch_ << this;
}
