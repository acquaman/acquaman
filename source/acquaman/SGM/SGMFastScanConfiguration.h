#ifndef SGMFASTSCANCONFIGURATION_H
#define SGMFASTSCANCONFIGURATION_H

#include "acquaman/AMFastScanConfiguration.h"
#include "SGMScanConfiguration.h"

class SGMFastScanParameters;

class SGMFastScanConfiguration : public AMFastScanConfiguration, public SGMScanConfiguration
{
	Q_OBJECT
public:
	SGMFastScanConfiguration(QObject *parent = 0);
	~SGMFastScanConfiguration();

	QString element() const { return element_;}
	double runTime() const { return runTime_;}

	QStringList options() const;

public slots:
	bool setParameters(const QString &element, double runTime);
//	bool setElement(const QString &element);
//	bool setRunTime(double runTime);

protected:
	QString element_;
	double runTime_;
	QList<SGMFastScanParameters*> settings_;
};

class SGMFastScanParameters : public QObject
{
	Q_OBJECT
public:
	SGMFastScanParameters(QObject *parent = 0);
	SGMFastScanParameters(const QString &element, double runSeconds, double energyStart, double energyMidpoint, double energyEnd, int velocity, int velocityBase, int acceleration, double scalerTime, QObject *parent = 0);

	QString element() const { return element_;}
	double runSeconds() const { return runSeconds_;}
	double energyStart() const { return energyStart_;}
	double energyMidpoint() const { return energyMidpoint_;}
	double energyEnd() const { return energyEnd_;}
	int velocity() const { return velocity_;}
	int velocityBase() const { return velocityBase_;}
	int acceleration() const { return acceleration_;}
	double scalerTime() const { return scalerTime_;}

public slots:
	void setElement(const QString &element) { element_ = element;}
	void setRunSeconds(double runSeconds) { runSeconds_ = runSeconds;}
	void setEnergyStart(double energyStart) { energyStart_ = energyStart;}
	void setEnergyMidpoint(double energyMidpoint) { energyMidpoint_ = energyMidpoint;}
	void setEnergyEnd(double energyEnd) { energyEnd_ = energyEnd;}
	void setVelocity(int velocity) { velocity_ = velocity;}
	void setVelocityBase(int velocityBase) { velocityBase_ = velocityBase;}
	void setAcceleration(int acceleration) { acceleration_ = acceleration;}
	void setScalerTime(double scalerTime) { scalerTime_ = scalerTime;}

protected:
	QString element_;
	double runSeconds_;
	double energyStart_;
	double energyMidpoint_;
	double energyEnd_;
	int velocity_;
	int velocityBase_;
	int acceleration_;
	double scalerTime_;
};

#endif // SGMFASTSCANCONFIGURATION_H
