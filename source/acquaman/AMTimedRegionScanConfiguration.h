#ifndef AMTIMEDREGIONSCANCONFIGURATION_H
#define AMTIMEDREGIONSCANCONFIGURATION_H

#include "AMScanConfiguration.h"

/// An AMTimedRegionScanConfiguration is an axis based scan where the independent axis is time, rather than moving any particular control.
/*!
  This type of scan is really simple in that all that it requires from a configuration stand point is the amount of time
  per point to acquire for and the overall amount of time to acquire.
  */
class AMTimedRegionScanConfiguration : public AMScanConfiguration
{
	Q_OBJECT

	Q_PROPERTY(double time READ time WRITE setTime)
	Q_PROPERTY(double totalTime READ totalTime WRITE setTotalTime)

	Q_CLASSINFO("AMDbObject_Attributes", "description=Generic Time Scan Configuration")

public:
	/// Constructor.
	AMTimedRegionScanConfiguration(QObject *parent = 0);
	/// Copy constructor.
	AMTimedRegionScanConfiguration(const AMTimedRegionScanConfiguration &original);
	/// Destructor.
	virtual ~AMTimedRegionScanConfiguration(){}

	/// Returns the meta object.
	const QMetaObject *getMetaObject();

	/// Returns the time per point.
	double time() const;
	/// Returns the total time for the scan.
	double totalTime() const;

	/// A human-readable description of this scan configuration. Can be re-implemented to provide more details. Used by scan action to set the title for the action view.
	virtual QString description() const
	{
		return QString("Time Scan of %1s in %2s time steps.").arg(totalTime_).arg(time_);
	}

	/// A human-readable synopsis of this scan configuration. Can be re-implemented to proved more details. Used by scan action to set the main text in the action view.
	virtual QString detailedDescription() const
	{
		return QString("Time Scan of %1s in %2s time steps.").arg(totalTime_).arg(time_);
	}

	/// The auto-generated scan name. Can be re-implemented to customize for each scan type.
	virtual QString autoScanName() const
	{
		return QString("Timed Scan");
	}

signals:
	/// Notifier that the time per point changed.
	void timeChanged(double);
	/// Notifier that the total time of the scan changed.
	void totalTimeChanged(double);

public slots:
	/// Sets the time per point.
	void setTime(double newTime);
	/// Sets the total time of the scan.
	void setTotalTime(double newTotalTime);

protected:
	/// Holds the time.
	double time_;
	/// Holds the total time.
	double totalTime_;
};

#endif // AMTIMEDREGIONSCANCONFIGURATION_H
