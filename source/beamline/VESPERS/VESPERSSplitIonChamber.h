#ifndef VESPERSSPLITIONCHAMBER_H
#define VESPERSSPLITIONCHAMBER_H

#include <QObject>

#include "beamline/AMProcessVariable.h"

/*!
  This class encasulates all the necessary components to calibrate a split ion chamber.  It can change the sensitivity to bring the counts into the linear range of the detector.
  Although specific access to the different components is possible, the gains should be set identically for this it to be used properly.  Therefore, read-only access is given
  to the different components, but write access is only given to the ion chamber as a whole unit.

	\todo This and VESPERSIonChamber should really have a super class to inherit from.  There are many similarities that can and should be encapsulated and exploit polymorphism.
*/
class VESPERSSplitIonChamber : public QObject
{
	Q_OBJECT
public:
	/// Constructor.  Builds a VESPERS specific ion chamber.
	/*!
	  \param name is the name of the ion chamber.
	  \param hvName is the full name of the PV for the high voltage.
	  \param sensitivityBaseNameA is the base name for the sensitivity value and units PV for the first half of the ion chamber (ex: AMP1607-202).
	  \param sensitivityBaseNameB is the base name for the sensitivity value and units PV for the second half of the ion chamber (ex: AMP1607-202).
	  \param voltageNameA is the full PV name for the voltage feedback for the first half of the ion chamber.
	  \param voltageNameB is the full PV name for the voltage feedback for the second half of the ion chamber.
	  \param countNameA is the full PV name for the counts feedback for the first half of the ion chamber.
	  \param countNameB is the full PV name for the counts feedback for the second half of the ion chamber.
	  */
	explicit VESPERSSplitIonChamber(QString name, QString hvName, QString sensitivityBaseNameA, QString sensitivityBaseNameB, QString voltageNameA, QString voltageNameB, QString countNameA, QString countNameB, QObject *parent = 0);

	/// Returns the name of the ion chamber.
	QString name() const { return name_; }
	/// Returns the high voltage of the ion chamber.
	int highVoltage() const { return hv_->getInt(); }
	/// Returns the value of the sensitivity.  Assumes that the sensitivity value is the same for both and therefore returns the same as the first half.
	int sensitivityValue() const { return sensitivityValueA_->getInt(); }
	/// Returns the value of the sensitivity for the first half of the ion chamber.
	int sensitivityValueA() const { return sensitivityValueA_->getInt(); }
	/// Returns the value of the sensitivity for the second half of the ion chamber.
	int sensitivityValueB() const { return sensitivityValueB_->getInt(); }
	/// Returns the units of the sensitivity.  Assumes that the sensitivity units are the same for both and therefore returns the same as the first half.
	QString sensitivityUnits() const { return sensitivityUnitsA_->getString(); }
	/// Returns the units of the sensitivity for the first half of the ion chamber.
	QString sensitivityUnitsA() const { return sensitivityUnitsA_->getString(); }
	/// Returns the units of the sensitivity for the second half of the ion chamber.
	QString sensitivityUnitsB() const { return sensitivityUnitsB_->getString(); }
	/// Returns the voltage feedback for the first half of the ion chamber.
	double voltageA() const { return voltageA_->getDouble(); }
	/// Returns the voltage feedback for the second half of the ion chamber.
	double voltageB() const { return voltageB_->getDouble(); }
	/// Returns the count feedback.  This is the full counts, which is only valid if the sensitivity for both halves are the same.
	double counts() const { return countsA_->getDouble()+countsB_->getDouble(); }
	/// Returns the count feedback for the first half of the ion chamber.
	double countsA() const { return countsA_->getDouble(); }
	/// Returns the count feedback for the second half of the ion chamber.
	double countsB() const { return countsB_->getDouble(); }

signals:
	/// Notifier that the high voltage has changed.  Passes the new value.
	void highVoltageChanged(int);
	/// Notifier that the sensitivity value has changed.  Passes the new value.
	void sensitivityValueChanged(int);
	/// Notifier that the sensitivity units have changed.  Passes the new value.
	void sensitivityUnitsChanged(QString);
	/// Notifier that the voltage feedback for the first half of the ion chamber has been updated.  Passes the new value.
	void voltageAChanged(double);
	/// Notifier that the voltage feedback for the second half of the ion chamber has been updated.  Passes the new value.
	void voltageBChanged(double);
	/// Notifier that the overall counts has been updated.  Passes the new value.
	void countsChanged(double);
	/// Notifer that the counts feedback for the first half of the ion chamber has been updated.  Passes the new value.
	void countsAChanged(double);
	/// Notifer that the counts feedback for the second half of the ion chamber has been updated.  Passes the new value.
	void countsBChanged(double);

public slots:
	/// Sets the high voltage.
	void setHighVoltage(int voltage) { hv_->setValue(voltage); }
	/// Sets the sensitivity value.  Must be 1, 2, 5, 10, 20, 50, 100, 200, or 500.  Does nothing otherwise.
	void setSensitivityValue(int value)
	{
		if (sensitivityValueOkay(value)){

			sensitivityValueA_->setValue(value);
			sensitivityValueB_->setValue(value);
		}
	}
	/// Sets the sensitivity units.  Must be pA/V, nA/V, uA/V, or mA/V.  Does nothing otherwise.
	void setSensitivityUnits(QString units)
	{
		if (sensitivityUnitsOkay(units)){

			sensitivityUnitsA_->setValue(units);
			sensitivityUnitsB_->setValue(units);
		}
	}

protected slots:
	/// Passes on the new overall counts after one of the counts is updated.
	void onCountsChanged() { emit countsChanged(countsA_->getDouble()+countsB_->getDouble()); }
	/// Turns the sensitivity value from an index to the value.
	void onSensitivityValueChanged(int index);

protected:
	/// Determines if the new sensitivity value is acceptable.
	bool sensitivityValueOkay(int value)
	{
		if (value >= 0 && value <= 8)
			return true;

		return false;
	}

	/// Determines if the new sensitivity units are acceptable.
	bool sensitivityUnitsOkay(QString units)
	{
		if (units == "pA/V" || units == "nA/V" || units == "uA/V" || units == "mA/V")
			return true;

		return false;
	}

	/// The name of the ion chamber.
	QString name_;

	/// The process variable that holds the high voltage of the ion chamber.
	AMProcessVariable *hv_;
	/// The process variable that holds the value of the sensitivity of the first half of the ion chamber.
	AMProcessVariable *sensitivityValueA_;
	/// The process variable that holds the value of the sensitivity of the second half of the ion chamber.
	AMProcessVariable *sensitivityValueB_;
	/// The process variable that holds the units of the sensitivity of the first half of the ion chamber.
	AMProcessVariable *sensitivityUnitsA_;
	/// The process variable that holds the units of the sensitivity of the second half of the ion chamber.
	AMProcessVariable *sensitivityUnitsB_;
	/// The process variable that holds the voltage of the first half of the ion chamber.
	AMProcessVariable *voltageA_;
	/// The process variable that holds the voltage of the second half of the ion chamber.
	AMProcessVariable *voltageB_;
	/// The process variable that holds the counts of the first half of the ion chamber.
	AMProcessVariable *countsA_;
	/// The process variable that holds the counts of the second half of the ion chamber.
	AMProcessVariable *countsB_;
};

#endif // VESPERSSPLITIONCHAMBER_H
