#ifndef ACQMAN_AMCONTROLSET_H
#define ACQMAN_AMCONTROLSET_H

#include <QObject>
#include <QList>
#include <QMap>
#include <QVariant>
//#include "acquaman/AMScanConfiguration.h"
//#include "acquaman/AMRegion.h"
#include "acquaman/AMRegionsList.h"
#include "AMAbstractDetector.h"

#include <gsl/gsl_multifit.h>

/// An AMControlSet is designed to hold a logical group of controls.
/*!
  Controls that are heirarchically linked should be children of an AMControl. On the other hand, AMControls that are logically linked should be in an AMControlSet.
  For example, beamline energy might be related to several other controls (say mono angle, undulator gap, and exit slit position).
  These are heirarchically linked, so should be children of an energy AMControl.
  This way you could monitor the status of each child to get the status of the control as a whole.
  Conversely, the expected flux of the beamline at any given energy may be several parameters (say the mono grating choice, the exit slit gap, and the harmonic).
  These are logically linked, and should be in an AMControlSet. The convenience is that a large control object can list its logical groups, which can be displayed together or operated on as a group.
  As well, heirarchically linked controls should not likely wind up as a child of more than one control; however, logically grouped controls have no real problem overlapping.

  All that is required to make an AMControlSet is to give the set a name and to add controls by passing a pointer to the AMControl. A function to remove controls is also offered for convenience.
  */
class AMControlSet : public QObject
{
Q_OBJECT
public:
	/// Constructor, only needs a QObject to act as a parent.
	explicit AMControlSet(QObject *parent = 0);

	~AMControlSet(){
		ctrls_.clear();
	}

	/// Returns the name defined for the control set.
	QString name() const { return name_;}
	int count() { return ctrls_.count();}
	AMControl* controlAt(int index) { return ctrls_.at(index);}
	int indexOf(const QString &name);
	AMControl* controlByName(const QString &name);

signals:

public slots:
	/// Sets the name of the control set.
	void setName(const QString &name) { name_ = name;}
	/// Adds an AMControl to the control set. Returns true if the addition was successful. Failure could result from adding the same AMControl twice.
	bool addControl(AMControl* ctrl);
	/// Removes an AMControl from the control set. Returns true if the removal was successful. Failure could result from removing an AMControl not in the set.
	bool removeControl(AMControl* ctrl);

protected:
	/// Holds the name of the control set. Should be descriptive of the logical relationship.
	/// AMControlSetView will use this value as the title of the group box being displayed.
	QString name_;
	/// Local list of AMControl pointers, which represent the controls in the set.
	QList<AMControl*> ctrls_;
};

/// An AMControlOptimization is an abstract class that represents how a single output can vary across a given region for a given state of a system.
/*!
  This class is designed to be subclassed to achieve functionality.
  The curve function can be called, but it returns an empty map. The intention is for subclasses to define their own list of required state parameters.
  See SGMFluxOptimization and SGMResolutionOptimization for examples.
  A name can be defined to identify the optimization.
  The subclasses can implement the curve() function however they like, whether its through an fitting equation, or a predetermined value mapping.

  */
class AMControlOptimization : public QObject
{
	Q_OBJECT
public:
	/// Constructor, only requires a QObject pointer as a parent.
	AMControlOptimization(QObject *parent=0) : QObject(parent){;}

	/// Returns the name of the optimization, likely hardcoded in the subclass.
	QString name() const { return name_;}
	QString description() const { return description_;}
	/// Returns a QMap to represent the output to optimize. Can be thought of as x-y pairs for a graph.
	/// The context parameters allow only the necessary region to be returned.
	virtual QMap<double, double> curve(QList<QVariant> stateParameters, AMRegionsList* contextParameters);
	virtual QMap< QString, QMap<double, double> > collapse(AMRegionsList* contextParameters);

public slots:
	/// Sets the name of the optimization.
	void setName(const QString &name) { name_ = name;}
	void setDescription(const QString &description) { description_ = description;}

protected:
	/// Holds the name of the optimization.
	QString name_;
	QString description_;
};

class AMCurve : public QObject
{
	Q_OBJECT
public:
	AMCurve(QMap<double, double> dataMap, QObject *parent = 0) : QObject(parent){
		hasXDiscontinuities_ = false;
		hasYDiscontinuities_ = false;
		setDataMap(dataMap);
	}

	QPair<double, double> minX() const { return minX_;}

	QPair<double, double> minY() const { return minY_;}

	QPair<double, double> maxX() const { return maxX_;}

	QPair<double, double> maxY() const { return maxY_;}

	void setDataMap(QMap<double, double> dataMap){
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
			//qDebug() << "Inserting " << tmpX << tmpY;
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

	void setMetaMap(QMap<double, QStringList> metaMap){metaMap_ = metaMap;}

	void addXDiscontinuity(QPair<double, double> dis){
		if(!hasXDiscontinuities_)
			hasXDiscontinuities_ = true;
		xDiscontinuities_.append(dis);
	}

	void addYDiscontinuity(QPair<double, double> dis){
		if(!hasYDiscontinuities_)
			hasYDiscontinuities_ = true;
		yDiscontinuities_.append(dis);
	}

	double valueAt(double x) const{
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
					qDebug() << "IN A DISCONTINUITY REGION " << x << " use " << tmpVal;
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

	double valueAtRange(double percent){
		//qDebug() << "Percent forwards to " << minX_.first + percent*(maxX_.first-minX_.first) << " for " << percent;
		return valueAt(minX_.first + percent*(maxX_.first-minX_.first) );
	}

	QPair<double, double> valuesAtRange(double percent){
		QPair<double, double> rVal;
		rVal.first = minX_.first + percent*(maxX_.first-minX_.first);
		rVal.second = valueAt(minX_.first + percent*(maxX_.first-minX_.first) );
		return rVal;
	}

	double percentFromValue(double x) const {
		return (x - minX_.first)/(maxX_.first - minX_.first);
	}

	QMap<double, double> dataMap() const { return dataMap_;}

	QMap<double, QStringList> metaMap() const { return metaMap_;}

	QMap<double, double> transposeMap() const {
		QMap<double, double> tMap;
		QMap<double, double>::const_iterator i = dataMap_.constBegin();
		while(i != dataMap_.constEnd()){
			tMap[i.value()] = i.key();
			++i;
		}
		return tMap;
	}
	QMap<double, QStringList> transposeMetaMap() const {
		QMap<double, QStringList> mtMap;
		QMap<double, double>::const_iterator i = dataMap_.constBegin();
		while(i != dataMap_.constEnd()){
			mtMap[i.value()] = metaMap_.value(i.key());
			++i;
		}
		return mtMap;
	}

	AMCurve* transposeCurve() {
		AMCurve *tmpCurve = new AMCurve(transposeMap());
		tmpCurve->setMetaMap(transposeMetaMap());
		for(int x = 0; x < xDiscontinuities_.count(); x++){
			tmpCurve->addYDiscontinuity(xDiscontinuities_.at(x));
			qDebug() << "Adding as Y " << xDiscontinuities_.at(x).first << xDiscontinuities_.at(x).second;
		}
		for(int x = 0; x < yDiscontinuities_.count(); x++){
			tmpCurve->addXDiscontinuity(yDiscontinuities_.at(x));
			qDebug() << "Adding as X " << yDiscontinuities_.at(x).first << yDiscontinuities_.at(x).second;
		}
		return tmpCurve;
	}

protected:
	QMap<double, double> dataMap_;
	QMap<double, QStringList> metaMap_;
	QPair<double, double> minX_, minY_, maxX_, maxY_;
	bool hasXDiscontinuities_;
	bool hasYDiscontinuities_;
	QList< QPair<double, double> > xDiscontinuities_;
	QList< QPair<double, double> > yDiscontinuities_;
};

/// An AMControlOptimizationSet is a combination of an AMControlSet (its parent class) and a list of AMControlOptimization.
/*!
  The class is designed to hold a list of AMControl (like AMControlSet), as well as the parameter, or parameters, these controls can be used to optimize.
  The true power of this class is to relate multiple optimizable parameters that compete with eachother.
  For example, the beamline flux and the beamline resolution often compete with eachother as increasing one likely decreases the other.
  */
class AMControlOptimizationSet : public AMControlSet
{
	Q_OBJECT
public:
	/// Constructor, only requires a QObject pointer as a parent.
	explicit AMControlOptimizationSet(QObject *parent=0) : AMControlSet(parent){;}

	~AMControlOptimizationSet(){
		outputs_.clear();
	}

	/// Adds an AMControlOptimization to the set.
	void addOptimization(AMControlOptimization *optimization){ outputs_.append(optimization) ;}
	const AMControlOptimization* optimizationAt(int index) const { return outputs_.at(index) ;}
	int optimizationCount() const { return outputs_.count() ;}
	QMap<double, double> curveAt(size_t index, QList<QVariant> stateParameters, AMRegionsList* contextParameters){
		return outputs_.at(index)->curve(stateParameters, contextParameters);
	}
	QMap<QString, QMap<double, double> > collapseAt(size_t index, AMRegionsList* contextParameters){
		return outputs_.at(index)->collapse(contextParameters);
	}
	QMap<QString, QMap<double, double> > plotAgainst(AMRegionsList* contextParameters){
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
	QMap<QString, AMCurve*> cPlotAgainst(AMRegionsList* contextParameters){
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

	double fitMyCubic(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, double input){
		//qDebug() << "Fitting " << x1 << y1 << x2 << y2 << x3 << y3 << x4 << y4 << " for " << input;
		//printf("Fitting %5.2f %5.2f ; %5.2f %5.2f ; %5.2f %5.2f ; %5.2f %5.2f ; for %5.2f\n", x1, y1, x2, y2, x3, y3, x4, y4, input);
		//fflush(stdout);
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

		/*
		printf ("# best fit: Y = %.10g + %.10g X + %.10g X^2 + %.10g X^3\n",
				C(0), C(1), C(2), C(3));

		_fitCoeffsCheck[caller][2] = C(0);
		_fitCoeffsCheck[caller][3] = C(1);
		_fitCoeffsCheck[caller][4] = C(2);
		_fitCoeffsCheck[caller][5] = C(3);
		*/
		double rVal = C(0) + C(1)*input + C(2)*input*input + C(3)*input*input*input;
		gsl_matrix_free (X);
		gsl_vector_free (y);
		gsl_vector_free (w);
		gsl_vector_free (c);
		gsl_matrix_free (cov);
//	}
//		double rVal = _fitCoeffsCheck[caller][2] + _fitCoeffsCheck[caller][3]*energy + _fitCoeffsCheck[caller][4]*energy*energy + _fitCoeffsCheck[caller][5]*energy*energy*energy;
		return rVal;
	}

	/**/
	//QMap<QString, QPair<double, double> > onePlot(AMRegionsList* contextParameters){
	QMap<double, double > onePlot(AMRegionsList* contextParameters){
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
/*
		qDebug() << "Flux Maxes: LEG - " << fMaxLEG.first << "(" << fMaxLEG.second << ") MEG - " << fMaxMEG.first << "(" << fMaxMEG.second
				<< ") HEG1 - " << fMaxHEG1.first << "(" << fMaxHEG1.second << ") HEG3 - " << fMaxHEG3.first << "(" << fMaxHEG3.second << ")\n"
				<< "Resolution Maxes: LEG - " << rMaxLEG.first << "(" << rMaxLEG.second << ") MEG - " << rMaxMEG.first << "(" << rMaxMEG.second
				<< ") HEG1 - " << rMaxHEG1.first << "(" << rMaxHEG1.second << ") HEG3 - " << rMaxHEG3.first << "(" << rMaxHEG3.second << ")";
*/

		QPair<double, double> bestFlux = fMaxLEG;
		if(bestFlux.first < fMaxMEG.first)
			bestFlux = fMaxMEG;
		if(bestFlux.first < fMaxHEG1.first)
			bestFlux = fMaxHEG1;
		if(bestFlux.first < fMaxHEG3.first)
			bestFlux = fMaxHEG3;
		qDebug() << "Best flux is " << bestFlux.first << " at res of " << bestFlux.second;

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
			//qDebug() << "LEG starts as " << nextLEG.key() << nextLEG.value();
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
							//qDebug() << "LEG says single end";
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
						//qDebug() << "LEG says double end";
					}
				}
				else{
					//Case 2, single front
					prevLEG++;
					nextLEG++;
					nnextLEG++;
					//qDebug() << "LEG says single front";
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
				//qDebug() << "LEG says double front";
			}
			if(x <= rMaxLEG.first){
				tmpVal = fitMyCubic(pprevLEG.key(), pprevLEG.value(), prevLEG.key(), prevLEG.value(), nextLEG.key(), nextLEG.value(), nnextLEG.key(), nnextLEG.value(), x);
				if(tmpVal > tmpFlux)
					tmpFlux = tmpVal;
			}


			nextMEG = MEG.lowerBound(x);
			prevMEG = MEG.lowerBound(x);
			//qDebug() << "MEG starts as " << nextMEG.key() << nextMEG.value();
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
							//qDebug() << "MEG says single end";
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
						//qDebug() << "MEG says double end";
					}
				}
				else{
					//Case 2, single front
					pprevMEG++;
					prevMEG++;
					nextMEG++;
					nnextMEG++;
					//qDebug() << "MEG says single front";
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
				//qDebug() << "MEG says double end";
			}
			if(x <= rMaxMEG.first){
				tmpVal = fitMyCubic(pprevMEG.key(), pprevMEG.value(), prevMEG.key(), prevMEG.value(), nextMEG.key(), nextMEG.value(), nnextMEG.key(), nnextMEG.value(), x);
				if(tmpVal > tmpFlux)
					tmpFlux = tmpVal;
			}

			nextHEG1 = HEG1.lowerBound(x);
			prevHEG1 = HEG1.lowerBound(x);
			//qDebug() << "HEG1 starts as " << nextHEG1.key() << nextHEG1.value();
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
							//qDebug() << "HEG1 says single end";
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
						//qDebug() << "HEG1 says double end";
					}
				}
				else{
					//Case 2, single front
					prevHEG1++;
					nextHEG1++;
					nnextHEG1++;
					//qDebug() << "HEG1 says single front";
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
				//qDebug() << "HEG1 says double front";
			}
			if(x <= rMaxHEG1.first){
				tmpVal = fitMyCubic(pprevHEG1.key(), pprevHEG1.value(), prevHEG1.key(), prevHEG1.value(), nextHEG1.key(), nextHEG1.value(), nnextHEG1.key(), nnextHEG1.value(), x);
				if(tmpVal > tmpFlux)
					tmpFlux = tmpVal;
			}

			nextHEG3 = HEG3.lowerBound(x);
			prevHEG3 = HEG3.lowerBound(x);
			//qDebug() << "HEG3 starts as " << nextHEG3.key() << nextHEG3.value();
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
							//qDebug() << "HEG3 says single end";
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
						//qDebug() << "HEG3 says double end";
					}
				}
				else{
					//Case 2, single front
					prevHEG3++;
					nextHEG3++;
					nnextHEG3++;
					//qDebug() << "HEG3 says single front";
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
				//qDebug() << "HEG3 says double front";
			}
			if(x <= rMaxHEG3.first){
				tmpVal = fitMyCubic(pprevHEG3.key(), pprevHEG3.value(), prevHEG3.key(), prevHEG3.value(), nextHEG3.key(), nextHEG3.value(), nnextHEG3.key(), nnextHEG3.value(), x);
				if(tmpVal > tmpFlux)
					tmpFlux = tmpVal;
			}

			//qDebug() << "Want to insert " << x << ", " << tmpFlux;
			rVal.insert(x, tmpFlux);
		}

//		QMap<QString, QPair<double, double> > rVal;
//		rVal.insert("Something", QPair<double, double>(100, 100));
		return rVal;
	}

	AMCurve* cOnePlot(AMRegionsList* contextParameters){
		/*
		QMap<QString, QMap<double, double> > allPlots = plotAgainst(contextParameters);
		AMCurve *cLEG = new AMCurve(allPlots.value("LEG1"));
		AMCurve *cMEG = new AMCurve(allPlots.value("MEG1"));
		AMCurve *cHEG1 = new AMCurve(allPlots.value("HEG1"));
		AMCurve *cHEG3 = new AMCurve(allPlots.value("HEG3"));
		*/
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
		double tmpFlux, tmpVal;
		QStringList tmpStrList, tmpSubList;
		QString tmpGrating, tmpHarmonic;
		for(double x = bestFlux_.second; x < bestRes_.first; x += resStep){
			tmpFlux = 0;
			tmpStrList.clear();
			tmpSubList.clear();
			tmpGrating = "NONE";
			if(x <= cLEG->maxX().first){
				tmpVal = cLEG->valueAt(x);
				if(tmpVal > tmpFlux){
					tmpFlux = tmpVal;
					tmpGrating = "grating|Low";
					tmpHarmonic = "harmonic|First";
					tmpSubList = cLEG->metaMap().lowerBound(x).value();
				}
			}
			if(x <= cMEG->maxX().first){
				tmpVal = cMEG->valueAt(x);
				if(tmpVal > tmpFlux){
					tmpFlux = tmpVal;
					tmpGrating = "grating|Medium";
					tmpHarmonic = "harmonic|First";
					tmpSubList = cMEG->metaMap().lowerBound(x).value();
				}
			}
			if(x <= cHEG1->maxX().first){
				tmpVal = cHEG1->valueAt(x);
				if(tmpVal > tmpFlux){
					tmpFlux = tmpVal;
					tmpGrating = "grating|High";
					tmpHarmonic = "harmonic|First";
					tmpSubList = cHEG1->metaMap().lowerBound(x).value();
				}
			}
			if(x <= cHEG3->maxX().first){
				tmpVal = cHEG3->valueAt(x);
				if(tmpVal > tmpFlux){
					tmpFlux = tmpVal;
					tmpGrating = "grating|High";
					tmpHarmonic = "harmonic|Third";
					tmpSubList = cHEG3->metaMap().lowerBound(x).value();
				}
			}
			//qDebug() << "Want to insert " << x << ", " << tmpFlux;
			tmpStrList.append(tmpGrating);
			tmpStrList.append(tmpHarmonic);
			tmpStrList.append(tmpSubList);
			rMap.insert(x, tmpFlux);
			mMap.insert(x, tmpStrList);
		}

		AMCurve *rVal = new AMCurve(rMap);
		rVal->setMetaMap(mMap);

		for(int x = 1; x < fluxes.count(); x++){
			if( fluxes.at(x)->maxY().second > tFluxes.at(x-1)->valueAt(fluxes.at(x)->maxY().first) )
				rVal->addXDiscontinuity( QPair<double, double>(fluxes.at(x)->maxY().second, tFluxes.at(x-1)->valueAt(fluxes.at(x)->maxY().first)) );
//				qDebug() << "Found resolution discontinuity comparing this (" << x << ")" << fluxes.at(x)->maxY().first << fluxes.at(x)->maxY().second
//						<< fluxes.at(x)->maxY().first << tFluxes.at(x-1)->valueAt(fluxes.at(x)->maxY().first);
		}

		for(int x = 1; x < resolutions.count(); x++){
			//if( resolutions.at(x)->maxX().second > tResolutions.at(x-1)->valueAt(resolutions.at(x)->maxX().first) )
			if( resolutions.at(x)->maxX().second > resolutions.at(x-1)->valueAt(resolutions.at(x)->maxX().first) ){
				rVal->addYDiscontinuity( QPair<double, double>( resolutions.at(x)->maxX().second, resolutions.at(x-1)->valueAt(resolutions.at(x)->maxX().first) ) );
//				qDebug() << "Found flux discontinuity comparing this (" << x << ")" << resolutions.at(x)->maxX().first << resolutions.at(x)->maxX().second
//						<< resolutions.at(x)->maxX().first << resolutions.at(x-1)->valueAt(resolutions.at(x)->maxX().first);
			}
		}

		return rVal;
	}

	QPair<double, double> bestFlux() const { return bestFlux_; }

	QPair<double, double> bestRes() const { return bestRes_; }
protected:
	/// Internal list of AMControlOptimization.
	QList<AMControlOptimization*> outputs_;
	QPair<double, double> bestFlux_, bestRes_;
};


class AMAbstractDetectorSet : public QObject
{
Q_OBJECT
public:
	/// Constructor, only needs a QObject to act as a parent.
	explicit AMAbstractDetectorSet(QObject *parent = 0);

	~AMAbstractDetectorSet(){
		detectors_.clear();
	}

	/// Returns the name defined for the control set.
	QString name() const { return name_;}
	int count() { return detectors_.count();}
	AMAbstractDetector* detectorAt(int index) { return detectors_.at(index);}
	int indexOf(const QString &name);
	AMAbstractDetector* detectorByName(const QString &name);
	bool isDefaultAt(int index) { return defaultDetectors_.at(index);}
	bool isDefaultByName(const QString &name);

signals:

public slots:
	/// Sets the name of the control set.
	void setName(const QString &name) { name_ = name;}
	/// Adds an AMControl to the control set. Returns true if the addition was successful. Failure could result from adding the same AMControl twice.
	bool addDetector(AMAbstractDetector* detector, bool defaultDetector = false);
	/// Removes an AMControl from the control set. Returns true if the removal was successful. Failure could result from removing an AMControl not in the set.
	bool removeDetector(AMAbstractDetector* detector);

protected:
	/// Holds the name of the control set. Should be descriptive of the logical relationship.
	/// AMControlSetView will use this value as the title of the group box being displayed.
	QString name_;
	/// Local list of AMControl pointers, which represent the controls in the set.
	QList<AMAbstractDetector*> detectors_;
	QList<bool> defaultDetectors_;
};

#endif // ACQMAN_AMCONTROLSET_H
