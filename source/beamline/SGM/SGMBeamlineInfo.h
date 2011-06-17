#ifndef SGMBEAMLINEINFO_H
#define SGMBEAMLINEINFO_H

#include <QObject>

#include <math.h>

#include "util/AMOrderedSet.h"

class SGMEnergyParameters;

class SGMBeamlineInfo : public QObject
{
Q_OBJECT
public:
	SGMBeamlineInfo(QObject *parent = 0);

	SGMEnergyParameters* standardEnergyParametersByName(QString gratingName);

protected:
	AMOrderedSet<QString, SGMEnergyParameters*> standardEnergyParameters_;
};

class SGMEnergyParameters : public QObject
{
Q_OBJECT
public:
	SGMEnergyParameters(QObject *parent = 0);
	SGMEnergyParameters(double spacingParameter, double c1Parameter, double c2Parameter, double sParameter, double thetaParameter, QObject *parent = 0);

	bool operator==(const SGMEnergyParameters &other);
	bool operator!=(const SGMEnergyParameters &other);

	double spacingParameter() const { return spacingParameter_;}
	double c1Parameter() const { return c1Parameter_;}
	double c2Parameter() const { return c2Parameter_;}
	double sParameter() const { return sParameter_;}
	double thetaParameter() const { return thetaParameter_;}

public slots:
	void setSpacingParameter(double spacingParamter) { spacingParameter_ = spacingParamter;}
	void setC1Parameter(double c1Parameter) { c1Parameter_ = c1Parameter;}
	void setC2Parameter(double c2Parameter) { c2Parameter_ = c2Parameter;}
	void setSParameter(double sParameter) { sParameter_ = sParameter;}
	void setThetaParameter(double thetaParameter) { thetaParameter_ = thetaParameter;}

protected:
	double spacingParameter_;
	double c1Parameter_;
	double c2Parameter_;
	double sParameter_;
	double thetaParameter_;
};

#endif // SGMBEAMLINEINFO_H
