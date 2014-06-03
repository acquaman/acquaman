#ifndef AMCURRENTAMPLIFIER_H
#define AMCURRENTAMPLIFIER_H

#include <QObject>

class AMCurrentAmplifier : public QObject
{
    Q_OBJECT

public:
    enum AmplifierMode {
        Gain = 0,
        Sensitivity = 1
    };

    /// Constructor. Sets up base requirements for current amplifier.
    explicit AMCurrentAmplifier(const QString &name, QObject *parent = 0);
    /// Destructor.
    virtual ~AMCurrentAmplifier();

    /// Returns the current amplifier mode.
    AmplifierMode amplifierMode() const;
    /// Returns the name of the current amplifier.
    QString name() const {return name_;}
    /// Returns whether the current amplifier is connected.
    bool isConnected() const {return connected_;}

    /// Returns the value.
    virtual double value() const = 0;
    /// Returns the value at the given index.
    virtual double valueAt(int index) const = 0;

    /// Returns true if the current amplifier supports Gain mode.
    bool supportsGainMode() const;
    /// Returns true if the current amplifier is at the minimum gain.
    virtual bool atMinimumGain() const;
    /// Returns true if the current amplifier is at the maximum gain.
    virtual bool atMaximumGain() const;

    /// Returns true if the current amplifier supports Sensitivity mode.
    bool supportsSensitivityMode() const;
    /// Returns whether the current amplifier is at minimum sensitivity.
    virtual bool atMinimumSensitivity() const;
    /// Returns whether the current amplifier is at maximum sensitivity.
    virtual bool atMaximumSensitivity() const;

signals:
    /// Emitted when the amplifier mode has been changed.
    void amplifierModeChanged(AmplifierMode newMode);
    /// Notifier that the name of the current amplifier has changed.
    void nameChanged(const QString &newName);
    /// Notifier that the connection state of the current amplifier has changed.
    void isConnected(bool);

    /// Emitted when the gain of the current amplifier is changed.
    void gainChanged();
    /// Emitted when the current amplifier has reached the minimum gain.
    void minimumGain(bool);
    /// Emitted when the current amplifier has reached the maximum gain.
    void maximumGain(bool);

    /// General notifier. Emitted when the sensitivity of the current amplifier is changed.
    void sensitivityChanged();
    /// Notifier that the current amplifier is at the minimum sensitivity.
    void minimumSensitivity(bool);
    /// Notifier that the current amplifier is at the maximum sensitivity.
    void maximumSensitivity(bool);

public slots:
    /// Sets the current amplifier mode.
    void setAmplifierMode(AmplifierMode newMode);
    /// Sets a new name for the current amplifier.
    void setName(const QString &newName);
    /// Sets the amplifier's value.
    void setValue(int valueIndex);

    /// Increases the gain of the current amplifier.
    virtual bool increaseGain() const;
    /// Decreases the gain of the current amplifier.
    virtual bool decreaseGain() const;

    /// Increases the sensitivity of the current amplifier.
    virtual bool increaseSensitivity() const;
    /// Decreases the sensitivity of the current amplifier.
    virtual bool decreaseSensitivity() const;

protected:
    virtual void setValueImplementation(int valueIndex);

protected:
    QString name_;
    bool connected_;
    AmplifierMode amplifierMode_;
    bool supportsGainMode_;
    bool supportsSensitivityMode_;

};

#endif // AMCURRENTAMPLIFIER_H
