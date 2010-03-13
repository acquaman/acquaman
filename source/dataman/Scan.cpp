#include "Scan.h"

Scan::Scan(QObject *parent) :
    QObject(parent)
{
}



/// Returns specified channel by name: (returns 0 if not found)
/// Warning: this will only return the first one if multiple channels with the same name
Channel* Scan::channel(QString name) {

    foreach(Channel* ch, ch_) {
        if(ch->name() == name)
            return ch;
    }
    return 0;
}


/// Delete a channel from scan: (All return true on success)
bool Scan::deleteChannel(Channel* channel) {
    return ch_.removeOne(channel);
}

bool Scan::deleteChannel(const QString& channelName) {
    return deleteChannel(channel(channelName));
}

bool Scan::deleteChannel(size_t index) {
    if(index < (size_t)ch_.count()) {
        ch_.removeAt(index);
        return true;
    }
    else
        return false;
}

