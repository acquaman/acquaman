/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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
