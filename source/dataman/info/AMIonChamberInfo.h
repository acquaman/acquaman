#ifndef AMIONCHAMBERINFO_H
#define AMIONCHAMBERINFO_H

#include "dataman/info/AMDetectorInfo.h"

#include <QPair>

/*!
  This class incapuslates the information contained in a general ion chamber.  Things that are important to
  an ion chamber are the counts used for data analysis, the voltage used for ensuring that the detector is in
  its linear range.  It also contains a range for which the detector can know if it is in that linear range.
  */
class AMIonChamberInfo : public AMDetectorInfo
{
	Q_OBJECT

	Q_PROPERTY(int counts READ counts WRITE setCounts)
	Q_PROPERTY(double voltage READ voltage WRITE setVoltage)
	Q_PROPERTY(double minimumVoltage READ minimumVoltage WRITE setMinimumVoltage)
	Q_PROPERTY(double maximumVoltage READ maximumVoltage WRITE setMaximumVoltage)

	Q_CLASSINFO("AMDbObject_Attributes", "description=Generic Detector")

public:
	/// Constructor.  Provide the name and description for the ion chamber.
	Q_INVOKABLE AMIonChamberInfo(const QString& name = "ionChamber", const QString& description = "Ion Chamber", QObject *parent = 0);

	/// Returns the current counts for the ion chamber.
	int counts() const { return counts_; }
	/// Returns the voltage for the ion chamber.
	double voltage() const { return voltage_; }
	/// Returns the minimum voltage range for the ion chamber.
	double minimumVoltage() const { return voltageRange_.first; }
	/// Returns the maximum voltage range for the ion chamber.
	double maximumVoltage() const { return voltageRange_.second; }
	/// Returns the voltage range for the ion chamber.
	QPair<double, double> voltageRange() const { return voltageRange_; }

public slots:
	/// Sets the counts for the ion chamber.
	void setCounts(int counts) { counts_ = counts; setModified(true); }
	/// Sets the voltage for the ion chamber.
	void setVoltage(double voltage) { voltage_ = voltage; setModified(true); }
	/// Sets the minimum voltage for the linear range for the detector.
	void setMinimumVoltage(double min) { voltageRange_.first = min; setModified(true); }
	/// Sets the maximum voltage for the linear range for the detector.
	void setMaximumVoltage(double max) { voltageRange_.second = max; setModified(true); }
	/// Sets the linear voltage range.
	void setVoltagRange(QPair<double, double> range) { setMinimumVoltage(range.first); setMaximumVoltage(range.second); }

protected:
	/// The counts.
	int counts_;
	/// The voltage.
	double voltage_;
	/// The linear voltage range of the detector.
	QPair<double, double> voltageRange_;
};

#endif // AMIONCHAMBERINFO_H
