#ifndef BIOXASJJSLITSSCANCONFIGURATION_H
#define BIOXASJJSLITSSCANCONFIGURATION_H

#include "acquaman/AMStepScanConfiguration.h"

class BioXASJJSlitsScanConfiguration : public AMStepScanConfiguration
{
    Q_OBJECT

public:
	/// Constructor.
	Q_INVOKABLE BioXASJJSlitsScanConfiguration(QObject *parent = 0);
	/// Copy constructor.
	BioXASJJSlitsScanConfiguration(const BioXASJJSlitsScanConfiguration &original);
	/// Destructor.
	virtual ~BioXASJJSlitsScanConfiguration();


	/// Returns a pointer to a newly-created copy of this scan configuration. (It takes the role of a copy constructor, but is virtual so that our high-level classes can copy a scan configuration without knowing exactly what kind it is.)
	virtual AMScanConfiguration* createCopy() const;
	/// Returns a pointer to a newly-created AMScanController that is appropriate for executing this kind of scan configuration. The controller should be initialized to use this scan configuration object as its scan configuration. Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanController* createController();
	/// Returns a pointer to a newly-created AMScanConfigurationView that is appropriate for viewing and editing this kind of scan configuration. Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanConfigurationView* createView();
	/// A human-readable synopsis of this scan configuration. Can be re-implemented to provide more details. Used by scan action to set the main text in the action view.
	virtual QString detailedDescription() const;
	/// Returns the total time.
	double totalTime() const { return totalTime_; }

	/// Returns the gap JJ slit control info.
	AMControlInfo jjSlitsGap() const { return axisControlInfos_.at(0); }
	/// Returns the center JJ slit control info.
	AMControlInfo jjSlitsCenter() const { return axisControlInfos_.at(1); }

signals:
	/// Notifier that the total time changed.
	void totalTimeChanged(double);

public slots:
	/// Set the gap control for the JJ slits.
	void setJJSlitGap(const AMControlInfo &info);
	/// Set the center control for the JJ slits.
	void setJJSlitCenter(const AMControlInfo &info);
	/// Sets which detector is going to be used.
	void setDetector(const AMDetectorInfo &info);

protected slots:
	/// Computes the total time required for the scan.
	void computeTotalTime();

protected:
	/// Holds the total time.
	double totalTime_;
};

#endif // BIOXASJJSLITSSCANCONFIGURATION_H
