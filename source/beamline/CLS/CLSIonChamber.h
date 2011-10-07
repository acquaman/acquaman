#ifndef CLSIONCHAMBER_H
#define CLSIONCHAMBER_H

#include "beamline/AMIonChamber.h"
#include "beamline/AMControl.h"

/*!
  This class is a CLS specific implementation of the AMIonChamber class.  It implements the necessary functions to make
  a CLS specific ion chamber.
  */
class CLSIonChamber : public AMIonChamber
{
	Q_OBJECT

public:
	/// Constructor.  Takes a the name of the counts PV and the voltage PV.
	CLSIonChamber(const QString &name, const QString &description, const QString &countsPV, const QString &voltagePV, QObject *parent = 0);

	/// Implements the pure virtual function.  Returns the current counts for the ion chamber.
	double counts() const { return counts_->value(); }
	/// Implements the pure virtual function.  Returns the voltage for the ion chamber.
	double voltage() const { return voltage_->value(); }

public slots:
	/// Pure virtual function.  Increases the sensitivity of the ion chamber.  How this is done is implementation specific.
	bool increaseSensitivity() { return false; }
	/// Pure virtual function.  Decreases the sensitivity of the ion chamber.  How this is done is implementation specific.
	bool decreaseSensitivity() { return false; }

protected:
	/// Pointer to the counts control.
	AMControl *counts_;
	/// Pointer to the voltage control.
	AMControl *voltage_;
};

#endif // CLSIONCHAMBER_H
