#ifndef CLSKEITHLEY428_H
#define CLSKEITHLEY428_H

#include <QObject>

#include "beamline/AMCurrentAmplifier.h"

#include "beamline/AMProcessVariable.h"

class CLSKeithley428 : public AMCurrentAmplifier
{
    Q_OBJECT

public:
    /// Constructor.
    explicit CLSKeithley428(const QString &name, const QString &valueName, const QString &unitsName, QObject *parent = 0);
    /// Destructor.
    virtual ~CLSKeithley428();

    /// Returns the value of the gain.
    int value();
    /// Returns the index of the gain.
    int valueIndex() const;
    /// Returns the units of the gain.
    QString units() const;

    /// Returns whether the current amplifier is at maximum gain.
    virtual bool atMaximumValue() const;
    /// Returns whether the current amplifier is at minimum gain.
    virtual bool atMinimumValue() const;

signals:
    /// Emitted when the value of the gain has changed.
    void gainChanged(int newValue);
    void unitsChanged(QString newUnits);

public slots:
    /// Sets the gain value.
    void setValue(int newValue);
    /// Sets the gain units.
    void setUnits(QString newUnits);

    /// Increases gain by one step, if the max gain hasn't been reached.
    virtual bool increaseValue();
    /// Decreases gain by one step, if the min gain hasn't been reached.
    virtual bool decreaseValue();

protected slots:
    void onValueChanged(int newIndex);
    void onUnitsChanged(QString newUnits);

protected:
    /// Represents whether the maximum sensitivity has been reached.
    bool atMinimumGain_;
    /// Represents whether the minimum sensitivity has been reached.
    bool atMaximumGain_;

    AMProcessVariable *valueControl_;
    AMProcessVariable *unitsControl_;

private:
    int valueToIndex(int value) const;
    int indexToValue(int index) const;
    bool indexOkay(int index);
    bool unitsOkay(QString units);

};

#endif // CLSKEITHLEY428_H
