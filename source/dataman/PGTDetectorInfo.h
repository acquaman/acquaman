#ifndef PGTDETECTORINFO_H
#define PGTDETECTORINFO_H

#include "AMSpectralOutputDetectorInfo.h"

class PGTDetectorInfo : public AMSpectralOutputDetectorInfo
{
Q_OBJECT
	Q_PROPERTY(double hvSetpoint READ hvSetpoint WRITE setHVSetpoint)
	Q_PROPERTY(double hvSetpointRangeMin READ hvSetpointRangeMin WRITE setHVSetpointRangeMin)
	Q_PROPERTY(double hvSetpointRangeMax READ hvSetpointRangeMax WRITE setHVSetpointRangeMax)

	Q_CLASSINFO("AMDbObject_Attributes", "description=PGT Detector")

public:
	PGTDetectorInfo(const QString& name, const QString& description, QObject *parent = 0);

	PGTDetectorInfo(const PGTDetectorInfo &original);

	/// Operational setpoint for High Voltage (HV)
	double hvSetpoint() const;
	double hvSetpointRangeMin() const;
	double hvSetpointRangeMax() const;
	QPair<double, double> hvSetpointRange() const;

	// Dimensionality and size:
	////////////////////////////////////

	// Using the base class (AMSpectralOutputDetector) for default rank(), size(), and axes().


	virtual bool hasDetails() const;


public slots:
	void setHVSetpoint(double hvSetpoint);
	void setHVSetpointRangeMin(double min);
	void setHVSetpointRangeMax(double max);
	void setHVSetpointRange(QPair<double, double> range);

protected:
	double hvSetpointRangeMin_, hvSetpointRangeMax_, hvSetpoint_;

};


#endif // PGTDETECTORINFO_H
