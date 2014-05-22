#ifndef CLSKEITHLEY428_H
#define CLSKEITHLEY428_H

#include <QObject>

#include "beamline/AMCurrentAmplifier.h"
#include "beamline/AMProcessVariable.h"

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

    /// Returns the value of the sensitivity.
    double value();
    /// Returns the units of the sensitivity.
    QString units() const;

    /// Returns whether the current amplifier is at maximum sensitivity.
    virtual bool atMaximumSensitivity() const;
    /// Returns whether the current amplifier is at minimum sensitivity.
    virtual bool atMinimumSensitivity() const;

signals:
    void gainChanged();
    /// Notifier that the gain value has changed. Passes the new value.
    void valueChanged(double newGain);
    /// Notifier that the gain value index has changed. Passes the new index.
    void valueIndexChanged(int newIndex);
    /// Emitted when the units of the gain have changed.
    void unitsChanged(QString newUnits);

public slots:
    /// Sets the gain value.
    void setGainValue(int newValue);
    /// Sets the gain to value corresponding to provided index.
    void setGainIndex(int newIndex);
    /// Sets the gain units.
    void setUnits(QString newUnits);

    /// Increases the sensitivity of the current amplifier.
    virtual bool increaseSensitivity();
    /// Decreases the sensitivity of the current amplifier.
    virtual bool decreaseSensitivity();

protected slots:
    /// Increases the gain by one value index, if not at maximum.
    bool increaseGain();
    /// Decreases the gain by one value index, if not at minimum.
    bool decreaseGain();
    void onValueChanged(int newIndex);
    void onUnitsChanged(QString newUnits);

protected:
    /// Returns the current gain value (V/A).
    double gain();
    /// Returns the gain units.
    QString gainUnits() const;
    /// Returns the index of current gain value.
    int gainIndex() const;
    /// Returns the sensitivity value corresponding to the current gain value (A/V).
    double sensitivity();
    /// Returns the index corresponding to the given gain exponent.
    int expToIndex(int exp) const;
    /// Returns the gain exponent corresponding to the given index.
    int indexToExp(int index) const;
    /// Returns the gain value corresponding to the given exponent.
    double expToValue(int exp);
    /// Returns the exponent corresponding to the given gain value.
    int valueToExp(double value);
    /// Returns the next index value, found by applying change to current index.
    int nextIndex(IndexChange change, int currentIndex);
    /// Returns true if the current gain is the maximum value allowed.
    bool atMaximumGain() const;
    /// Returns true if the current gain is the minimum value allowed.
    bool atMinimumGain() const;

protected:
    /// Represents whether the minimum gain has been reached.
    bool atMinimumGain_;
    /// Represents whether the maximum gain has been reached.
    bool atMaximumGain_;

    /// Pointer to the pv controlling Keithley value.
    AMProcessVariable *valueControl_;

    QString gainUnits_;

private:
    /// Returns true if the provided value is a valid gain option.
    bool valueOkay(int value);
    /// Returns true if the provided index is a valid index option.
    bool indexOkay(int index);
    /// Returns true if the provided exponent is a vlid option.
    bool exponentOkay(int exp);
    /// Returns true if the provided units are valid.
    bool unitsOkay(QString &units);

};


#endif // CLSKEITHLEY428_H
