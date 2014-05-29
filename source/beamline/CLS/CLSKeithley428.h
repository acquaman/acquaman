#ifndef CLSKEITHLEY428_H
#define CLSKEITHLEY428_H

#include <QObject>

#include "beamline/AMCurrentAmplifier.h"
#include "beamline/AMProcessVariable.h"
#include "beamline/CLS/CLSKeithley428ValueMap.h"

class CLSKeithley428 : public AMCurrentAmplifier
{
    Q_OBJECT

public:
    enum IndexChange {
        IncreaseOne = 0,
        DecreaseOne = 1
    };

    /// Constructor.
    explicit CLSKeithley428(const QString &name, const QString &valueName, QObject *parent = 0);
    /// Destructor.
    virtual ~CLSKeithley428();

    /// Returns the current value.
    double value(CLSKeithley428ValueMap::ValueMode mode);
    /// Returns the current value index.
    int index();
    /// Returns string representing the units to use, depending on the display mode.
    QString units(CLSKeithley428ValueMap::ValueMode mode) const;

    /// Returns the value map object for this amplifier.
    CLSKeithley428ValueMap* valueMap() const;

    /// Returns true if the current sensitivity is the maximum value allowed.
    virtual bool atMaximumSensitivity() const;
    /// Returns true if the current sensitivity ist he minimum value allowed.
    virtual bool atMinimumSensitivity() const;

    /// Returns true if the current gain is the maximum value allowed.
    bool atMaximumGain() const;
    /// Returns true if the current gain is the minimum value allowed.
    bool atMinimumGain() const;

    bool atMaximumValue(CLSKeithley428ValueMap::ValueMode mode) const;
    bool atMinimumValue(CLSKeithley428ValueMap::ValueMode mode) const;

signals:
    /// Notifier that the Keithley class is connected to the amplifier.
    void connected(bool connectState);
    /// Notifier that the gain/sensitivity value has changed.
    void valueChanged();
    /// Notifier that the gain units have changed. Passes the new units.
    void unitsChanged(const QString &units);
    /// Notifier that the gain value index has changed. Passes the new index.
    void indexChanged(int newIndex);
    /// Emitted when the gain value has been changed and the new value is the max.
    void maximumGain(bool atMax);
    /// Emitted when the gain value has been changed and the new value is the min.
    void minimumGain(bool atMin);

public slots:
    /// Sets the gain to value corresponding to provided index.
    void setValueIndex(int valueIndex);

    /// Increases the sensitivity by one value index, if not at maximum.
    virtual bool increaseSensitivity();
    /// Decreases the sensitivity by one value index, if not at minimum.
    virtual bool decreaseSensitivity();

    /// Increases the gain by one value index, if not at maximum.
    bool increaseGain();
    /// Decreases the gain by one value index, if not at minimum.
    bool decreaseGain();

protected slots:
    void onValueChanged(int newIndex);
    void onConnectedStateChanged(bool isConnected);

    void onMaximumGain();
    void onMinimumGain();

protected:
    /// Returns the next index value for the mode, found by applying change to current index.
    int nextIndex(IndexChange change, CLSKeithley428ValueMap::ValueMode mode, int currentIndex);
    /// Populates the value map with set gain values. Order corresponds to increasing pv value index.
    void setValueMap();

protected:    
    /// String holding the name of the amplifier.
    QString name_;
    /// Pointer to the pv controlling Keithley value.
    AMProcessVariable *valueControl_;
    /// Pointer to a map between gain/sensitivity and value/index.
    CLSKeithley428ValueMap *valueMap_;
};


#endif // CLSKEITHLEY428_H
