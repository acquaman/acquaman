#ifndef CLSKEITHLEY428_H
#define CLSKEITHLEY428_H

#include <QObject>

#include "beamline/AMCurrentAmplifier.h"

class CLSKeithley428 : public AMCurrentAmplifier
{
    Q_OBJECT

public:
    /// Constructor.
    explicit CLSKeithley428(const QString &name, QObject *parent = 0);
    /// Destructor.
    virtual ~CLSKeithley428();

    /// Returns whether the current amplifier is at maximum sensitivity.
    virtual bool atMaximumSensitivity() const;
    /// Returns whether the current amplifier is at minimum sensitivity.
    virtual bool atMinimumSensitivity() const;

signals:

public slots:
    virtual bool increaseSensitivity();
    virtual bool decreaseSensitivity();

protected:
    /// Represents whether the maximum sensitivity has been reached.
    bool atMaximumSensitivity_;
    /// Represents whether the minimum sensitivity has been reached.
    bool atMinimumSensitivity_;

};

#endif // CLSKEITHLEY428_H
