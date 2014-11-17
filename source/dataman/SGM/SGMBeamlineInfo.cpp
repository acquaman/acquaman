/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#include "SGMBeamlineInfo.h"

SGMBeamlineInfo* SGMBeamlineInfo::instance_ = 0;

SGMBeamlineInfo* SGMBeamlineInfo::sgmInfo(){
	if(!instance_){
		instance_ = new SGMBeamlineInfo();
	}
	return instance_;
}

SGMBeamlineInfo::SGMBeamlineInfo(QObject *parent) :
	QObject(parent)
{
	standardEnergyParameters_.append( new SGMEnergyParameters(1.68923e-06, 2.45477e-05, -1.59392, 509.468, 3.05575, this), "lowGrating" );
	standardEnergyParameters_.append( new SGMEnergyParameters(9.16358e-07, 2.46204e-05, -1.59047, 511.292, 3.05478, this), "mediumGrating" );
	standardEnergyParameters_.append( new SGMEnergyParameters(5.9087e-07,  2.45691e-05, -1.59401, 510.465, 3.05458, this), "highGrating" );
}

SGMBeamlineInfo::~SGMBeamlineInfo()
{
	while(standardEnergyParameters_.count() > 0)
		standardEnergyParameters_.takeAt(standardEnergyParameters_.count()-1)->deleteLater();
}

QString SGMBeamlineInfo::sgmGratingName(SGMBeamlineInfo::sgmGrating grating) const {
	if(grating == SGMBeamlineInfo::lowGrating)
		return "lowGrating";
	else if(grating == SGMBeamlineInfo::mediumGrating)
		return "mediumGrating";
	else if(grating == SGMBeamlineInfo::highGrating)
		return "highGrating";
	else
		return "ERROR";
}

QString SGMBeamlineInfo::sgmGratingDescription(SGMBeamlineInfo::sgmGrating grating) const{
	if(grating == SGMBeamlineInfo::lowGrating)
		return "Low Energy";
	else if(grating == SGMBeamlineInfo::mediumGrating)
		return "Medium Energy";
	else if(grating == SGMBeamlineInfo::highGrating)
		return "High Energy";
	else
		return "ERROR";
}

SGMEnergyParameters* SGMBeamlineInfo::energyParametersForGrating(SGMBeamlineInfo::sgmGrating grating) const{
	return standardEnergyParametersByName(sgmGratingName(grating));
}

QString SGMBeamlineInfo::sgmHarmonicName(SGMBeamlineInfo::sgmHarmonic harmonic) const {
	if(harmonic == SGMBeamlineInfo::firstHarmonic)
		return "firstHarmonic";
	else if(harmonic == SGMBeamlineInfo::thirdHarmonic)
		return "thirdHarmonic";
	else
		return "ERROR";
}

QString SGMBeamlineInfo::sgmHarmonicDescription(SGMBeamlineInfo::sgmHarmonic harmonic) const{
	if(harmonic == SGMBeamlineInfo::firstHarmonic)
		return "First";
	else if(harmonic == SGMBeamlineInfo::thirdHarmonic)
		return "Third";
	else
		return "ERROR";
}

QString SGMBeamlineInfo::sgmDetectorSignalSourceName(SGMBeamlineInfo::sgmDetectorSignalSource dss) const{
	if(dss == SGMBeamlineInfo::sourcePicoammeters)
		return "Picoammeters";
	else if(dss == SGMBeamlineInfo::sourceScaler)
		return "Scaler";
	else
		return "ERROR";
}

QString SGMBeamlineInfo::sgmEndstationName(SGMBeamlineInfo::sgmEndstation endstation) const{
	if(endstation == SGMBeamlineInfo::scienta)
		return "Scienta";
	else if(endstation == SGMBeamlineInfo::ssa)
		return "SSA";
	else
		return "ERROR";
}

QString SGMBeamlineInfo::sgmMirrorStripeName(SGMBeamlineInfo::sgmMirrorStripe mirrorStripe) const{
	if(mirrorStripe == SGMBeamlineInfo::carbonStripe)
		return "Carbon Stripe";
	else if(mirrorStripe == SGMBeamlineInfo::siliconStripe)
		return "Silicon Stripe";
	else
		return "ERROR";
}

SGMEnergyParameters* SGMBeamlineInfo::standardEnergyParametersByName(QString gratingName) const{
	if(standardEnergyParameters_.contains(gratingName))
		return standardEnergyParameters_.at(standardEnergyParameters_.indexOfKey(gratingName));
	else
		return 0; //NULL
}

bool SGMBeamlineInfo::energyValidForSettings(SGMBeamlineInfo::sgmGrating grating, SGMBeamlineInfo::sgmHarmonic harmonic, double energy){
	if( (grating == 0) && (harmonic == SGMBeamlineInfo::firstHarmonic) && (energy > 240) && (energy < 750) )
		return true;
	else if( (grating == 1) && (harmonic == SGMBeamlineInfo::firstHarmonic) && (energy > 440) && (energy < 1200) )
		return true;
	else if( (grating == 2) && (harmonic == SGMBeamlineInfo::firstHarmonic) && (energy > 800) && (energy < 1150) )
		return true;
	else if( (grating == 2) && (harmonic == SGMBeamlineInfo::thirdHarmonic) && (energy > 1050) && (energy < 2000) )
		return true;
	else
		return false;
}

bool SGMBeamlineInfo::energyRangeValidForSettings(SGMBeamlineInfo::sgmGrating grating, SGMBeamlineInfo::sgmHarmonic harmonic, double minEnergy, double maxEnergy){
	if( (grating == 0) && (harmonic == SGMBeamlineInfo::firstHarmonic) && (minEnergy > 240) && (maxEnergy > 240) && (minEnergy < 750) && (maxEnergy < 750) )
		return true;
	else if( (grating == 1) && (harmonic == SGMBeamlineInfo::firstHarmonic) && (minEnergy > 440) && (maxEnergy > 440) && (minEnergy < 1200) && (maxEnergy < 1200) )
		return true;
	else if( (grating == 2) && (harmonic == SGMBeamlineInfo::firstHarmonic) && (minEnergy > 800) && (maxEnergy > 800) && (minEnergy < 1150) && (maxEnergy < 1150) )
		return true;
	else if( (grating == 2) && (harmonic == SGMBeamlineInfo::thirdHarmonic) && (minEnergy > 1050) && (maxEnergy > 1050) && (minEnergy < 2000) && (maxEnergy < 2000) )
		return true;
	else
		return false;
}

QList< QPair<SGMBeamlineInfo::sgmGrating, SGMBeamlineInfo::sgmHarmonic> > SGMBeamlineInfo::gratingHarmonicForEnergyRange(double minEnergy, double maxEnergy){
	QList< QPair<SGMBeamlineInfo::sgmGrating, SGMBeamlineInfo::sgmHarmonic> > rVal;
	if( (maxEnergy > 240) && (maxEnergy < 750) && (minEnergy > 240) && (minEnergy < 750) )
		rVal.append(QPair<SGMBeamlineInfo::sgmGrating, SGMBeamlineInfo::sgmHarmonic>((SGMBeamlineInfo::sgmGrating)0, SGMBeamlineInfo::firstHarmonic ));
	if((maxEnergy > 440) && (maxEnergy < 1200) && (minEnergy > 440) && (minEnergy < 1200) )
		rVal.append(QPair<SGMBeamlineInfo::sgmGrating, SGMBeamlineInfo::sgmHarmonic>((SGMBeamlineInfo::sgmGrating)1, SGMBeamlineInfo::firstHarmonic ));
	if( (maxEnergy > 800) && (maxEnergy < 1100) && (minEnergy > 800) && (minEnergy < 1100) )
		rVal.append(QPair<SGMBeamlineInfo::sgmGrating, SGMBeamlineInfo::sgmHarmonic>((SGMBeamlineInfo::sgmGrating)2, SGMBeamlineInfo::firstHarmonic ));
	if( (maxEnergy > 1100) && (maxEnergy < 2000) && (minEnergy > 1100) && (minEnergy < 2000) )
		rVal.append(QPair<SGMBeamlineInfo::sgmGrating, SGMBeamlineInfo::sgmHarmonic>((SGMBeamlineInfo::sgmGrating)2, SGMBeamlineInfo::thirdHarmonic ));
	return rVal;
}

AMRange SGMBeamlineInfo::energyRangeForGratingHarmonic(SGMBeamlineInfo::sgmGrating grating, SGMBeamlineInfo::sgmHarmonic harmonic){
	AMRange rVal;
	if( (grating == 0) && (harmonic == SGMBeamlineInfo::firstHarmonic) )
		rVal = AMRange(240, 750);
	else if( (grating == 1) && (harmonic == SGMBeamlineInfo::firstHarmonic) )
		rVal = AMRange(440, 1200);
	else if( (grating == 2) && (harmonic == SGMBeamlineInfo::firstHarmonic) )
		rVal = AMRange(800, 1100);
	else if( (grating == 2) && (harmonic == SGMBeamlineInfo::thirdHarmonic) )
		rVal = AMRange(1100, 2000);
	return rVal;
}

QPair<SGMBeamlineInfo::sgmGrating, SGMBeamlineInfo::sgmHarmonic> SGMBeamlineInfo::forBestFlux(double minEnergy, double maxEnergy) const{
	bool straddlesTransition = false;
	double testEnergy = 0;
	QList<double> transitionPoints;
	transitionPoints << 625 << 1200;
	for(int x = 0; x < transitionPoints.count(); x++){
		if(minEnergy < transitionPoints.at(x) && maxEnergy > transitionPoints.at(x)){
			straddlesTransition = true;
			if( (transitionPoints.at(x)-minEnergy) >= (maxEnergy-transitionPoints.at(x)) )
				testEnergy = minEnergy;
			else
				testEnergy = maxEnergy;
		}
	}

	if(!straddlesTransition)
		testEnergy = minEnergy;

	if(testEnergy < 625)
		return QPair<SGMBeamlineInfo::sgmGrating, SGMBeamlineInfo::sgmHarmonic>(SGMBeamlineInfo::lowGrating, SGMBeamlineInfo::firstHarmonic);
	else if(testEnergy < 1200)
		return QPair<SGMBeamlineInfo::sgmGrating, SGMBeamlineInfo::sgmHarmonic>(SGMBeamlineInfo::mediumGrating, SGMBeamlineInfo::firstHarmonic);
	else
		return QPair<SGMBeamlineInfo::sgmGrating, SGMBeamlineInfo::sgmHarmonic>(SGMBeamlineInfo::highGrating, SGMBeamlineInfo::thirdHarmonic);
}

QPair<SGMBeamlineInfo::sgmGrating, SGMBeamlineInfo::sgmHarmonic> SGMBeamlineInfo::forBestResolution(double minEnergy, double maxEnergy) const{
	bool straddlesTransition = false;
	double testEnergy = 0;
	QList<double> transitionPoints;
	transitionPoints << 400 << 800 << 1100;
	for(int x = 0; x < transitionPoints.count(); x++){
		if(minEnergy < transitionPoints.at(x) && maxEnergy > transitionPoints.at(x)){
			straddlesTransition = true;
			if( (transitionPoints.at(x)-minEnergy) >= (maxEnergy-transitionPoints.at(x)) )
				testEnergy = minEnergy;
			else
				testEnergy = maxEnergy;
		}
	}

	if(!straddlesTransition)
		testEnergy = minEnergy;

	if(testEnergy < 400)
		return QPair<SGMBeamlineInfo::sgmGrating, SGMBeamlineInfo::sgmHarmonic>(SGMBeamlineInfo::lowGrating, SGMBeamlineInfo::firstHarmonic);
	else if(testEnergy < 800)
		return QPair<SGMBeamlineInfo::sgmGrating, SGMBeamlineInfo::sgmHarmonic>(SGMBeamlineInfo::mediumGrating, SGMBeamlineInfo::firstHarmonic);
	else if(testEnergy < 1100)
		return QPair<SGMBeamlineInfo::sgmGrating, SGMBeamlineInfo::sgmHarmonic>(SGMBeamlineInfo::highGrating, SGMBeamlineInfo::firstHarmonic);
	else
		return QPair<SGMBeamlineInfo::sgmGrating, SGMBeamlineInfo::sgmHarmonic>(SGMBeamlineInfo::highGrating, SGMBeamlineInfo::thirdHarmonic);
}

 SGMEnergyParameters::~SGMEnergyParameters(){}
SGMEnergyParameters::SGMEnergyParameters(QObject *parent) : QObject(parent)
{
}

SGMEnergyParameters::SGMEnergyParameters(double spacingParameter, double c1Parameter, double c2Parameter, double sParameter, double thetaParameter, QObject *parent) :
		QObject(parent)
{
	setSpacingParameter(spacingParameter);
	setC1Parameter(c1Parameter);
	setC2Parameter(c2Parameter);
	setSParameter(sParameter);
	setThetaParameter(thetaParameter);
}

bool SGMEnergyParameters::operator ==(const SGMEnergyParameters &other){
	if( (fabs(spacingParameter() - other.spacingParameter() ) <= 0.001e-07) &&
		(fabs(c1Parameter() - other.c1Parameter()) <= 0.0001e-05) &&
		(fabs(c2Parameter() - other.c2Parameter()) <= 0.0001) &&
		(fabs(sParameter() - other.sParameter()) <= 0.01) &&
		(fabs(thetaParameter() - other.thetaParameter()) <= 0.00001) ){
		return true;
	}
	return false;
}

bool SGMEnergyParameters::operator !=(const SGMEnergyParameters &other){
	return !(this->operator ==(other));
}
