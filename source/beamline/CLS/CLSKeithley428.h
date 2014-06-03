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
    enum IndexChange {
        IncreaseOne = 0,
        DecreaseOne = 1
    };

    /// Constructor.
    explicit CLSKeithley428(const QString &name, const QString &valueName, QObject *parent = 0);
    /// Destructor.
    virtual ~CLSKeithley428();

    /// Returns the current value.
    virtual double value() const;

    /// Returns the value at the given index.
    virtual double valueAt(int index) const;

    /// Returns the current value index.
    int index() const;
    /// Returns string representing the units to use, depending on the display mode.
    QString units() const;

    /// Returns the value map object for this amplifier.
    CLSKeithley428ValueMap* valueMap() const;

    /// Returns true if the current index corresponds to the maximum gain allowed, false otherwise.
    virtual bool atMaximumGain() const;
    /// Returns true if the current index corresponds to the minimum gain allowed, false otherwise.
    virtual bool atMinimumGain() const;
    /// Returns true if the current index corresponds to the maximum sensitivity allowed, false otherwise.
    virtual bool atMaximumSensitivity() const;
    /// Returns true if the current index corresponds to the minimum sensitivity allowed, false otherwise.
    virtual bool atMinimumSensitivity() const;

    /// Returns a pointer to a string list of allowed values (according to the current amplifier mode), suitable for a view to display.
    QStringList *valueStringList() const;
    /// Returns a pointer to a string list of allowed units (according to the current amplifier mode), suitable for a view to display.
    QStringList *unitsStringList() const;

public slots:

    /// Increases the gain by one value index, if not at maximum.
    virtual bool increaseGain() const;
    /// Decreases the gain by one value index, if not at minimum.
    virtual bool decreaseGain() const;
    /// Increases the sensitivity by one value index, if not at maximum.
    virtual bool increaseSensitivity() const;
    /// Decreases the sensitivity by one value index, if not at minimum.
    virtual bool decreaseSensitivity() const;

protected slots:
    /// Emits value/index changed signals and additionally performs max/min value test.
    void onValueChanged(int newIndex);
    /// Updates the connected_ state.
    void onConnectedStateChanged(bool isConnected);

protected:
    /// Returns the next index value, found by applying change to current index.
    int nextIndex(IndexChange change, int currentIndex);
    /// Populates the value map with set gain values. Order corresponds to increasing pv value index.
    void setValueMap();
    /// Sets the gain to value corresponding to provided index.
    virtual void setValueImplementation(int valueIndex);

protected:    
    /// Pointer to the pv controlling Keithley value.
    AMProcessVariable *valueControl_;
    /// Pointer to a map between gain/sensitivity and value/index.
    CLSKeithley428ValueMap *valueMap_;
};


#endif // CLSKEITHLEY428_H
