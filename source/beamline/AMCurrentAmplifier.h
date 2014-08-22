/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef AMCURRENTAMPLIFIER_H
#define AMCURRENTAMPLIFIER_H

#include <QObject>
#include <QStringList>

class AMCurrentAmplifierSingleView;

class AMCurrentAmplifier : public QObject
{
    Q_OBJECT

public:
    enum AmplifierMode {
        Gain = 0,
        Sensitivity = 1,
        None = 2
    };

    /// Constructor. Sets up base requirements for current amplifier.
    explicit AMCurrentAmplifier(const QString &name, QObject *parent = 0);
    /// Destructor.
    virtual ~AMCurrentAmplifier();

    /// Returns the current amplifier mode.
    AmplifierMode amplifierMode() const;
    /// Returns the name of the current amplifier.
    QString name() const {return name_;}
    /// Returns whether the current amplifier is connected.
    bool isConnected() const {return connected_;}

    /// Returns the value.
    virtual double value() const = 0;
    /// Returns a list of the available value options.
    virtual QList<double> values() const = 0;

    /// Returns the current units.
    virtual QString units() const = 0;
    /// Returns a string list of the available units options, suitable for a view to display.
    virtual QStringList unitsList() const = 0;

    /// Returns true if the given amplifier mode is supported.
    bool supports(AmplifierMode mode);
    /// Returns true if the current amplifier supports Gain mode.
    bool supportsGainMode() const;
    /// Returns true if the current amplifier supports Sensitivity mode.
    bool supportsSensitivityMode() const;

    /// Returns true if the current amplifier is in Gain mode, false otherwise.
    bool inGainMode() const;
    /// Returns true if the current amplifier is in Sensitivity mode, false otherwise.
    bool inSensitivityMode() const;

    /// Returns true if, for the current amplifier mode, the amplifier is at a minimum value limit. False otherwise.
    bool atMinimumValue() const;
    /// Returns true if, for the current amplifier mode, the amplifier is at a maximum value limit. False otherwise.
    bool atMaximumValue() const;

    /// Returns the minimum/maximum value + units pair for the given amplifier.
    virtual double minimumValueForUnits(const QString &units) const = 0;
    virtual double maximumValueForUnits(const QString &units) const = 0;

    /// Returns the view for this amplifier.
    virtual AMCurrentAmplifierSingleView *createView();

signals:
    /// Emitted when the amplifier mode has been changed.
    void amplifierModeChanged();
    /// Notifier that the name of the current amplifier has changed.
    void nameChanged(const QString &newName);
    /// Notifier that the connection state of the current amplifier has changed.
    void isConnected(bool);
    /// Emitted when the value of the amplifier has changed.
    void valueChanged();
    /// Emitted when a minimum value limit for the amplifier has been reached.
    void minimumValue(bool);
    /// Emitted when a maximum value limit for the amplifier has been reached.
    void maximumValue(bool);

public slots:
    /// Sets the current amplifier mode. Returns true if successful.
    bool setAmplifierMode(AmplifierMode newMode);
    /// Sets the current amplifier mode to Gain, if this mode is supported. Returns true if successful.
    bool setToGainMode();
    /// Sets the current amplifier mode to Sensitivity, if this mode is supported. Returns true if successful.
    bool setToSensitivityMode();

    /// Sets a new name for the current amplifier.
    void setName(const QString &newName);

    /// Sets the amplifier's value.
    void setValue(const QString &valueArg);
    /// Increases the value of the current amplifier.
    bool increaseValue();
    /// Decreases the value of the current amplifier.
    bool decreaseValue();

protected:
    /// Returns true if the current amplifier is at the minimum gain.
    virtual bool atMinimumGain() const;
    /// Returns true if the current amplifier is at the maximum gain.
    virtual bool atMaximumGain() const;

    /// Returns whether the current amplifier is at minimum sensitivity.
    virtual bool atMinimumSensitivity() const;
    /// Returns whether the current amplifier is at maximum sensitivity.
    virtual bool atMaximumSensitivity() const;

    /// Increases the gain of the current amplifier.
    virtual bool increaseGain();
    /// Decreases the gain of the current amplifier.
    virtual bool decreaseGain();

    /// Increases the sensitivity of the current amplifier.
    virtual bool increaseSensitivity();
    /// Decreases the sensitivity of the current amplifier.
    virtual bool decreaseSensitivity();

    /// Sets the current amplifier's sensitivity/gain value.
    virtual void setValueImplementation(const QString &valueArg) = 0;

protected:
    QString name_;
    bool connected_;
    AmplifierMode amplifierMode_;
    bool supportsGainMode_;
    bool supportsSensitivityMode_;

};

#endif // AMCURRENTAMPLIFIER_H
