#ifndef ACQMAN_AMCONTROLSET_H
#define ACQMAN_AMCONTROLSET_H

#define AMCONTROLSET_CONTROL_TIMEOUT_MS 4000

#include <QObject>
#include <QList>
#include <QMap>
#include <QVariant>
//#include "acquaman/AMScanConfiguration.h"
//#include "acquaman/AMRegion.h"
#include "acquaman/AMRegionsList.h"
#include "dataman/AMControlSetInfo.h"
#include "dataman/AMDetectorInfo.h"

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

	AMControlSetInfo* info() { return info_; }

	bool isConnected();

signals:
	void connected(bool groupConnected);

public slots:
	/// Sets the name of the control set.
	void setName(const QString &name);
	/// Adds an AMControl to the control set. Returns true if the addition was successful. Failure could result from adding the same AMControl twice.
	bool addControl(AMControl* ctrl);
	/// Removes an AMControl from the control set. Returns true if the removal was successful. Failure could result from removing an AMControl not in the set.
	bool removeControl(AMControl* ctrl);

//	void syncInfo();
	void setFromInfo(AMControlSetInfo *info);

protected slots:
	void onConnected(bool ctrlConnected);
	void onConnectionsTimedOut();
	void onValueChanged(double value);

protected:
	/// Holds the name of the control set. Should be descriptive of the logical relationship.
	/// AMControlSetView will use this value as the title of the group box being displayed.
	QString name_;
	/// Local list of AMControl pointers, which represent the controls in the set.
	QList<AMControl*> ctrls_;

	AMControlSetInfo *info_;

	bool wasConnected_;
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

	~AMControlOptimizationSet();

	/// Adds an AMControlOptimization to the set.
	void addOptimization(AMControlOptimization *optimization){ outputs_.append(optimization) ;}
	const AMControlOptimization* optimizationAt(int index) const { return outputs_.at(index) ;}
	int optimizationCount() const { return outputs_.count() ;}

	QMap<double, double> curveAt(size_t index, QList<QVariant> stateParameters, AMRegionsList* contextParameters);
	QMap<QString, QMap<double, double> > collapseAt(size_t index, AMRegionsList* contextParameters);
	QMap<QString, AMCurve*> cCollapseAt(size_t index, AMRegionsList* contextParameters);

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


class AMDetectorInfoSet : public QObject
{
Q_OBJECT
public:
	/// Constructor, only needs a QObject to act as a parent.
	explicit AMDetectorInfoSet(QObject *parent = 0);
	//AMDetectorInfoSet(AMDetectorInfoSet *copy, QObject *parent = 0);

	~AMDetectorInfoSet(){
		detectors_.clear();
	}

	/// Returns the name defined for the control set.
	QString name() const { return name_;}
	int count() { return detectors_.count();}
	AMDetectorInfo* detectorAt(int index) { return detectors_.at(index);}
	int indexOf(const QString &name);
	AMDetectorInfo* detectorByName(const QString &name);
	bool isDefaultAt(int index) { return defaultDetectors_.at(index);}
	bool isDefaultByName(const QString &name);

signals:

public slots:
	/// Sets the name of the control set.
	void setName(const QString &name) { name_ = name;}
	/// Adds an AMControl to the control set. Returns true if the addition was successful. Failure could result from adding the same AMControl twice.
	bool addDetector(AMDetectorInfo* detector, bool defaultDetector = false);
	/// Removes an AMControl from the control set. Returns true if the removal was successful. Failure could result from removing an AMControl not in the set.
	bool removeDetector(AMDetectorInfo* detector);

protected:
	/// Holds the name of the control set. Should be descriptive of the logical relationship.
	/// AMControlSetView will use this value as the title of the group box being displayed.
	QString name_;
	/// Local list of AMControl pointers, which represent the controls in the set.
	QList<AMDetectorInfo*> detectors_;
	QList<bool> defaultDetectors_;
};

#endif // ACQMAN_AMCONTROLSET_H
