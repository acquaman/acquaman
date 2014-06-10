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


#include "AMControlOptimization.h"

/// Default implementation returns an empty map. This function is the core of the implementation for subclasses.
QMap<double, double> AMControlOptimization::curve(QList<QVariant> stateParameters, AMRegionsList* contextParameters) {
	QMap<double, double> noMap;
	Q_UNUSED(stateParameters);
	Q_UNUSED(contextParameters);
	return noMap;
}

QMap<QString, QMap<double, double> > AMControlOptimization::collapse(AMRegionsList* contextParameters) {
	QMap<QString, QMap<double, double> > noMap;
	Q_UNUSED(contextParameters);
	return noMap;
}

 AMCurve::~AMCurve(){}
AMCurve::AMCurve(QMap<double, double> dataMap, QObject *parent) : QObject(parent){
	hasXDiscontinuities_ = false;
	hasYDiscontinuities_ = false;
	setDataMap(dataMap);
}

void AMCurve::setDataMap(QMap<double, double> dataMap){
	dataMap_ = dataMap;
	QMap<double, double>::const_iterator i = dataMap_.constBegin();
	minX_.first = i.key();
	minX_.second = i.value();
	minY_.first = i.value();
	minY_.second = i.key();
	maxX_.first = i.key();
	maxX_.second = i.value();
	maxY_.first = i.value();
	maxY_.second = i.key();
	double tmpX, tmpY;
	while(i != dataMap_.constEnd()){
		tmpX = i.key();
		tmpY = i.value();
		if(tmpX < minX_.first){
			minX_.first = tmpX;
			minX_.second = tmpY;
		}
		else if(tmpX > maxX_.first){
			maxX_.first = tmpX;
			maxX_.second = tmpY;
		}
		if(tmpY < minY_.first){
			minY_.first = tmpY;
			minY_.second = tmpX;
		}
		else if(tmpY > maxY_.first){
			maxY_.first = tmpY;
			maxY_.second = tmpX;
		}
		++i;
	}
}


void AMCurve::addXDiscontinuity(QPair<double, double> dis){
	if(!hasXDiscontinuities_)
		hasXDiscontinuities_ = true;
	xDiscontinuities_.append(dis);
}

void AMCurve::addYDiscontinuity(QPair<double, double> dis){
	if(!hasYDiscontinuities_)
		hasYDiscontinuities_ = true;
	yDiscontinuities_.append(dis);
}

double AMCurve::valueAt(double x) const{
	if(dataMap_.count() == 0)
		return 0;
	if( (x < minX_.first) || (x > maxX_.first) )
		return 0;
	QMap<double, double>::const_iterator i = dataMap_.lowerBound(x);
	if(hasXDiscontinuities_){
		for(int y = 0; y < xDiscontinuities_.count(); y++)
			if( (xDiscontinuities_.at(y).first > x) && (x > xDiscontinuities_.at(y).second) ){
				double x1, x2, y1, y2, tmpVal;
				x2 = i.key();
				y2 = i.value();
				--i;
				x1 = i.key();
				y1 = i.value();
				tmpVal = ((y2-y1)/(x2-x1))*(x-x2) + y2;
				return tmpVal;
			}
	}
	if(dataMap_.count() <= 3)
		return i.value();
	int seek = 0;
	++i;
	if(i == dataMap_.constEnd())
		seek = -1;
	else{
		--i;
		if(i == dataMap_.constBegin())
			seek = 2;
		else{
			--i;
			if(i == dataMap_.constBegin())
				seek = 1;
		}
	}

	i = dataMap_.lowerBound(x);
	if(seek == -1)
		--i;
	else
		for(int x = 0; x < seek; x++)
			++i;

	--i;
	--i;
	QPair<double, double> first, second, third, fourth;
	first.first = i.key();
	first.second = i.value();
	++i;
	second.first = i.key();
	second.second = i.value();
	++i;
	third.first = i.key();
	third.second = i.value();
	++i;
	fourth.first = i.key();
	fourth.second = i.value();

	int j, n;
	double xi, yi, ei, chisq;
	gsl_matrix *X, *cov;
	gsl_vector *y, *w, *c;

	n = 4;

	X = gsl_matrix_alloc (n, 4);
	y = gsl_vector_alloc (n);
	w = gsl_vector_alloc (n);

	c = gsl_vector_alloc (4);
	cov = gsl_matrix_alloc (4, 4);

	double ix[4];
	double iy[4];
	double ie[4];
	ix[0] = first.first;
	ix[1] = second.first;
	ix[2] = third.first;
	ix[3] = fourth.first;
	iy[0] = first.second;
	iy[1] = second.second;
	iy[2] = third.second;
	iy[3] = fourth.second;
	ie[0] = 0.1*iy[0];
	ie[1] = 0.1*iy[1];
	ie[2] = 0.1*iy[2];
	ie[3] = 0.1*iy[3];
	for (j = 0; j < n; j++)
	{
		xi = ix[j];
		yi = iy[j];
		ei = ie[j];

		gsl_matrix_set (X, j, 0, 1.0);
		gsl_matrix_set (X, j, 1, xi);
		gsl_matrix_set (X, j, 2, xi*xi);
		gsl_matrix_set (X, j, 3, xi*xi*xi);

		gsl_vector_set (y, j, yi);
		gsl_vector_set (w, j, 1.0/(ei*ei*ei));
	}

	gsl_multifit_linear_workspace * work
			= gsl_multifit_linear_alloc (n, 4);
	gsl_multifit_wlinear (X, w, y, c, cov,
						  &chisq, work);
	gsl_multifit_linear_free (work);

#define C(i) (gsl_vector_get(c,(i)))
#define COV(i,j) (gsl_matrix_get(cov,(i),(j)))

	double rVal = C(0) + C(1)*x + C(2)*x*x + C(3)*x*x*x;
	gsl_matrix_free (X);
	gsl_vector_free (y);
	gsl_vector_free (w);
	gsl_vector_free (c);
	gsl_matrix_free (cov);

	return rVal;
}

double AMCurve::valueAtRange(double percent){
	return valueAt(minX_.first + percent*(maxX_.first-minX_.first) );
}

QPair<double, double> AMCurve::valuesAtRange(double percent){
	QPair<double, double> rVal;
	rVal.first = minX_.first + percent*(maxX_.first-minX_.first);
	rVal.second = valueAt(minX_.first + percent*(maxX_.first-minX_.first) );
	return rVal;
}

double AMCurve::percentFromValue(double x) const {
	return (x - minX_.first)/(maxX_.first - minX_.first);
}

QStringList AMCurve::metaAt(double x) const {
	QStringList rVal;
	QMap<double, QStringList>::const_iterator ii = metaMap_.lowerBound(x);
	QString tmpSlit;
	double newSlit, x1, x2, y1, y2;
	QString tmpName, tmpGrating, tmpHarmonic;
	if(ii == metaMap_.constBegin()){
		rVal = ii.value();
		tmpName = ii.value().at(2).split("|").at(0)+"|";
		newSlit = ii.value().at(2).split("|").at(1).toDouble();
	}
	else{
		if(ii == metaMap_.constEnd())
			--ii;
		rVal = ii.value();
		tmpName = ii.value().at(2).split("|").at(0)+"|";
		tmpGrating = ii.value().at(0).split("|").at(1);
		tmpHarmonic = ii.value().at(1).split("|").at(1);
		x1 = ii.key();
		y1 = ii.value().at(2).split("|").at(1).toDouble();
		--ii;
		if(tmpGrating != ii.value().at(0).split("|").at(1)){
			rVal.replace(0, ii.value().at(0).split("|").at(0)+"|SWITCH|"+ii.value().at(0).split("|").at(2));
		}
		if(tmpHarmonic != ii.value().at(1).split("|").at(1)){
			rVal.replace(1, ii.value().at(1).split("|").at(0)+"|SWITCH|"+ii.value().at(1).split("|").at(2));
		}
		x2 = ii.key();
		y2 = ii.value().at(2).split("|").at(1).toDouble();
		newSlit = ((y2-y1)/(x2-x1))*(x-x2)+y2;
	}
	tmpSlit.setNum(newSlit);
	tmpSlit.prepend(tmpName);
	tmpSlit.append("|c");
	rVal.replace(2, tmpSlit);

	return rVal;
}

QMap<double, double> AMCurve::transposeMap() const {
	QMap<double, double> tMap;
	QMap<double, double>::const_iterator i = dataMap_.constBegin();
	while(i != dataMap_.constEnd()){
		tMap[i.value()] = i.key();
		++i;
	}
	return tMap;
}
QMap<double, QStringList> AMCurve::transposeMetaMap() const {
	QMap<double, QStringList> mtMap;
	QMap<double, double>::const_iterator i = dataMap_.constBegin();
	while(i != dataMap_.constEnd()){
		mtMap[i.value()] = metaMap_.value(i.key());
		++i;
	}
	return mtMap;
}

AMCurve* AMCurve::transposeCurve() {
	AMCurve *tmpCurve = new AMCurve(transposeMap());
	tmpCurve->setMetaMap(transposeMetaMap());
	for(int x = 0; x < xDiscontinuities_.count(); x++){
		tmpCurve->addYDiscontinuity(xDiscontinuities_.at(x));
	}
	for(int x = 0; x < yDiscontinuities_.count(); x++){
		tmpCurve->addXDiscontinuity(yDiscontinuities_.at(x));
	}
	return tmpCurve;
}

AMControlOptimizationSet::~AMControlOptimizationSet(){
	outputs_.clear();
}

QMap<double, double> AMControlOptimizationSet::curveAt(int index, QList<QVariant> stateParameters, AMRegionsList* contextParameters){
	return outputs_.at(index)->curve(stateParameters, contextParameters);
}
QMap<QString, QMap<double, double> > AMControlOptimizationSet::collapseAt(int index, AMRegionsList* contextParameters){
	return outputs_.at(index)->collapse(contextParameters);
}
QMap<QString, AMCurve*> AMControlOptimizationSet::cCollapseAt(int index, AMRegionsList* contextParameters){
	QMap<QString, AMCurve*> rVal;
	AMCurve *tmpCurve;
	QMap<QString, QMap<double, double> > cMaps = collapseAt(index, contextParameters);
	QMap<QString, QMap<double, double> >::const_iterator i = cMaps.constBegin();
	while(i != cMaps.constEnd()){
		tmpCurve = new AMCurve(i.value());
		rVal.insert(i.key(), tmpCurve);
		++i;
	}
	return rVal;
}

QMap<QString, QMap<double, double> > AMControlOptimizationSet::plotAgainst(AMRegionsList* contextParameters){
	QMap<QString, QMap<double, double> > fluxes, resolutions;
	QMap<double, double> LEG, MEG, HEG1, HEG3;
	fluxes = collapseAt(0, contextParameters);
	resolutions = collapseAt(1, contextParameters);
	QMap<double, double>::const_iterator i;
	i = fluxes.value("LEG1").constBegin();
	while(i != fluxes.value("LEG1").constEnd()){
		LEG.insert(resolutions.value("LEG1").value(i.key()), fluxes.value("LEG1").value(i.key()));
		MEG.insert(resolutions.value("MEG1").value(i.key()), fluxes.value("MEG1").value(i.key()));
		HEG1.insert(resolutions.value("HEG1").value(i.key()), fluxes.value("HEG1").value(i.key()));
		HEG3.insert(resolutions.value("HEG3").value(i.key()), fluxes.value("HEG3").value(i.key()));
		++i;
	}
	QMap<QString, QMap<double, double> > rVal;
	rVal.insert("LEG1", LEG);
	rVal.insert("MEG1", MEG);
	rVal.insert("HEG1", HEG1);
	rVal.insert("HEG3", HEG3);
	return rVal;
}
QMap<QString, AMCurve*> AMControlOptimizationSet::cPlotAgainst(AMRegionsList* contextParameters){
	QMap<QString, QMap<double, double> > fluxes, resolutions;
	QMap<double, double> LEG, MEG, HEG1, HEG3;
	QMap<double, QStringList> mLEG, mMEG, mHEG1, mHEG3;
	fluxes = collapseAt(0, contextParameters);
	resolutions = collapseAt(1, contextParameters);
	QMap<double, double>::const_iterator i;
	i = fluxes.value("LEG1").constBegin();
	QString tmpStr;
	QStringList tmpStrList;
	while(i != fluxes.value("LEG1").constEnd()){
		tmpStr.clear();
		tmpStrList.clear();
		LEG.insert(resolutions.value("LEG1").value(i.key()), fluxes.value("LEG1").value(i.key()));
		MEG.insert(resolutions.value("MEG1").value(i.key()), fluxes.value("MEG1").value(i.key()));
		HEG1.insert(resolutions.value("HEG1").value(i.key()), fluxes.value("HEG1").value(i.key()));
		HEG3.insert(resolutions.value("HEG3").value(i.key()), fluxes.value("HEG3").value(i.key()));
		tmpStr.setNum(i.key());
		tmpStr.prepend("exitSlitGap|");
		tmpStr.append("|c");
		tmpStrList.append(tmpStr);
		mLEG.insert(resolutions.value("LEG1").value(i.key()), tmpStrList);
		mMEG.insert(resolutions.value("MEG1").value(i.key()), tmpStrList);
		mHEG1.insert(resolutions.value("HEG1").value(i.key()), tmpStrList);
		mHEG3.insert(resolutions.value("HEG3").value(i.key()), tmpStrList);
		++i;
	}
	//QMap<QString, QMap<double, double> > rVal;
	QMap<QString, AMCurve*> rVal;
	AMCurve *cLEG = new AMCurve(LEG);
	cLEG->setMetaMap(mLEG);
	AMCurve *cMEG = new AMCurve(MEG);
	cMEG->setMetaMap(mMEG);
	AMCurve *cHEG1 = new AMCurve(HEG1);
	cHEG1->setMetaMap(mHEG1);
	AMCurve *cHEG3 = new AMCurve(HEG3);
	cHEG3->setMetaMap(mHEG3);
	rVal.insert("LEG1", cLEG);
	rVal.insert("MEG1", cMEG);
	rVal.insert("HEG1", cHEG1);
	rVal.insert("HEG3", cHEG3);
	return rVal;
}

double AMControlOptimizationSet::fitMyCubic(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, double input){
	int i, n;
	double xi, yi, ei, chisq;
	gsl_matrix *X, *cov;
	gsl_vector *y, *w, *c;

	n = 4;

	X = gsl_matrix_alloc (n, 4);
	y = gsl_vector_alloc (n);
	w = gsl_vector_alloc (n);

	c = gsl_vector_alloc (4);
	cov = gsl_matrix_alloc (4, 4);

	double ix[4];
	double iy[4];
	double ie[4];
	ix[0] = x1;
	ix[1] = x2;
	ix[2] = x3;
	ix[3] = x4;
	iy[0] = y1;
	iy[1] = y2;
	iy[2] = y3;
	iy[3] = y4;
	ie[0] = 0.1*iy[0];
	ie[1] = 0.1*iy[1];
	ie[2] = 0.1*iy[2];
	ie[3] = 0.1*iy[3];
	for (i = 0; i < n; i++)
	{
		xi = ix[i];
		yi = iy[i];
		ei = ie[i];

		gsl_matrix_set (X, i, 0, 1.0);
		gsl_matrix_set (X, i, 1, xi);
		gsl_matrix_set (X, i, 2, xi*xi);
		gsl_matrix_set (X, i, 3, xi*xi*xi);

		gsl_vector_set (y, i, yi);
		gsl_vector_set (w, i, 1.0/(ei*ei*ei));
	}

	gsl_multifit_linear_workspace * work
			= gsl_multifit_linear_alloc (n, 4);
	gsl_multifit_wlinear (X, w, y, c, cov,
						  &chisq, work);
	gsl_multifit_linear_free (work);

#define C(i) (gsl_vector_get(c,(i)))
#define COV(i,j) (gsl_matrix_get(cov,(i),(j)))

	double rVal = C(0) + C(1)*input + C(2)*input*input + C(3)*input*input*input;
	gsl_matrix_free (X);
	gsl_vector_free (y);
	gsl_vector_free (w);
	gsl_vector_free (c);
	gsl_matrix_free (cov);
	return rVal;
}

QMap<double, double > AMControlOptimizationSet::onePlot(AMRegionsList* contextParameters){
	QMap<QString, QMap<double, double> > allPlots = plotAgainst(contextParameters);
	QMap<double, double>::const_iterator l, m, h, hh;
	l = allPlots.value("LEG1").constBegin();
	m = allPlots.value("MEG1").constBegin();
	h = allPlots.value("HEG1").constBegin();
	hh = allPlots.value("HEG3").constBegin();
	QPair<double, double> fMaxLEG, fMaxMEG, fMaxHEG1, fMaxHEG3, rMaxLEG, rMaxMEG, rMaxHEG1, rMaxHEG3;
	rMaxLEG = QPair<double, double>(l.key(), l.value());
	rMaxMEG = QPair<double, double>(m.key(), m.value());
	rMaxHEG1 = QPair<double, double>(h.key(), h.value());
	rMaxHEG3 = QPair<double, double>(hh.key(), hh.value());
	fMaxLEG = QPair<double, double>(l.value(), l.key());
	fMaxMEG = QPair<double, double>(m.value(), m.key());
	fMaxHEG1 = QPair<double, double>(h.value(), h.key());
	fMaxHEG3 = QPair<double, double>(hh.value(), hh.key());
	++l;
	++m;
	++h;
	++hh;
	while(l != allPlots.value("LEG1").constEnd() ){
		if(rMaxLEG.first < l.key()){
			rMaxLEG.first = l.key();
			rMaxLEG.second = l.value();
		}
		if(fMaxLEG.first < l.value()){
			fMaxLEG.first = l.value();
			fMaxLEG.second = l.key();
		}
		++l;
	}
	while(m != allPlots.value("MEG1").constEnd() ){
		if(rMaxMEG.first < m.key()){
			rMaxMEG.first = m.key();
			rMaxMEG.second = m.value();
		}
		if(fMaxMEG.first < m.value()){
			fMaxMEG.first = m.value();
			fMaxMEG.second = m.key();
		}
		++m;
	}
	while(h != allPlots.value("HEG1").constEnd() ){
		if(rMaxHEG1.first < h.key()){
			rMaxHEG1.first = h.key();
			rMaxHEG1.second = h.value();
		}
		if(fMaxHEG1.first < h.value()){
			fMaxHEG1.first = h.value();
			fMaxHEG1.second = h.key();
		}
		++h;
	}
	while(hh != allPlots.value("HEG3").constEnd() ){
		if(rMaxHEG3.first < hh.key()){
			rMaxHEG3.first = hh.key();
			rMaxHEG3.second = hh.value();
		}
		if(fMaxHEG3.first < hh.value()){
			fMaxHEG3.first = hh.value();
			fMaxHEG3.second = hh.key();
		}
		++hh;
	}
	if(rMaxLEG.first < 1e-100)
		rMaxLEG.first = 0;
	if(rMaxMEG.first < 1e-100)
		rMaxMEG.first = 0;
	if(rMaxHEG1.first < 1e-100)
		rMaxHEG1.first = 0;
	if(rMaxHEG3.first < 1e-100)
		rMaxHEG3.first = 0;
	if(fMaxLEG.first < 1e-100)
		fMaxLEG.first = 0;
	if(fMaxMEG.first < 1e-100)
		fMaxMEG.first = 0;
	if(fMaxHEG1.first < 1e-100)
		fMaxHEG1.first = 0;
	if(fMaxHEG3.first < 1e-100)
		fMaxHEG3.first = 0;

	QPair<double, double> bestFlux = fMaxLEG;
	if(bestFlux.first < fMaxMEG.first)
		bestFlux = fMaxMEG;
	if(bestFlux.first < fMaxHEG1.first)
		bestFlux = fMaxHEG1;
	if(bestFlux.first < fMaxHEG3.first)
		bestFlux = fMaxHEG3;

	QPair<double, double> bestRes = rMaxLEG;
	if(bestRes.first < rMaxMEG.first)
		bestRes = rMaxMEG;
	if(bestRes.first < rMaxHEG1.first)
		bestRes = rMaxHEG1;
	if(bestRes.first < rMaxHEG3.first)
		bestRes = rMaxHEG3;

	bestFlux_ = bestFlux;
	bestRes_ = bestRes;

	QMap<double, double> LEG, MEG, HEG1, HEG3;
	QMap<double, double> rVal;
	LEG = allPlots.value("LEG1");
	MEG = allPlots.value("MEG1");
	HEG1 = allPlots.value("HEG1");
	HEG3 = allPlots.value("HEG3");
	QMap<double, double>::const_iterator prevLEG, nextLEG, pprevLEG, nnextLEG;
	QMap<double, double>::const_iterator prevMEG, nextMEG, pprevMEG, nnextMEG;
	QMap<double, double>::const_iterator prevHEG1, nextHEG1, pprevHEG1, nnextHEG1;
	QMap<double, double>::const_iterator prevHEG3, nextHEG3, pprevHEG3, nnextHEG3;
	double resStep = (bestRes.first - bestFlux.second)/100;
	double tmpFlux, tmpVal;
	for(double x = bestFlux.second; x < bestRes.first; x += resStep){
		tmpFlux = 0;
		nextLEG = LEG.lowerBound(x);
		prevLEG = LEG.lowerBound(x);
		nnextLEG = nextLEG;
		if(prevLEG != LEG.constBegin()){
			prevLEG--;
			pprevLEG = prevLEG;
			if(pprevLEG != LEG.constBegin()){
				pprevLEG--;
				if(nextLEG != LEG.constEnd()){
					nnextLEG++;
					if(nnextLEG == LEG.constEnd()){
						//Case 4, single end
						nnextLEG--;
						nextLEG--;
						prevLEG--;
						pprevLEG--;
					}
				}
				else{
					//Case 3, double end
					nnextLEG--;
					nnextLEG--;
					nextLEG--;
					nextLEG--;
					prevLEG--;
					prevLEG--;
					pprevLEG--;
					pprevLEG--;
				}
			}
			else{
				//Case 2, single front
				prevLEG++;
				nextLEG++;
				nnextLEG++;
			}
		}
		else{
			//Case 1, double front
			pprevLEG = prevLEG;
			prevLEG++;
			nextLEG++;
			nextLEG++;
			nnextLEG++;
			nnextLEG++;
		}
		if(x <= rMaxLEG.first){
			tmpVal = fitMyCubic(pprevLEG.key(), pprevLEG.value(), prevLEG.key(), prevLEG.value(), nextLEG.key(), nextLEG.value(), nnextLEG.key(), nnextLEG.value(), x);
			if(tmpVal > tmpFlux)
				tmpFlux = tmpVal;
		}


		nextMEG = MEG.lowerBound(x);
		prevMEG = MEG.lowerBound(x);
		nnextMEG = nextMEG;
		if(prevMEG != MEG.constBegin()){
			prevMEG--;
			pprevMEG = prevMEG;
			if(pprevMEG != MEG.constBegin()){
				pprevMEG--;
				if(nextMEG != MEG.constEnd()){
					nnextMEG++;
					if(nnextMEG == MEG.constEnd()){
						//Case 4, single end
						nnextMEG--;
						nextMEG--;
						prevMEG--;
						pprevMEG--;
					}
				}
				else{
					//Case 3, double end
					nnextMEG--;
					nnextMEG--;
					nextMEG--;
					nextMEG--;
					prevMEG--;
					prevMEG--;
					pprevMEG--;
					pprevMEG--;
				}
			}
			else{
				//Case 2, single front
				pprevMEG++;
				prevMEG++;
				nextMEG++;
				nnextMEG++;
			}
		}
		else{
			//Case 1, double front
			pprevMEG = prevMEG;
			prevMEG++;
			nextMEG++;
			nextMEG++;
			nnextMEG++;
			nnextMEG++;
		}
		if(x <= rMaxMEG.first){
			tmpVal = fitMyCubic(pprevMEG.key(), pprevMEG.value(), prevMEG.key(), prevMEG.value(), nextMEG.key(), nextMEG.value(), nnextMEG.key(), nnextMEG.value(), x);
			if(tmpVal > tmpFlux)
				tmpFlux = tmpVal;
		}

		nextHEG1 = HEG1.lowerBound(x);
		prevHEG1 = HEG1.lowerBound(x);
		nnextHEG1 = nextHEG1;
		if(prevHEG1 != HEG1.constBegin()){
			prevHEG1--;
			pprevHEG1 = prevHEG1;
			if(pprevHEG1 != HEG1.constBegin()){
				pprevHEG1--;
				if(nextHEG1 != HEG1.constEnd()){
					nnextHEG1++;
					if(nnextHEG1 == HEG1.constEnd()){
						//Case 4, single end
						nnextHEG1--;
						nextHEG1--;
						prevHEG1--;
						pprevHEG1--;
					}
				}
				else{
					//Case 3, double end
					nnextHEG1--;
					nnextHEG1--;
					nextHEG1--;
					nextHEG1--;
					prevHEG1--;
					prevHEG1--;
					pprevHEG1--;
					pprevHEG1--;
				}
			}
			else{
				//Case 2, single front
				prevHEG1++;
				nextHEG1++;
				nnextHEG1++;
			}
		}
		else{
			//Case 1, double front
			pprevHEG1 = prevHEG1;
			prevHEG1++;
			nextHEG1++;
			nextHEG1++;
			nnextHEG1++;
			nnextHEG1++;
		}
		if(x <= rMaxHEG1.first){
			tmpVal = fitMyCubic(pprevHEG1.key(), pprevHEG1.value(), prevHEG1.key(), prevHEG1.value(), nextHEG1.key(), nextHEG1.value(), nnextHEG1.key(), nnextHEG1.value(), x);
			if(tmpVal > tmpFlux)
				tmpFlux = tmpVal;
		}

		nextHEG3 = HEG3.lowerBound(x);
		prevHEG3 = HEG3.lowerBound(x);
		nnextHEG3 = nextHEG3;
		if(prevHEG3 != HEG3.constBegin()){
			prevHEG3--;
			pprevHEG3 = prevHEG3;
			if(pprevHEG3 != HEG3.constBegin()){
				pprevHEG3--;
				if(nextHEG3 != HEG3.constEnd()){
					nnextHEG3++;
					if(nnextHEG3 == HEG3.constEnd()){
						//Case 4, single end
						nnextHEG3--;
						nextHEG3--;
						prevHEG3--;
						pprevHEG3--;
					}
				}
				else{
					//Case 3, double end
					nnextHEG3--;
					nnextHEG3--;
					nextHEG3--;
					nextHEG3--;
					prevHEG3--;
					prevHEG3--;
					pprevHEG3--;
					pprevHEG3--;
				}
			}
			else{
				//Case 2, single front
				prevHEG3++;
				nextHEG3++;
				nnextHEG3++;
			}
		}
		else{
			//Case 1, double front
			pprevHEG3 = prevHEG3;
			prevHEG3++;
			nextHEG3++;
			nextHEG3++;
			nnextHEG3++;
			nnextHEG3++;
		}
		if(x <= rMaxHEG3.first){
			tmpVal = fitMyCubic(pprevHEG3.key(), pprevHEG3.value(), prevHEG3.key(), prevHEG3.value(), nextHEG3.key(), nextHEG3.value(), nnextHEG3.key(), nnextHEG3.value(), x);
			if(tmpVal > tmpFlux)
				tmpFlux = tmpVal;
		}

		rVal.insert(x, tmpFlux);
	}
	return rVal;
}

AMCurve* AMControlOptimizationSet::cOnePlot(AMRegionsList* contextParameters){
	QMap<QString, AMCurve*> allCurves = cPlotAgainst(contextParameters);
	AMCurve *cLEG = allCurves.value("LEG1");
	AMCurve *cMEG = allCurves.value("MEG1");
	AMCurve *cHEG1 = allCurves.value("HEG1");
	AMCurve *cHEG3 = allCurves.value("HEG3");

	AMCurve *tLEG = new AMCurve(cLEG->transposeMap());
	AMCurve *tMEG = new AMCurve(cMEG->transposeMap());
	AMCurve *tHEG1 = new AMCurve(cHEG1->transposeMap());
	AMCurve *tHEG3 = new AMCurve(cHEG3->transposeMap());

	QList<AMCurve*> fluxes;
	QList<AMCurve*> resolutions;
	fluxes.append(cLEG);
	fluxes.append(cMEG);
	fluxes.append(cHEG1);
	fluxes.append(cHEG3);
	resolutions.append(cLEG);
	resolutions.append(cMEG);
	resolutions.append(cHEG1);
	resolutions.append(cHEG3);

	QList<AMCurve*> tFluxes;
	QList<AMCurve*> tResolutions;
	tFluxes.append(tLEG);
	tFluxes.append(tMEG);
	tFluxes.append(tHEG1);
	tFluxes.append(tHEG3);
	tResolutions.append(tLEG);
	tResolutions.append(tMEG);
	tResolutions.append(tHEG1);
	tResolutions.append(tHEG3);

	for(int x = 0; x < 3; x++){
		if( fluxes.at(0)->maxY().first < fluxes.at(1)->maxY().first ){
			fluxes.swap(0,1);
			tFluxes.swap(0,1);
		}
		if( fluxes.at(2)->maxY().first < fluxes.at(3)->maxY().first ){
			fluxes.swap(2,3);
			tFluxes.swap(2,3);
		}
		if( resolutions.at(0)->maxX().first < resolutions.at(1)->maxX().first ){
			resolutions.swap(0,1);
			tResolutions.swap(0,1);
		}
		if( resolutions.at(2)->maxX().first < resolutions.at(3)->maxX().first ){
			resolutions.swap(2,3);
			tResolutions.swap(2,3);
		}
	}
	bestFlux_ = fluxes.at(0)->maxY();
	bestRes_ = resolutions.at(0)->maxX();

	QMap<double, double> rMap;
	QMap<double, QStringList> mMap;
	double resStep = (bestRes_.first - bestFlux_.second)/100;
	double tmpFlux, tmpVal, newSlit;
	QStringList tmpStrList, tmpSubList;
	QString tmpGrating, tmpHarmonic, tmpSlit, tmpName;
	QMap<double, QStringList> lMetas;
	QMap<double, QStringList>::const_iterator ii;
	double x1, x2, y1, y2;

	for(double x = bestFlux_.second; x < bestRes_.first; x += resStep){
		tmpFlux = 0;
		tmpStrList.clear();
		tmpSubList.clear();
		tmpGrating = "NONE";
		if(x <= cLEG->maxX().first){
			tmpVal = cLEG->valueAt(x);
			if(tmpVal > tmpFlux){
				tmpFlux = tmpVal;
				tmpGrating = "grating|Low|d";
				tmpHarmonic = "harmonic|First|d";
				lMetas = cLEG->metaMap();
				ii = lMetas.lowerBound(x);
				newSlit = 0;
				tmpName = ii.value().at(0).split("|").at(0)+"|";
				if(ii == lMetas.constBegin())
					newSlit = ii.value().at(0).split("|").at(1).toDouble();
				else{
					if(ii == lMetas.constEnd())
						--ii;

					x1 = ii.key();
					y1 = ii.value().at(0).split("|").at(1).toDouble();
					--ii;
					x2 = ii.key();
					y2 = ii.value().at(0).split("|").at(1).toDouble();
					newSlit = ((y2-y1)/(x2-x1))*(x-x2)+y2;
				}
				tmpSlit.setNum(newSlit);
				tmpSlit.prepend(tmpName);
				tmpSlit.append("|c");
			}
		}
		if(x <= cMEG->maxX().first){
			tmpVal = cMEG->valueAt(x);
			if(tmpVal > tmpFlux){
				tmpFlux = tmpVal;
				tmpGrating = "grating|Medium|d";
				tmpHarmonic = "harmonic|First|d";

				lMetas = cMEG->metaMap();
				ii = lMetas.lowerBound(x);
				newSlit = 0;
				tmpName = ii.value().at(0).split("|").at(0)+"|";
				if(ii == lMetas.constBegin())
					newSlit = ii.value().at(0).split("|").at(1).toDouble();
				else{
					if(ii == lMetas.constEnd())
						--ii;
					x1 = ii.key();
					y1 = ii.value().at(0).split("|").at(1).toDouble();
					--ii;
					x2 = ii.key();
					y2 = ii.value().at(0).split("|").at(1).toDouble();
					newSlit = ((y2-y1)/(x2-x1))*(x-x2)+y2;
				}
				tmpSlit.setNum(newSlit);
				tmpSlit.prepend(tmpName);
				tmpSlit.append("|c");
			}
		}
		if(x <= cHEG1->maxX().first){
			tmpVal = cHEG1->valueAt(x);
			if(tmpVal > tmpFlux){
				tmpFlux = tmpVal;
				tmpGrating = "grating|High|d";
				tmpHarmonic = "harmonic|First|d";
				lMetas = cHEG1->metaMap();
				ii = lMetas.lowerBound(x);
				newSlit = 0;
				tmpName = ii.value().at(0).split("|").at(0)+"|";
				if(ii == lMetas.constBegin())
					newSlit = ii.value().at(0).split("|").at(1).toDouble();
				else{
					if(ii == lMetas.constEnd())
						--ii;
					x1 = ii.key();
					y1 = ii.value().at(0).split("|").at(1).toDouble();
					--ii;
					x2 = ii.key();
					y2 = ii.value().at(0).split("|").at(1).toDouble();
					newSlit = ((y2-y1)/(x2-x1))*(x-x2)+y2;
				}
				tmpSlit.setNum(newSlit);
				tmpSlit.prepend(tmpName);
				tmpSlit.append("|c");
			}
		}
		if(x <= cHEG3->maxX().first){
			tmpVal = cHEG3->valueAt(x);
			if(tmpVal > tmpFlux){
				tmpFlux = tmpVal;
				tmpGrating = "grating|High|d";
				tmpHarmonic = "harmonic|Third|d";

				lMetas = cHEG3->metaMap();
				ii = lMetas.lowerBound(x);
				newSlit = 0;
				tmpName = ii.value().at(0).split("|").at(0)+"|";
				if(ii == lMetas.constBegin())
					newSlit = ii.value().at(0).split("|").at(1).toDouble();
				else{
					if(ii == lMetas.constEnd())
						--ii;
					x1 = ii.key();
					y1 = ii.value().at(0).split("|").at(1).toDouble();
					--ii;
					x2 = ii.key();
					y2 = ii.value().at(0).split("|").at(1).toDouble();
					newSlit = ((y2-y1)/(x2-x1))*(x-x2)+y2;
				}
				tmpSlit.setNum(newSlit);
				tmpSlit.prepend(tmpName);
				tmpSlit.append("|c");
			}
		}
		tmpStrList.append(tmpGrating);
		tmpStrList.append(tmpHarmonic);
		tmpStrList.append(tmpSlit);
		rMap.insert(x, tmpFlux);
		mMap.insert(x, tmpStrList);
	}

	AMCurve *rVal = new AMCurve(rMap);
	rVal->setMetaMap(mMap);

	for(int x = 1; x < fluxes.count(); x++){
		if( fluxes.at(x)->maxY().second > tFluxes.at(x-1)->valueAt(fluxes.at(x)->maxY().first) )
			rVal->addXDiscontinuity( QPair<double, double>(fluxes.at(x)->maxY().second, tFluxes.at(x-1)->valueAt(fluxes.at(x)->maxY().first)) );
	}

	for(int x = 1; x < resolutions.count(); x++){
		if( resolutions.at(x)->maxX().second > resolutions.at(x-1)->valueAt(resolutions.at(x)->maxX().first) ){
			rVal->addYDiscontinuity( QPair<double, double>( resolutions.at(x)->maxX().second, resolutions.at(x-1)->valueAt(resolutions.at(x)->maxX().first) ) );
		}
	}

	return rVal;
}

