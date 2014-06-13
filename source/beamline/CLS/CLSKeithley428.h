#ifndef CLSKEITHLEY428_H
#define CLSKEITHLEY428_H

#include <QObject>
#include <QMultiMap>

#include "beamline/AMCurrentAmplifier.h"
#include "beamline/AMProcessVariable.h"

class CLSKeithley428ValueMap;

class CLSKeithley428 : public AMCurrentAmplifier
{
    Q_OBJECT

public:
    /// Constructor.
    explicit CLSKeithley428(const QString &name, const QString &valueName, QObject *parent = 0);
    /// Destructor.
    virtual ~CLSKeithley428();

    /// Returns the current value.
    virtual double value() const;
    /// Returns the value at the given index.
    virtual double valueAt(int index) const;
    /// Returns the current value index.
    virtual int valueIndex() const;
    /// Returns the list of possible values.
    virtual QList<double> values() const;

    /// Returns string representing the units to use, depending on the display mode.
    virtual QString units() const;
    /// Returns a string list of the available unit options.
    virtual QStringList unitsList() const;

    /// Returns true if the current index corresponds to the maximum gain allowed, false otherwise.
    virtual bool atMaximumGain() const;
    /// Returns true if the current index corresponds to the minimum gain allowed, false otherwise.
    virtual bool atMinimumGain() const;
    /// Returns true if the current index corresponds to the maximum sensitivity allowed, false otherwise.
    virtual bool atMaximumSensitivity() const;
    /// Returns true if the current index corresponds to the minimum sensitivity allowed, false otherwise.
    virtual bool atMinimumSensitivity() const;

public slots:
    /// Increases the gain by one value index, if not at maximum.
    virtual bool increaseGain();
    /// Decreases the gain by one value index, if not at minimum.
    virtual bool decreaseGain();
    /// Increases the sensitivity by one value index, if not at maximum.
    virtual bool increaseSensitivity();
    /// Decreases the sensitivity by one value index, if not at minimum.
    virtual bool decreaseSensitivity();

protected slots:
    /// Sets the gain value by index.
    void setValueIndex(int newIndex);
    /// Emits value/index changed signals and additionally performs max/min value test.
    void onValueChanged(int newIndex);
    /// Updates the connected_ state.
    void onConnectedStateChanged(bool isConnected);

protected:
    /// Returns the sensitivity value corresponding to the given gain (simple inversion).
    double toSensitivity(double gain) const;
    /// Returns the gain value corresponding to the given sensitivity (simple inversion).
    double toGain(double sensitivity) const;
    /// Sets the gain to value corresponding to provided index.
    virtual void setValueImplementation(const QString &valueArg);

protected:    
    /// Pointer to the pv controlling Keithley value.
    AMProcessVariable *valueControl_;
    /// List of available gain values.
    QList<double> gains_;
    /// String list of the available units.
    QString units_;
};


#endif // CLSKEITHLEY428_H
