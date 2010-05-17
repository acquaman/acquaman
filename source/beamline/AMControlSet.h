#ifndef ACQMAN_AMCONTROLSET_H
#define ACQMAN_AMCONTROLSET_H

#include <QObject>
#include <QList>
#include <QMap>
#include <QVariant>
//#include "acquaman/AMScanConfiguration.h"
//#include "acquaman/AMRegion.h"
#include "acquaman/AMRegionsList.h"

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

	/// Returns the name defined for the control set.
	QString name() const { return name_;}
	//const QList<const AMControl*> controls() const { return ctrls_;}
	/// Returns the QList of AMControl pointers that currently comprise the controls in the set.
	QList<AMControl*> controls() { return ctrls_;}

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
	/// Returns a QMap to represent the output to optimize. Can be thought of as x-y pairs for a graph.
	/// The context parameters allow only the necessary region to be returned.
//	virtual QMap<double, double> curve(QList<QVariant> stateParameters, QList<AMRegion*> contextParameters);
	virtual QMap<double, double> curve(QList<QVariant> stateParameters, AMRegionsList* contextParameters);

public slots:
	/// Sets the name of the optimization.
	void setName(const QString &name) { name_ = name;}

protected:
	/// Holds the name of the optimization.
	QString name_;
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

	/// Adds an AMControlOptimization to the set.
	void addOptimization(AMControlOptimization *optimization){ outputs_.append(optimization) ;}
//	QMap<double, double> curveAt(size_t index, QList<QVariant> stateParameters, QList<AMRegion*> contextParameters){
	QMap<double, double> curveAt(size_t index, QList<QVariant> stateParameters, AMRegionsList* contextParameters){
		qDebug() << "Running reg curve on " << index << " who is " << outputs_.at(index)->name();
		return outputs_.at(index)->curve(stateParameters, contextParameters);
	}

protected:
	/// Internal list of AMControlOptimization.
	QList<AMControlOptimization*> outputs_;
};

#endif // ACQMAN_AMCONTROLSET_H
