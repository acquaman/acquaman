#ifndef BIOXASSOLLERSLITSCANCONFIGURATION_H
#define BIOXASSOLLERSLITSCANCONFIGURATION_H

#include "acquaman/AMStepScanConfiguration.h"

class BioXASSollerSlitScanConfiguration : public AMStepScanConfiguration
{
	Q_OBJECT

public:
	/// Constructor.
	Q_INVOKABLE BioXASSollerSlitScanConfiguration(QObject *parent = 0);
	/// Copy constructor.
	BioXASSollerSlitScanConfiguration(const BioXASSollerSlitScanConfiguration &original);
	/// Destructor.
	virtual ~BioXASSollerSlitScanConfiguration();

	/// Returns a pointer to a newly-created copy of this scan configuration.  (It takes the role of a copy constructor, but is virtual so that our high-level classes can copy a scan configuration without knowing exactly what kind it is.)
	virtual AMScanConfiguration* createCopy() const;

	/// Returns a pointer to a newly-created AMScanController that is appropriate for executing this kind of scan configuration.  The controller should be initialized to use this scan configuration object as its scan configuration.  Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanController* createController();

	/// Returns a pointer to a newly-created AMScanConfigurationView that is appropriate for viewing and editing this kind of scan configuration. Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanConfigurationView* createView();

	/// A human-readable synopsis of this scan configuration. Can be re-implemented to proved more details. Used by scan action to set the main text in the action view.
	virtual QString detailedDescription() const;

	/// Returns the total time.
	double totalTime() const { return totalTime_; }

signals:
	/// Notifier that the total time changed.
	void totalTimeChanged(double);

public slots:
	/// Set the X motor for the soller slits.
	void setSollerSlitX(const AMControlInfo &info);
	/// Set the Z motor for the soller slits.
	void setSollerSlitZ(const AMControlInfo &info);
	/// Sets which detector is going to be used.
	void setDetector(const AMDetectorInfo &info);

protected slots:
	/// Computes the total time required for the scan.
	void computeTotalTime();

protected:
	/// Holds the total time.
	double totalTime_;
};

#endif // BIOXASSOLLERSLITSCANCONFIGURATION_H
