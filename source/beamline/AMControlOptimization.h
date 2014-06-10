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


#ifndef AMCONTROLOPTIMIZATION_H
#define AMCONTROLOPTIMIZATION_H

#include <QObject>
#include <QList>
#include <QMap>
#include <QVariant>
#include "acquaman/AMRegionsList.h"
#include "beamline/AMControlSet.h"
#include <gsl/gsl_multifit.h>

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
 	virtual ~AMCurve();
	AMCurve(QMap<double, double> dataMap, QObject *parent = 0);

	QPair<double, double> minX() const { return minX_;}
	QPair<double, double> minY() const { return minY_;}
	QPair<double, double> maxX() const { return maxX_;}
	QPair<double, double> maxY() const { return maxY_;}

	void setDataMap(QMap<double, double> dataMap);
	void setMetaMap(QMap<double, QStringList> metaMap){metaMap_ = metaMap;}
	void addXDiscontinuity(QPair<double, double> dis);
	void addYDiscontinuity(QPair<double, double> dis);

	double valueAt(double x) const;
	double valueAtRange(double percent);
	QPair<double, double> valuesAtRange(double percent);
	double percentFromValue(double x) const;
	QStringList metaAt(double x) const;

	QMap<double, double> dataMap() const { return dataMap_;}
	QMap<double, QStringList> metaMap() const { return metaMap_;}
	QMap<double, double> transposeMap() const;
	QMap<double, QStringList> transposeMetaMap() const;
	AMCurve* transposeCurve();

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

	virtual ~AMControlOptimizationSet();

	/// Adds an AMControlOptimization to the set.
	void addOptimization(AMControlOptimization *optimization){ outputs_.append(optimization) ;}
	const AMControlOptimization* optimizationAt(int index) const { return outputs_.at(index) ;}
	int optimizationCount() const { return outputs_.count() ;}

	QMap<double, double> curveAt(int index, QList<QVariant> stateParameters, AMRegionsList* contextParameters);
	QMap<QString, QMap<double, double> > collapseAt(int index, AMRegionsList* contextParameters);
	QMap<QString, AMCurve*> cCollapseAt(int index, AMRegionsList* contextParameters);

	QMap<QString, QMap<double, double> > plotAgainst(AMRegionsList* contextParameters);
	QMap<QString, AMCurve*> cPlotAgainst(AMRegionsList* contextParameters);

	double fitMyCubic(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, double input);

	QMap<double, double > onePlot(AMRegionsList* contextParameters);
	AMCurve* cOnePlot(AMRegionsList* contextParameters);

	QPair<double, double> bestFlux() const { return bestFlux_; }
	QPair<double, double> bestRes() const { return bestRes_; }

protected:
	/// Internal list of AMControlOptimization.
	QList<AMControlOptimization*> outputs_;
	QPair<double, double> bestFlux_, bestRes_;
};


#endif // AMCONTROLOPTIMIZATION_H
