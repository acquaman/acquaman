#ifndef CLSIONCHAMBER_H
#define CLSIONCHAMBER_H

#include "beamline/AMIonChamber.h"
#include "beamline/AMControl.h"
#include "beamline/CLS/CLSSR570.h"

/*!
  This class is a CLS specific implementation of the AMIonChamber class.  It implements the necessary functions to make
  a CLS specific ion chamber.  It offers more functionality than AMIonChamber because it is implemented with a CLSSR570
  for setting the sensitivity.  These allow you to set the gain more precisely than simply increasing or decreasing by a
  single step.
  */
class CLSIonChamber : public AMIonChamber
{
	Q_OBJECT

public:
	/// Constructor.  Takes a the name of the counts PV and the voltage PV.
	CLSIonChamber(const QString &name, const QString &description, const QString &countsPV, const QString &voltagePV, const QString &sensitivityName, const QString &sensitivityUnits, QObject *parent = 0);

	/// Reimplemented.  Returns whether the ion chamber is connected.
	virtual bool isConnected() const { return connected_; }
	/// Implements the pure virtual function.  Returns the current counts for the ion chamber.
	double counts() const { return counts_->value(); }
	/// Implements the pure virtual function.  Returns the voltage for the ion chamber.
	double voltage() const { return voltage_->value(); }

public slots:
	/// Reimplemented.  Increases the sensitivity of the ion chamber.  How this is done is implementation specific.
	bool increaseSensitivity() { return sensitivity_->increaseSensitivity(); }
	/// Reimplemented.  Decreases the sensitivity of the ion chamber.  How this is done is implementation specific.
	bool decreaseSensitivity() { return sensitivity_->decreaseSensitivity(); }

	/// Sets the value for the sensitivity directly.  Must be a valid SR570 input.
	void setSensitivityValue(int value) { sensitivity_->setValue(value); }
	/// Sets the units for the sensitivity directly.  Must be a valid SR570 input.
	void setSensitivityUnits(QString units) { sensitivity_->setUnits(units); }

protected slots:
	/// Helper slot that determines whether the ion chamber is connected or not.  Is called every time the connected status of any of the controls changes.
	void onConnectedChanged();

protected:
	/// Pointer to the counts control.
	AMControl *counts_;
	/// Pointer to the voltage control.
	AMControl *voltage_;
	/// Pointer to the SR570 used to control the sensitivity.
	CLSSR570 *sensitivity_;

	/// Bool holding the current state of connectivity for the ion chamber.
	bool connected_;
};

#endif // CLSIONCHAMBER_H
