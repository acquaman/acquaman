#ifndef AMCURRENTAMPLIFIER_H
#define AMCURRENTAMPLIFIER_H

#include <QObject>
#include <QStringList>

class AMCurrentAmplifier : public QObject
{
    Q_OBJECT

public:
    enum AmplifierMode {
        Gain = 0,
        Sensitivity = 1,
        None = 2
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
    /// Returns the value index.
    virtual int valueIndex() const = 0;
    /// Returns the value at the given index.
    virtual double valueAt(int index) const = 0;
    /// Returns a string list of the available value options.
    virtual QList<double> values() const = 0;

    /// Returns the current units.
    virtual QString units() const = 0;
    /// Returns a string list of the available units options, suitable for a view to display.
    virtual QStringList unitsList() const = 0;

    /// Returns true if the given amplifier mode is supported.
    bool supports(AmplifierMode mode);

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

    /// Returns true if the current amplifier is in Gain mode, false otherwise.
    bool inGainMode() const;
    /// Returns true if the current amplifier is in Sensitivity mode, false otherwise.
    bool inSensitivityMode() const;

signals:
    /// Emitted when the amplifier mode has been changed.
    void amplifierModeChanged(AMCurrentAmplifier::AmplifierMode newMode);
    /// Notifier that the name of the current amplifier has changed.
    void nameChanged(const QString &newName);
    /// Notifier that the connection state of the current amplifier has changed.
    void isConnected(bool);

    /// Emitted when the gain of the current amplifier is changed.
    void gainChanged(int valueIndex);
    /// Emitted when the current amplifier has reached the minimum gain.
    void minimumGain(bool);
    /// Emitted when the current amplifier has reached the maximum gain.
    void maximumGain(bool);

    /// General notifier. Emitted when the sensitivity of the current amplifier is changed.
    void sensitivityChanged(int valueIndex);
    /// Notifier that the current amplifier is at the minimum sensitivity.
    void minimumSensitivity(bool);
    /// Notifier that the current amplifier is at the maximum sensitivity.
    void maximumSensitivity(bool);

    /// Emitted when the amplifier's units have changed, passes new units.
    void unitsChanged(const QString &newUnits);

public slots:
    /// Sets the current amplifier mode. Returns true if successful.
    bool setAmplifierMode(AmplifierMode newMode);
    /// Sets the current amplifier mode to Gain, if this mode is supported. Returns true if successful.
    bool setToGainMode();
    /// Sets the current amplifier mode to Sensitivity, if this mode is supported. Returns true if successful.
    bool setToSensitivityMode();
    /// Sets a new name for the current amplifier.
    void setName(const QString &newName);

    /// Sets the amplifier's value.
    void setValue(const QString &valueArg);

    /// Increases the gain of the current amplifier.
    virtual bool increaseGain();
    /// Decreases the gain of the current amplifier.
    virtual bool decreaseGain();

    /// Increases the sensitivity of the current amplifier.
    virtual bool increaseSensitivity();
    /// Decreases the sensitivity of the current amplifier.
    virtual bool decreaseSensitivity();

protected slots:

protected:
    /// Sets the current amplifier's sensitivity/gain value.
    virtual void setValueImplementation(const QString &valueArg) = 0;

protected:
    QString name_;
    bool connected_;
    AmplifierMode amplifierMode_;
    bool supportsGainMode_;
    bool supportsSensitivityMode_;

};

#endif // AMCURRENTAMPLIFIER_H
