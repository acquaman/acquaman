#ifndef VESPERSTIMEDLINESCANCONFIGURATION_H
#define VESPERSTIMEDLINESCANCONFIGURATION_H

#include "acquaman/VESPERS/VESPERSSpatialLineScanConfiguration.h"

/// This class subclasses the standard line scan and adds in a wait after each line scan and then repeats it.
class VESPERSTimedLineScanConfiguration : public VESPERSSpatialLineScanConfiguration
{
	Q_OBJECT

	Q_PROPERTY(double timePerAcquisition READ timePerAcquisition WRITE setTimePerAcquisition)
	Q_PROPERTY(int iterations READ iterations WRITE setIterations)

	Q_CLASSINFO("AMDbObject_Attributes", "description=VESPERS Timed Line Scan Configuration")

public:
	/// Constructor.
	Q_INVOKABLE VESPERSTimedLineScanConfiguration(QObject *parent = 0);
	/// Copy constructor.
	VESPERSTimedLineScanConfiguration(const VESPERSTimedLineScanConfiguration &original);
	/// Destructor.
	virtual ~VESPERSTimedLineScanConfiguration();

	/// Returns a pointer to a newly-created copy of this scan configuration.  (It takes the role of a copy constructor, but is virtual so that our high-level classes can copy a scan configuration without knowing exactly what kind it is.)
	virtual AMScanConfiguration* createCopy() const;

	/// Returns a pointer to a newly-created AMScanController that is appropriate for executing this kind of scan configuration.  The controller should be initialized to use this scan configuration object as its scan configuration.  Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanController* createController();

	/// Returns a pointer to a newly-created AMScanConfigurationView that is appropriate for viewing and editing this kind of scan configuration. Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanConfigurationView* createView();

	/// Returns the technique string.
	virtual QString technique() const;
	/// A human-readable description of this scan configuration. Can be re-implemented to provide more details. Used by scan action to set the title for the action view.
	virtual QString description() const;
	/// A human-readable synopsis of this scan configuration. Can be re-implemented to proved more details. Used by scan action to set the main text in the action view.
	virtual QString detailedDescription() const;

	/// Get a nice looking string that contains all the standard information in an XAS scan.   Used when exporting.
	QString headerText() const;

	/// Returns the total time for the scan.
	double timePerAcquisition() const;
	/// Returns the number of iterations.
	int iterations() const;

signals:
	/// Notifier that the total time of the scan changed.
	void timePerAcquisitionChanged(double);
	/// Notifier that the number of iterations has changed.
	void iterationsChanged(int);

public slots:
	/// Sets the total time of the scan.
	void setTimePerAcquisition(double newTotalTime);
	/// Sets the number of iterations.
	void setIterations(int newIteration);

protected:
	/// Method that does all the calculations for calculating the estimated scan time.
	virtual void computeTotalTimeImplementation();

	/// Holds the total time.
	double timePerAcquisition_;
	/// The number of times we will acquire.
	int iterations_;
};

#endif // VESPERSTIMEDLINESCANCONFIGURATION_H
