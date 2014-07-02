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


#include "SGMOptimizationSupport.h"

#include "SGMBeamline.h"

 SGMFluxOptimization::~SGMFluxOptimization(){}
SGMFluxOptimization::SGMFluxOptimization(QObject *parent) : AMControlOptimization(parent) {
	name_ = "SGMFlux";
}

QMap<double, double> SGMFluxOptimization::curve(QList<QVariant> stateParameters, AMRegionsList* contextParameters){
	double _maxenergy = maximumEnergy(contextParameters);
	double _minenergy = minimumEnergy(contextParameters);
	double _maxflux = 0;
	double _minflux = 0;
	double _maximum = 0;
	double _minimum = 1;
	double _slit = stateParameters.at(0).toDouble();
	SGMBeamlineInfo::sgmGrating _grating = (SGMBeamlineInfo::sgmGrating)stateParameters.at(1).toInt();
	SGMBeamlineInfo::sgmHarmonic _harmonic = (SGMBeamlineInfo::sgmHarmonic)stateParameters.at(2).toInt();
	if(!SGMBeamlineInfo::sgmInfo()->energyRangeValidForSettings(_grating, _harmonic, _minenergy, _maxenergy))
	{
	}
	else if((_harmonic == SGMBeamlineInfo::thirdHarmonic) && (_grating == SGMBeamlineInfo::highGrating)){

		_maxflux = 1.2;
		_minflux = 1.05;
		_maximum = 1600;
		_minimum = 1100;
	}
	else{
		if( (_grating == 0) && SGMBeamlineInfo::sgmInfo()->energyRangeValidForSettings(_grating, _harmonic, _minenergy, _maxenergy) ){
			_maxflux = 5.75;
			_minflux = 0.5;
			_maximum = 475;
			_minimum = 200;
		}
		else if( (_grating == 1) && SGMBeamlineInfo::sgmInfo()->energyRangeValidForSettings(_grating, _harmonic, _minenergy, _maxenergy) ){
			_maxflux = 3.25;
			_minflux = 0.5;
			_maximum = 815;
			_minimum = 450;
		}
		else if( (_grating == 2) && SGMBeamlineInfo::sgmInfo()->energyRangeValidForSettings(_grating, _harmonic, _minenergy, _maxenergy) ){
			_maxflux = 1.1;
			_minflux = 0.5;
			_maximum = 1075;
			_minimum = 800;
		}
	}
	QMap<double, double> rCurve;
	double tmpStart, tmpEnd, tmpDelta;
	for( int x = 0; x < contextParameters->count(); x++){
		tmpStart = contextParameters->start(x);
		tmpDelta = contextParameters->delta(x);
		tmpEnd = contextParameters->end(x);
		if( tmpStart >= 250 && tmpStart <= 2000 && tmpEnd >= 250 && tmpEnd <= 2000 ){
			for( double y = tmpStart; ((tmpDelta > 0) ? (y <= tmpEnd) : (y >= tmpEnd)); y += tmpDelta ){
				rCurve[y] = !SGMBeamlineInfo::sgmInfo()->energyValidForSettings(_grating, _harmonic, y) ? 0.0 : (_slit/62500)*(500-_slit)* ( ((_minflux-_maxflux)/((_minimum-_maximum)*(_minimum-_maximum)))*(y-_maximum)*(y-_maximum)+_maxflux );
			}
		}
		else
			rCurve[250] = 0;
	}
	return rCurve;
}

QMap< QString, QMap<double, double> > SGMFluxOptimization::collapse(AMRegionsList *contextParameters){
	QList<QVariant> l1, m1, h1, h3;
	l1 << 250.0 << 0 << 1;
	m1 << 250.0 << 1 << 1;
	h1 << 250.0 << 2 << 1;
	h3 << 250.0 << 2 << 3;
	int numPoints = 50;
	double stepSize = 250/(numPoints-1);
	QMap<double, double> fluxL1, fluxM1, fluxH1, fluxH3;
	for(double x = stepSize; x < 250; x+=stepSize){
		l1.replace(0, x);
		m1.replace(0, x);
		h1.replace(0, x);
		h3.replace(0, x);

		fluxL1.insert(x, collapser(curve(l1, contextParameters)));
		fluxM1.insert(x, collapser(curve(m1, contextParameters)));
		fluxH1.insert(x, collapser(curve(h1, contextParameters)));
		fluxH3.insert(x, collapser(curve(h3, contextParameters)));
	}

	QMap< QString, QMap<double, double> > rVal;
	rVal.insert("LEG1", fluxL1);
	rVal.insert("MEG1", fluxM1);
	rVal.insert("HEG1", fluxH1);
	rVal.insert("HEG3", fluxH3);
	return rVal;
}

double SGMFluxOptimization::collapser(QMap<double, double> optCurve){
	QMap<double, double>::const_iterator i;
	i = optCurve.constBegin();
	double tmpMin = i.value();
	++i;
	while(i != optCurve.constEnd()){
		if(i.value() < tmpMin)
			tmpMin = i.value();
		++i;
	}
	return tmpMin;
}

double SGMFluxOptimization::maximumEnergy(AMRegionsList* regions){
	double curMax = 240;
	if(regions->count() == 0)
		return -1;
	for(int x = 0; x < regions->count(); x++){
		curMax = (regions->start(x) > curMax ? regions->start(x) : curMax);
		curMax = (regions->end(x) > curMax ? regions->end(x) : curMax);
	}
	return curMax;
}

double SGMFluxOptimization::minimumEnergy(AMRegionsList* regions){
	double curMin = 2000;
	if(regions->count() == 0)
		return -1;
	for(int x = 0; x < regions->count(); x++){
		curMin = (regions->start(x) < curMin ? regions->start(x) : curMin);
		curMin = (regions->end(x) < curMin ? regions->end(x) : curMin);
	}
	return curMin;
}


 SGMResolutionOptimization::~SGMResolutionOptimization(){}
SGMResolutionOptimization::SGMResolutionOptimization(QObject *parent) : AMControlOptimization(parent) {
	name_ = "SGMResolution";
}

QMap<double, double> SGMResolutionOptimization::curve(QList<QVariant> stateParameters, AMRegionsList* contextParameters){
	double _maxenergy = maximumEnergy(contextParameters);
	double _minenergy = minimumEnergy(contextParameters);
	double _slit = stateParameters.at(0).toDouble();
	double _y1 = 0;
	double _y2 = 0;
	double _y3 = 0;
	double _x1 = 0;
	double _x2 = 0;
	double _x3 = 0;
	double _maxRes = 0;
	SGMBeamlineInfo::sgmGrating _grating = (SGMBeamlineInfo::sgmGrating)stateParameters.at(1).toInt();
	SGMBeamlineInfo::sgmHarmonic _harmonic = (SGMBeamlineInfo::sgmHarmonic)stateParameters.at(2).toInt();
	if(!SGMBeamlineInfo::sgmInfo()->energyRangeValidForSettings(_grating, _harmonic, _minenergy, _maxenergy))
	{
	}
	else if((_harmonic == SGMBeamlineInfo::thirdHarmonic) && (_grating == SGMBeamlineInfo::highGrating)){
		_y1 = (0.95)*(0.5229*log(_slit)+1.4221);
		_y2 = (0.95)*(0.4391*log(_slit)+1.2617);
		_y3 = (0.95)*(0.421*log(_slit)+0.9037);
		_x1 = 2000;
		_x2 = 1200;
		_x3 = 800;
		_maxRes = 12500;
	}
	else{
		if( (_grating == 0) && SGMBeamlineInfo::sgmInfo()->energyRangeValidForSettings(_grating, _harmonic, _minenergy, _maxenergy) ){
			_y1 = 0.4568*log(_slit)+1.0199;
			_y2 = 0.4739*log(_slit)+0.605;
			_y3 = 0.4257*log(_slit)+0.4438;
			_x1 = 600;
			_x2 = 400;
			_x3 = 250;
			_maxRes = 17500;
		}
		else if( (_grating == 1) && SGMBeamlineInfo::sgmInfo()->energyRangeValidForSettings(_grating, _harmonic, _minenergy, _maxenergy) ){
			_y1 = 0.425*log(_slit)+1.4936;
			_y2 = 0.4484*log(_slit)+1.0287;
			_y3 = 0.4029*log(_slit)+0.7914;
			_x1 = 1200;
			_x2 = 800;
			_x3 = 500;
			_maxRes = 15000;
		}
		else if( (_grating == 2) && SGMBeamlineInfo::sgmInfo()->energyRangeValidForSettings(_grating, _harmonic, _minenergy, _maxenergy) ){
			_y1 = 0.5229*log(_slit)+1.4221;
			_y2 = 0.4391*log(_slit)+1.2617;
			_y3 = 0.421*log(_slit)+0.9037;
			_x1 = 2000;
			_x2 = 1200;
			_x3 = 800;
			_maxRes = 13750;
		}
	}
	int i, n;
	double xi, yi, ei, chisq;
	gsl_matrix *X, *cov;
	gsl_vector *y, *w, *c;

	n = 3;

	X = gsl_matrix_alloc (n, 3);
	y = gsl_vector_alloc (n);
	w = gsl_vector_alloc (n);

	c = gsl_vector_alloc (3);
	cov = gsl_matrix_alloc (3, 3);

	double ix[3];
	double iy[3];
	double ie[3];
	ix[0] = _x1;
	ix[1] = _x2;
	ix[2] = _x3;
	iy[0] = _y1;
	iy[1] = _y2;
	iy[2] = _y3;
	ie[0] = 0.1*iy[0];
	ie[1] = 0.1*iy[1];
	ie[2] = 0.1*iy[2];
	for (i = 0; i < n; i++)
	{
		xi = ix[i];
		yi = iy[i];
		ei = ie[i];

		gsl_matrix_set (X, i, 0, 1.0);
		gsl_matrix_set (X, i, 1, xi);
		gsl_matrix_set (X, i, 2, xi*xi);

		gsl_vector_set (y, i, yi);
		gsl_vector_set (w, i, 1.0/(ei*ei));
	}

	gsl_multifit_linear_workspace * work
			= gsl_multifit_linear_alloc (n, 3);
	gsl_multifit_wlinear (X, w, y, c, cov,
				  &chisq, work);
	gsl_multifit_linear_free (work);

#define C(i) (gsl_vector_get(c,(i)))
#define COV(i,j) (gsl_matrix_get(cov,(i),(j)))

	QMap<double, double> rCurve;
	double tmpStart, tmpEnd, tmpDelta;//, tmpVal;
	for( int x = 0; x < contextParameters->count(); x++){
		tmpStart = contextParameters->start(x);
		tmpDelta = contextParameters->delta(x);
		tmpEnd = contextParameters->end(x);

		if( tmpStart >= 250 && tmpStart <= 2000 && tmpEnd >= 250 && tmpEnd <= 2000 ){
			for( double y = tmpStart; ((tmpDelta > 0) ? (y <= tmpEnd) : (y >= tmpEnd)); y += tmpDelta ){
				//rCurve[y] = !SGMBeamline::sgm()->energyValidForSettings(_grating, _harmonic, y) ? 0.0 :y/(pow(10, C(2)*y*y + C(1)*y + C(0))*1e-3);
				if(!SGMBeamlineInfo::sgmInfo()->energyValidForSettings(_grating, _harmonic, y))
					rCurve[y] = 0.0;
				else{
					rCurve[y] = y/(pow(10, C(2)*y*y + C(1)*y + C(0))*1e-3);
				}
			}
		}
		else
			rCurve[250] = 0;
	}


	gsl_matrix_free (X);
	gsl_vector_free (y);
	gsl_vector_free (w);
	gsl_vector_free (c);
	gsl_matrix_free (cov);
	return rCurve;
}

QMap< QString, QMap<double, double> > SGMResolutionOptimization::collapse(AMRegionsList *contextParameters){
	QList<QVariant> l1, m1, h1, h3;
	l1 << 250.0 << 0 << 1;
	m1 << 250.0 << 1 << 1;
	h1 << 250.0 << 2 << 1;
	h3 << 250.0 << 2 << 3;
	int numPoints = 50;
	double stepSize = 250/(numPoints-1);
	QMap<double, double> resL1, resM1, resH1, resH3;
	for(double x = stepSize; x < 250; x+=stepSize){
		l1.replace(0, x);
		m1.replace(0, x);
		h1.replace(0, x);
		h3.replace(0, x);

		resL1.insert(x, collapser(curve(l1, contextParameters)));
		resM1.insert(x, collapser(curve(m1, contextParameters)));
		resH1.insert(x, collapser(curve(h1, contextParameters)));
		resH3.insert(x, collapser(curve(h3, contextParameters)));
	}
	QMap< QString, QMap<double, double> > rVal;
	rVal.insert("LEG1", resL1);
	rVal.insert("MEG1", resM1);
	rVal.insert("HEG1", resH1);
	rVal.insert("HEG3", resH3);
	return rVal;
}

double SGMResolutionOptimization::collapser(QMap<double, double> optCurve){
	QMap<double, double>::const_iterator i;
	i = optCurve.constBegin();
	double tmpMin = i.value();
	++i;
	while(i != optCurve.constEnd()){
		if(i.value() < tmpMin)
			tmpMin = i.value();
		++i;
	}
	return tmpMin;
}

double SGMResolutionOptimization::maximumEnergy(AMRegionsList* regions){
	double curMax = 240;
	if(regions->count() == 0)
		return -1;
	for(int x = 0; x < regions->count(); x++){
		curMax = (regions->start(x) > curMax ? regions->start(x) : curMax);
		curMax = (regions->end(x) > curMax ? regions->end(x) : curMax);
	}
	return curMax;
}

double SGMResolutionOptimization::minimumEnergy(AMRegionsList* regions){
	double curMin = 2000;
	if(regions->count() == 0)
		return -1;
	for(int x = 0; x < regions->count(); x++){
		curMin = (regions->start(x) < curMin ? regions->start(x) : curMin);
		curMin = (regions->end(x) < curMin ? regions->end(x) : curMin);
	}
	return curMin;
}
