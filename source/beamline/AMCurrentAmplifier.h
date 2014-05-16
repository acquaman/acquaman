#ifndef AMCURRENTAMPLIFIER_H
#define AMCURRENTAMPLIFIER_H

#include <QObject>

class AMCurrentAmplifier : public QObject
{
    Q_OBJECT

public:

    /// Constructor. Sets up base requirements for current amplifier.
    explicit AMCurrentAmplifier(const QString &name, QObject *parent = 0);
    /// Destructor.
    virtual ~AMCurrentAmplifier();

    /// Returns whether the current amplifier is at maximum sensitivity.
    virtual bool atMaximumSensitivity() const = 0;
    /// Returns whether the current amplifier is at minimum sensitivity.
    virtual bool atMinimumSensitivity() const = 0;

    /// Returns the name of the current amplifier.
    QString name() const {return name_;}

    /// Returns whether the current amplifier is connected.
    bool isConnected() const {return connected_;}

signals:
    /// General notifier. Emitted when the sensitivity of the current amplifier is changed.
    void sensitivityChanged();
    /// Notifier that the current amplifier is at the minimum sensitivity/gain.
    void minimumSensitivity(bool);
    /// Notifier that the current amplifier is at the maximum sensitivity/gain.
    void maximumSensitivity(bool);
    /// Notifier that the name of the current amplifier has changed.
    void nameChanged(const QString &newName);
    /// Notifier that the connection state of the current amplifier has changed.
    void isConnected(bool);

public slots:
    /// Sets a new name for the current amplifier.
    void setName(const QString &newName);
    /// Increases the sensitivity of the current amplifier.
    virtual bool increaseSensitivity() = 0;
    /// Decreases the sensitivity of the current amplifier.
    virtual bool decreaseSensitivity() = 0;

protected:
    QString name_;
    bool connected_;

};

#endif // AMCURRENTAMPLIFIER_H
