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


#ifndef AM2DSCANCONFIGURATION_H
#define AM2DSCANCONFIGURATION_H

#include "acquaman/AMScanConfiguration.h"
#include "util/AMRange.h"

/// An AM2DScanConfiguration is the parent class for any scan that wants to implement a simple rectangular 2D map.
/*!
	The AM2DScanConfiguration class is designed to allow beamline specific implementation classes
	to inherit it, but take advantage of a common method for configuring some key features associated
	with the scan.

	Regardless of the type of map being implemented there will always be a x-axis control and a
	y-axis control.  By the nature of a map, one will be defined as the "slow axis" and the other
	as the "fast axis".  These represent which part of the map is changed each point.  The fast
	axis will change after every point where as the slow axis will only change after the end of
	the line of the fast axis.

	Some default information is required is:
		- x range
		- y range
		- step size for x
		- step size for y
		- time per point

	What is actually being scanned can be handled by the implementation classes.

	Note, this is a pure virtual class and cannot be instantiated.  The methods for the
	name and units for each axis were chosen as the methods to be explicitly implemented
	due to the immense possibilities for 2D maps.
  */
class AM2DScanConfiguration : public AMScanConfiguration
{
	Q_OBJECT

	Q_PROPERTY(double xStart READ xStart WRITE setXStart)
	Q_PROPERTY(double xStep READ xStep WRITE setXStep)
	Q_PROPERTY(double xEnd READ xEnd WRITE setXEnd)
	Q_PROPERTY(double yStart READ yStart WRITE setYStart)
	Q_PROPERTY(double yStep READ yStep WRITE setYStep)
	Q_PROPERTY(double yEnd READ yEnd WRITE setYEnd)
	Q_PROPERTY(double timeStep READ timeStep WRITE setTimeStep)
	Q_PROPERTY(int fastAxis READ fastAxis WRITE setFastAxis)
	Q_PROPERTY(int slowAxis READ slowAxis WRITE setSlowAxis)

	Q_CLASSINFO("AMDbObject_Attributes", "description=Generic 2D Scan Configuration")

public:
	/// Axis enum.
	enum Axis { X = 0, Y };

	/// Constructor.
	AM2DScanConfiguration(QObject *parent = 0);
	/// Copy constructor.
	AM2DScanConfiguration(const AM2DScanConfiguration &original);
	/// Destructor.
	virtual ~AM2DScanConfiguration();

	// Description and general info section
	/////////////////////////////////////////////////

	/// Returns the pointer to the meta object.
	virtual const QMetaObject* getMetaObject() { return metaObject(); }

	/// A human-readable description of this scan configuration. Can be re-implemented to provide more details. Used by scan action to set the title for the action view.
	virtual QString description() const {
		return QString("2D Scan over %1 and %2").arg(xAxisName()).arg(yAxisName());
	}
	/// A human-readable synopsis of this scan configuration. Can be re-implemented to proved more details. Used by scan action to set the main text in the action view.
	virtual QString detailedDescription() const{
		return QString("2D Scan over %1 and %2 from (%3,%4) %5 and (%6,%7) %8")
				.arg(xAxisName())
				.arg(yAxisName())
				.arg(xStart())
				.arg(xEnd())
				.arg(xAxisUnits())
				.arg(yStart())
				.arg(yEnd())
				.arg(yAxisUnits());
	}
	/// The auto-generated scan name. Can be re-implemented to customize for each scan type.
	virtual QString autoScanName() const{
		return QString("2D Scan");
	}
	/// The scientific technique this configuration is for
	virtual QString technique() const{
		return "2D Scan";
	}

	/// Returns whether the x axis setup is valid.  Checks to make sure that you can make it from the start to the end using the current step size.  Should be re-implemented for more complicated scans.
	virtual bool validXAxis() const;
	/// Returns whether the y axis setup is valid.  Checks to make sure that you can make it from the start to the end using the current step size.  Should be re-implemented for more complicated scans.
	virtual bool validYAxis() const;
	/// Returns whether the time step is valid.  Returns true if the time is greater than zero.
	virtual bool validTimeStep() const;

	// Axis and time information
	/////////////////////////////////////////////////////

	/// Returns the start position of the x axis.
	double xStart() const { return xRange_.minimum(); }
	/// Returns the step size of the x axis.
	double xStep() const { return steps_.first; }
	/// Returns the end position of the x axis.
	double xEnd() const { return xRange_.maximum(); }

	/// Returns the start position of the y axis.
	double yStart() const { return yRange_.minimum(); }
	/// Returns the step size of the y axis.
	double yStep() const { return steps_.second; }
	/// Returns the end position of the y axis.
	double yEnd() const { return yRange_.maximum(); }

	/// Returns the time step per point.
	double timeStep() const { return time_; }

	/// Returns which axis is the fast axis.
	Axis fastAxis() const { return fast_; }
	/// Returns which axis is the slow axis.
	Axis slowAxis() const { return slow_; }

	// Convenience methods.
	/////////////////////////////////////////////////////

	/// Returns the range of the x axis <min, max>.
	AMRange xRange() const { return xRange_; }
	/// Returns the range of the y axis <min, max>.
	AMRange yRange() const { return yRange_; }
	/// Returns the step size as a QPair<xStep, yStep>.
	QPair<double, double> steps() const { return steps_; }

	// Axis name and units info (pure virtual)
	////////////////////////////////////////////////////

	/// Pure virtual function.  Returns the x-axis name.
	virtual QString xAxisName() const = 0;
	/// Pure virtual function.  Returns the x-axis units.
	virtual QString xAxisUnits() const = 0;
	/// Pure virtual function.  Returns the y-axis name.
	virtual QString yAxisName() const = 0;
	/// Pure virtual function.  Returns teh y-axis units.
	virtual QString yAxisUnits() const = 0;

signals:
	/// Notifier that the x axis start value has changed.
	void xStartChanged(double);
	/// Notifier that the x axis step size has changed.
	void xStepChanged(double);
	/// Notifier that the x axis end value has changed.
	void xEndChanged(double);
	/// Notifier that the y axis start value has changed.
	void yStartChanged(double);
	/// Notifier that the y axis step size has changed.
	void yStepChanged(double);
	/// Notifier that the y axis end value has changed.
	void yEndChanged(double);
	/// Notifier that the time per point has changed.
	void timeStepChanged(double);
	/// Notifier that the axis configuration has changed (ie: the fast axis and slow axis has changed).
	void axisConfigurationChanged();

public slots:
	// Setters
	//////////////////////////////////

	/// Sets the start position for the x axis.
	void setXStart(double start) { xRange_.setMinimum(start); emit xStartChanged(start); setModified(true); }
	/// Sets the step size for the x axis.
	void setXStep(double stepSize) { steps_.first = stepSize; emit xStepChanged(stepSize); setModified(true); }
	/// Sets the end position for the x axis.
	void setXEnd(double end) { xRange_.setMaximum(end); emit xEndChanged(end); setModified(true); }
	/// Sets the start position for the y axis.
	void setYStart(double start) { yRange_.setMinimum(start); emit yStartChanged(start); setModified(true); }
	/// Sets the step size for the y axis.
	void setYStep(double stepSize) { steps_.second = stepSize; emit yStepChanged(stepSize); setModified(true); }
	/// Sets the end position for the y axis.
	void setYEnd(double end) { yRange_.setMaximum(end); emit yEndChanged(end); setModified(true); }
	/// Sets the time per point.
	void setTimeStep(double time) { time_ = time; emit timeStepChanged(time); setModified(true); }
	/// Sets the fast axis.  This will automatically change the slow axis as well.
	void setFastAxis(Axis newFast) { fast_ = newFast; slow_ = (newFast == X) ? Y : X; emit axisConfigurationChanged(); setModified(true); }
	/// Overloaded.  Used for database loading.
	void setFastAxis(int newFast) { setFastAxis((Axis)newFast); }
	/// Sets the slow axis.  This will automatically change the fast axis as well.
	void setSlowAxis(Axis newSlow) { slow_ = newSlow; fast_ = (newSlow == X) ? Y : X; emit axisConfigurationChanged(); setModified(true); }
	/// Overloaded.  Used for database loading.
	void setSlowAxis(int newSlow) { setSlowAxis((Axis)newSlow); }

	// Convenience setters.
	///////////////////////////////////

	/// Sets the range for the x axis.
	void setXRange(AMRange x) { setXStart(x.minimum()); setXEnd(x.maximum()); }
	/// Overloaded.  Sets the range for the x axis.
	void setXRange(double start, double end) { setXStart(start); setXEnd(end); }
	/// Sets the range for the y axis.
	void setYRange(AMRange y) { setYStart(y.minimum()); setYEnd(y.maximum()); }
	/// Overloaded.  Sets the range for the y axis.
	void setYRange(double start, double end) { setYStart(start); setYEnd(end); }
	/// Sets the step size of both axes.
	void setStepSize(QPair<double, double> step) { setXStep(step.first); setYStep(step.second); }
	/// Overloaded.  Sets the step size of both axes.
	void setStepSize(double x, double y) { setXStep(x); setYStep(y); }

protected:
	/// Holds the range for the x axis. <min, max>
	AMRange xRange_;
	/// Holds the range for the y axis. <min, max>
	AMRange yRange_;
	/// Holds the steps for both axes. <xStep, yStep>
	QPair<double, double> steps_;
	/// Holds the time per point.
	double time_;

	/// Holds which axis is the fast axis.
	Axis fast_;
	/// Holds which axis is the slow axis.  This will always be the opposite of fast.
	Axis slow_;
};

#endif // AM2DSCANCONFIGURATION_H
