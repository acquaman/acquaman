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


#ifndef AM3DSCANCONFIGURATION_H
#define AM3DSCANCONFIGURATION_H

#include "acquaman/AMScanConfiguration.h"
#include "util/AMRange.h"

/// An AM3DScanConfiguration is the parent class for any scan that wants to implement a simple parallelpiped 3D scan.
/*!
	The AM3DScanConfiguration class is designed to allow beamline specific implementation classes
	to inherit it, but take advantage of a common method for configuring some key features associated
	with the scan.

	Regardless of the type of scan being implemented there will always be a x-axis control, y-axis control, and a
	z-axis control.  By the nature of a map, there is a priority or 'fastness' to the map where each axis is scanned
	faster than another.  A 2D example would be rastering along the x-axis.  The x-axis is 'faster' than the
	y-axis because it moves every point where as the y-axis only changes at the end of a line.  This is the same
	for 3D maps.

	Some default information is required is:
		- x range
		- y range
		- z range
		- step size for x
		- step size for y
		- step size for z
		- time per point

	What is actually being scanned can be handled by the implementation classes.

	Note, this is a pure virtual class and cannot be instantiated.  The methods for the
	name and units for each axis were chosen as the methods to be explicitly implemented
	due to the many possibilities for 3D maps.
  */
class AM3DScanConfiguration : public AMScanConfiguration
{
	Q_OBJECT

	Q_PROPERTY(double xStart READ xStart WRITE setXStart)
	Q_PROPERTY(double xStep READ xStep WRITE setXStep)
	Q_PROPERTY(double xEnd READ xEnd WRITE setXEnd)
	Q_PROPERTY(double yStart READ yStart WRITE setYStart)
	Q_PROPERTY(double yStep READ yStep WRITE setYStep)
	Q_PROPERTY(double yEnd READ yEnd WRITE setYEnd)
	Q_PROPERTY(double zStart READ zStart WRITE setZStart)
	Q_PROPERTY(double zStep READ zStep WRITE setZStep)
	Q_PROPERTY(double zEnd READ zEnd WRITE setZEnd)
	Q_PROPERTY(double timeStep READ timeStep WRITE setTimeStep)
	Q_PROPERTY(int xPriority READ xPriority WRITE setXPriority)
	Q_PROPERTY(int yPriority READ yPriority WRITE setYPriority)
	Q_PROPERTY(int zPriority READ zPriority WRITE setZPriority)

	Q_CLASSINFO("AMDbObject_Attributes", "description=Generic 2D Scan Configuration")

public:
	/// Constructor.
 	virtual ~AM3DScanConfiguration();
	AM3DScanConfiguration(QObject *parent = 0);
	/// Copy constructor.
	AM3DScanConfiguration(const AM3DScanConfiguration &original);

	// Description and general info section
	/////////////////////////////////////////////////

	/// Returns the pointer to the meta object.
	virtual const QMetaObject* getMetaObject() { return metaObject(); }

	/// A human-readable description of this scan configuration. Can be re-implemented to provide more details. Used by scan action to set the title for the action view.
	virtual QString description() const {
		return QString("3D Scan over %1, %2, and %3").arg(xAxisName()).arg(yAxisName()).arg(zAxisName());
	}
	/// A human-readable synopsis of this scan configuration. Can be re-implemented to proved more details. Used by scan action to set the main text in the action view.
	virtual QString detailedDescription() const{
		return QString("3D Scan over %1, %2, and %3 from (%4,%5) %6, (%7,%8) %9, and (%10,%11) %12")
				.arg(xAxisName())
				.arg(yAxisName())
				.arg(zAxisName())
				.arg(xStart())
				.arg(xEnd())
				.arg(xAxisUnits())
				.arg(yStart())
				.arg(yEnd())
				.arg(yAxisUnits())
				.arg(zStart())
				.arg(zEnd())
				.arg(zAxisUnits());
	}
	/// The auto-generated scan name. Can be re-implemented to customize for each scan type.
	virtual QString autoScanName() const{
		return QString("3D Scan");
	}
	/// The scientific technique this configuration is for
	virtual QString technique() const{
		return "3D Scan";
	}

	/// Returns whether the x axis setup is valid.  Checks to make sure that you can make it from the start to the end using the current step size.  Should be re-implemented for more complicated scans.
	virtual bool validXAxis() const;
	/// Returns whether the y axis setup is valid.  Checks to make sure that you can make it from the start to the end using the current step size.  Should be re-implemented for more complicated scans.
	virtual bool validYAxis() const;
	/// Returns whether the z axis setup is valid.  Checks to make sure that you can make it from the start to the end using the current step size.  Should be re-implemented for more complicated scans.
	virtual bool validZAxis() const;
	/// Returns whether the time step is valid.  Returns true if the time is greater than zero.
	virtual bool validTimeStep() const;

	// Axis and time information
	/////////////////////////////////////////////////////

	/// Returns the start position of the x axis.
	double xStart() const { return xRange_.minimum(); }
	/// Returns the step size of the x axis.
	double xStep() const { return steps_.at(0); }
	/// Returns the end position of the x axis.
	double xEnd() const { return xRange_.maximum(); }

	/// Returns the start position of the y axis.
	double yStart() const { return yRange_.minimum(); }
	/// Returns the step size of the y axis.
	double yStep() const { return steps_.at(1); }
	/// Returns the end position of the y axis.
	double yEnd() const { return yRange_.maximum(); }

	/// Returns the start position of the z axis.
	double zStart() const { return zRange_.minimum(); }
	/// Returns the step size of the z axis.
	double zStep() const { return steps_.at(2); }
	/// Returns the end position of the z axis.
	double zEnd() const { return zRange_.maximum(); }

	/// Returns the time step per point.
	double timeStep() const { return time_; }

	/// Returns priority for the x axis.  Values are 1, 2, or 3 with 3 being the fastest.
	int xPriority() const { return xPriority_; }
	/// Returns priority for the x axis.  Values are 1, 2, or 3 with 3 being the fastest.
	int yPriority() const { return yPriority_; }
	/// Returns priority for the x axis.  Values are 1, 2, or 3 with 3 being the fastest.
	int zPriority() const { return zPriority_; }

	// Convenience methods.
	/////////////////////////////////////////////////////

	/// Returns the range of the x axis <min, max>.
	AMRange xRange() const { return xRange_; }
	/// Returns the range of the y axis <min, max>.
	AMRange yRange() const { return yRange_; }
	/// Returns the range of the y axis <min, max>.
	AMRange zRange() const { return zRange_; }
	/// Returns the step size as a list.  Order is {xStep, yStep, zStep}.
	QList<double> steps() const { return steps_; }

	// Axis name and units info (pure virtual)
	////////////////////////////////////////////////////

	/// Pure virtual function.  Returns the x-axis name.
	virtual QString xAxisName() const = 0;
	/// Pure virtual function.  Returns the x-axis units.
	virtual QString xAxisUnits() const = 0;
	/// Pure virtual function.  Returns the y-axis name.
	virtual QString yAxisName() const = 0;
	/// Pure virtual function.  Returns the y-axis units.
	virtual QString yAxisUnits() const = 0;
	/// Pure virtual function.  Returns the z-axis name.
	virtual QString zAxisName() const = 0;
	/// Pure virtual function.  Returns the z-axis units.
	virtual QString zAxisUnits() const = 0;

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
	/// Notifier that the z axis start value has changed.
	void zStartChanged(double);
	/// Notifier that the z axis step size has changed.
	void zStepChanged(double);
	/// Notifier that the z axis end value has changed.
	void zEndChanged(double);
	/// Notifier that the time per point has changed.
	void timeStepChanged(double);
	/// Notifier that the axis configuration has changed (ie: the fast axis and slow axis has changed).
	void axisConfigurationChanged();

public slots:

	// Setters
	//////////////////////////////////

	/// Sets the start position for the x axis.
	void setXStart(double start);
	/// Sets the step size for the x axis.
	void setXStep(double stepSize);
	/// Sets the end position for the x axis.
	void setXEnd(double end);
	/// Sets the start position for the y axis.
	void setYStart(double start);
	/// Sets the step size for the y axis.
	void setYStep(double stepSize);
	/// Sets the end position for the y axis.
	void setYEnd(double end);
	/// Sets the start position for the z axis.
	void setZStart(double start);
	/// Sets the step size for the z axis.
	void setZStep(double stepSize);
	/// Sets the end position for the z axis.
	void setZEnd(double end);
	/// Sets the time per point.
	void setTimeStep(double time);
	/// Sets the priority for the x-axis.  Must be 0, 1, or 2.
	void setXPriority(int priority);
	/// Sets the priority for the y-axis.  Must be 0, 1, or 2.
	void setYPriority(int priority);
	/// Sets the priority for the z-axis.  Must be 0, 1, or 2.
	void setZPriority(int priority);

	// Convenience setters.
	///////////////////////////////////

	/// Sets the range for the x axis.
	void setXRange(AMRange x);
	/// Overloaded.  Sets the range for the x axis.
	void setXRange(double start, double end);
	/// Sets the range for the y axis.
	void setYRange(AMRange y);
	/// Overloaded.  Sets the range for the y axis.
	void setYRange(double start, double end);
	/// Sets the range for the z axis.
	void setZRange(AMRange z);
	/// Overloaded.  Sets the range for the z axis.
	void setZRange(double start, double end);
	/// Sets the step size of all axes.
	void setStepSize(QList<double> steps);
	/// Overloaded.  Sets the step size of all axes.
	void setStepSize(double x, double y, double z);

protected:
	/// Holds the range for the x axis. <min, max>
	AMRange xRange_;
	/// Holds the range for the y axis. <min, max>
	AMRange yRange_;
	/// Holds the range for the z axis. <min, max>
	AMRange zRange_;
	/// Holds the steps for all axes. <xStep, yStep, zStep>
	QList<double> steps_;
	/// Holds the time per point.
	double time_;
	/// Holds the priority for the x-axis.
	int xPriority_;
	/// Holds the priority for the y-axis.
	int yPriority_;
	/// Holds the priority for the z-axis.
	int zPriority_;
};

#endif // AM3DSCANCONFIGURATION_H
