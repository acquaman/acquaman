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

    /// Returns whether the current amplifier is at maximum sensitivity.
    virtual bool atMaximumSensitivity() const;
    /// Returns whether the current amplifier is at minimum sensitivity.
    virtual bool atMinimumSensitivity() const;

signals:

public slots:
    /// Increases sensitivity by one step, if the max sensitivity hasn't been reached.
    virtual bool increaseSensitivity();
    /// Decreases sensitivity by one step, if the min sensitivity hasn't been reached.
    virtual bool decreaseSensitivity();

protected slots:
    void onGainChanged(double newVal);

protected:
    bool decreaseGain();
    bool increaseGain();
    bool atMinimumGain() const;
    bool atMaximumGain() const;

protected:
    /// Represents whether the maximum sensitivity has been reached.
    bool atMinimumGain_;
    /// Represents whether the minimum sensitivity has been reached.
    bool atMaximumGain_;

    AMProcessVariable *valueControl_;
    AMProcessVariable *unitsControl_;

};

#endif // CLSKEITHLEY428_H
