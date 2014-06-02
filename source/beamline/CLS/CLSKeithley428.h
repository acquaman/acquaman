#ifndef CLSKEITHLEY428_H
#define CLSKEITHLEY428_H

#include <QObject>

#include "beamline/AMCurrentAmplifier.h"
#include "beamline/AMProcessVariable.h"

class CLSKeithley428ValueMap;

class CLSKeithley428 : public AMCurrentAmplifier
{
    Q_OBJECT

public:
    enum AmplifierMode {
        Gain = 0,
        Sensitivity = 1
    };

    enum IndexChange {
        IncreaseOne = 0,
        DecreaseOne = 1
    };

    /// Constructor.
    explicit CLSKeithley428(const QString &name, const QString &valueName, QObject *parent = 0);
    /// Destructor.
    virtual ~CLSKeithley428();

    /// Returns the amplifier mode.
    AmplifierMode amplifierMode() const;

    /// Returns the current value.
    double value();
    /// Returns the current value index.
    int index();
    /// Returns string representing the units to use, depending on the display mode.
    QString units() const;

    /// Returns the value map object for this amplifier.
    CLSKeithley428ValueMap* valueMap() const;

    /// Returns true if the current sensitivity is the maximum value allowed.
    virtual bool atMaximumSensitivity() const;
    /// Returns true if the current sensitivity ist he minimum value allowed.
    virtual bool atMinimumSensitivity() const;

    bool atMaximumValue() const;
    bool atMinimumValue() const;

    QStringList valueStringList();
//    QStringList unitsStringList();

signals:
    /// Notifier that the Keithley class is connected to the amplifier.
    void connected(bool connectState);
    /// Notifier that the amplifier mode has changed.
    void amplifierModeChanged(CLSKeithley428::AmplifierMode newMode);
    /// Notifier that the gain/sensitivity value has changed.
    void valueChanged();
    /// Notifier that the gain units have changed. Passes the new units.
    void unitsChanged(const QString &units);
    /// Notifier that the gain value index has changed. Passes the new index.
    void indexChanged(int newIndex);
    /// Emitted when the value has been changed and the new value is the max.
    void maximumValue(bool atMax);
    /// Emitted when the value has been changed and the new value is the min.
    void minimumValue(bool atMin);

public slots:
    /// Sets the amplifier value mode.
    void setAmplifierMode(CLSKeithley428::AmplifierMode newMode);
    /// Sets the gain to value corresponding to provided index.
    void setValueIndex(int valueIndex);

    /// Increases the sensitivity by one value index, if not at maximum.
    virtual bool increaseSensitivity();
    /// Decreases the sensitivity by one value index, if not at minimum.
    virtual bool decreaseSensitivity();

    /// Increases the value by one value index, if not at maximum.
    bool increaseValue();
    /// Decreases the value by one value index, if not at minimum.
    bool decreaseValue();

protected slots:
    void onValueChanged(int newIndex);
    void onConnectedStateChanged(bool isConnected);

    // for testing only.
    void onMaximumValue();
    void onMinimumValue();

protected:
    /// Returns the next index value, found by applying change to current index.
    int nextIndex(IndexChange change, int currentIndex);
    /// Populates the value map with set gain values. Order corresponds to increasing pv value index.
    void setValueMap();
    void setUnitsMap();

protected:    
    /// Flag indicating what mode the amplifier is operating in: sensitivity, gain.
    AmplifierMode amplifierMode_;
    /// Pointer to the pv controlling Keithley value.
    AMProcessVariable *valueControl_;
    /// Pointer to a map between gain/sensitivity and value/index.
    CLSKeithley428ValueMap *valueMap_;
};


#endif // CLSKEITHLEY428_H
