#ifndef PGMBPMCONTROL_H
#define PGMBPMCONTROL_H


#include "beamline/AMPVControl.h"

/// Class to hold the Beam Position Monitor class.
class PGMBPMControl : public QObject
{
    Q_OBJECT
public:
    /*!
    \param name - A unique description of this control
    \param pvName - The EPICS channel-access name for this Process Variable
    \param avgValue - average beam position value, meaured in um. Provided by beamline staff.
    \param variance - allowed value variance. Provided by beamline staff.
    \param parent - QObject parent class
    */
    /// Constructor
    PGMBPMControl(const QString &name, const QString &pvName, int avgValue, int variance, QObject *parent = 0);

    /// Returns current value of beam position.
    int currentValue() const { return currentValue_; }
    /// Helper function to determin if current value is within the value specified by the average and the allowed variance.
    void updateValidity(double newValue);

signals:
    /// Emitted when value has been changed.
    void onBPMValueChanged(QString newValue);
    /// Once value has been changed, determine if it is within acceptable value.
    void stateValidityChanged(bool state);


public slots:


protected slots:
    /// Called from the parent class, sets the currentValue field, calls isValid check and emits signal.
    void setCurrentValue(double value);


protected:
    /// Control for beam position monitor
    AMReadOnlyPVControl *bpmPV_;

    /// Current beam position value, measured in um.
    int currentValue_;
    /// Average value provided by staff.
    int averageValue_;
    /// Allowed position variance. Provided by staff. If position changes outside a flag is risen to user.
    int variance_;

    /// The current validity state of the BPM.
    bool state_;

};

#endif // PGMBPMCONTROL_H
