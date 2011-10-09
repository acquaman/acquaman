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

	/// Reimplemented.  Returns whether the ion chamber is connected.
	virtual bool isConnected() const { return connected_; }
	/// Implements the pure virtual function.  Returns the current counts for the ion chamber.
	double counts() const { return counts_->value(); }
	/// Implements the pure virtual function.  Returns the voltage for the ion chamber.
	double voltage() const { return voltage_->value(); }

public slots:
	/// Reimplemented.  Increases the sensitivity of the ion chamber.  How this is done is implementation specific.
	bool increaseSensitivity() { return false; }
	/// Reimplemented.  Decreases the sensitivity of the ion chamber.  How this is done is implementation specific.
	bool decreaseSensitivity() { return false; }

protected slots:
	/// Helper slot that determines whether the ion chamber is connected or not.  Is called every time the connected status of any of the controls changes.
	void onConnectedChanged();

protected:
	/// Pointer to the counts control.
	AMControl *counts_;
	/// Pointer to the voltage control.
	AMControl *voltage_;

	/// Bool holding the current state of connectivity for the ion chamber.
	bool connected_;
};

#endif // CLSIONCHAMBER_H
