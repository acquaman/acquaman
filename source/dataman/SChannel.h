#ifndef ACQMAN_CHANNEL_H
#define ACQMAN_CHANNEL_H

#include <QObject>
class Scan;

class Channel : public QObject
{
Q_OBJECT
Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

public:
    explicit Channel(Scan &parent, QString name);

    /// Channel name: (ex: "tey", "tfy_io", etc.)
    QString name() const { return name_; }

signals:
    void nameChanged(QString);

public slots:
    /// Set channel name:
    void setName(const QString& newName) { name_ = newName; }

protected:
    QString name_;


public:
    /// Adds ourself to the scan (TODO: Don't call this! Should not be public)
    void addToScan(Scan& destination);
};

#endif // CHANNEL_H
