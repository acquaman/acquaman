#ifndef MCPDETECTORINFO_H
#define MCPDETECTORINFO_H

#include "AMDetectorInfo.h"

class MCPDetectorInfo : public AMDetectorInfo
{
Q_OBJECT
	Q_PROPERTY(double hvSetpoint READ hvSetpoint WRITE setHVSetpoint)
	Q_PROPERTY(double hvSetpointRangeMin READ hvSetpointRangeMin WRITE setHVSetpointRangeMin)
	Q_PROPERTY(double hvSetpointRangeMax READ hvSetpointRangeMax WRITE setHVSetpointRangeMax)

	Q_CLASSINFO("AMDbObject_Attributes", "description=MCP Detector")

public:
	MCPDetectorInfo(const QString& name, const QString& description, QObject *parent = 0);

	MCPDetectorInfo(const MCPDetectorInfo &original);

	/// Operational setpoint for High Voltage (HV)
	double hvSetpoint() const;
	double hvSetpointRangeMin() const;
	double hvSetpointRangeMax() const;
	QPair<double, double> hvSetpointRange() const;


	// Dimensionality and size:
	////////////////////////////////////

	// Since this is a single-point detector, we're using the default rank(), size(), and axes() from AMDetectorInfo.

	virtual bool hasDetails() const;



public slots:
	void setHVSetpoint(double hvSetpoint);
	void setHVSetpointRangeMin(double min);
	void setHVSetpointRangeMax(double max);
	void setHVSetpointRange(QPair<double, double> range);

protected:
	double hvSetpointRangeMin_, hvSetpointRangeMax_, hvSetpoint_;
};


#endif // MCPDETECTORINFO_H
