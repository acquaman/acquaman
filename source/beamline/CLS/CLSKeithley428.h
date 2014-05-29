#ifndef CLSKEITHLEY428_H
#define CLSKEITHLEY428_H

#include <QObject>

#include "beamline/AMCurrentAmplifier.h"
#include "beamline/AMProcessVariable.h"
#include "beamline/CLS/CLSKeithley428ValueMap.h"

class CLSKeithley428 : public QObject
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

    /// Returns the current gain value.
    double value();
    /// Returns the current value index.
    int index();
    /// Returns string representing the units to use, depending on the display mode.
    QString units(CLSKeithley428ValueMap::ValueMode mode) const;

    /// Returns the connection state of the amplifier.
    bool isConnected();

    /// Returns the value map object for this amplifier.
    CLSKeithley428ValueMap* valueMap() const;

    /// Returns true if the current gain is the maximum value allowed.
    bool atMaximumGain() const;
    /// Returns true if the current gain is the minimum value allowed.
    bool atMinimumGain() const;

signals:
    /// Notifier that the Keithley class is connected to the amplifier.
    void connected(bool connectState);
    /// Notifier that the gain value has changed. Passes the new value.
    void valueChanged(double gain);
    /// Notifier that the gain units have changed. Passes the new units.
    void unitsChanged(const QString &units);
    /// Notifier that the gain value index has changed. Passes the new index.
    void indexChanged(int newIndex);
    /// Emitted when the gain value has been changed and the new value is the max.
    void atMaximumValue(bool atMax);
    /// Emitted when the gain value has been changed and the new value is the min.
    void atMinimumValue(bool atMin);

public slots:
    /// Sets the gain to value corresponding to provided index.
    void setValueIndex(int valueIndex);

    /// Increases the gain by one value index, if not at maximum.
    bool increaseGain();
    /// Decreases the gain by one value index, if not at minimum.
    bool decreaseGain();

protected slots:
    void onValueChanged(int newIndex);
    void onConnectedStateChanged(bool isConnected);

protected:
    /// Returns the next index value, found by applying change to current index.
    int nextIndex(IndexChange change, int currentIndex);
    /// Populates the value map with set gain values. Order corresponds to increasing pv value index.
    void setValueMap();

protected:    
    /// String holding the name of the amplifier.
    QString name_;
    /// Pointer to the pv controlling Keithley value.
    AMProcessVariable *valueControl_;
    /// Pointer to a map between gain/sensitivity and value/index.
    CLSKeithley428ValueMap *valueMap_;
    /// Bool indicating whether the Keithley class is connected to the amplifier.
    bool isConnected_;
};


#endif // CLSKEITHLEY428_H
