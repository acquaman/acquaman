#ifndef CLSJJSLITOPTIMIZATIONACTIONINFO_H
#define CLSJJSLITOPTIMIZATIONACTIONINFO_H

#include "actions3/AMActionInfo3.h"
#include "acquaman/AMGenericStepScanConfiguration.h"

class CLSJJSlitOptimizationActionInfo : public AMActionInfo3
{
    Q_OBJECT

public:
	Q_INVOKABLE CLSJJSlitOptimizationActionInfo(AMGenericStepScanConfiguration *configuration = 0, const AMDetectorInfo &detectorInfo = AMDetectorInfo(), QObject *parent = 0);
	/// Copy constructor.
	CLSJJSlitOptimizationActionInfo(const CLSJJSlitOptimizationActionInfo &original);
	/// Destructor.
	virtual ~CLSJJSlitOptimizationActionInfo();

	/// Returns an AMActionInfo3 instance that is identical to this action info.
	virtual AMActionInfo3* createCopy() const;
	/// Returns this action info's description.
	virtual QString typeDescription() const { return "Optimize the JJ slit"; }

	/// Returns the scan configuration.
	AMGenericStepScanConfiguration* configuration() const { return configuration_; }
	/// Returns the analysis detector info.
	AMDetectorInfo detectorInfo() const { return detectorInfo_; }

protected:
	/// Returns the scan configuration.
	AMDbObject* dbReadConfiguration();
	/// Called by the database system on loadFromDb() to give a scan configuration.
	void dbLoadConfiguration(const AMDbObject &newConfiguration);

	/// Returns the detector info.
	AMDbObject* dbReadDetectorInfo() { return &detectorInfo_; }
	/// Called by the database system on loadFromDb() to give the detector info.
	void dbLoadDetectorInfo(AMDbObject *);

protected:
	/// The list of scan configurations to be run.
	AMGenericStepScanConfiguration* configuration_;
	/// The detector info to use for the optimization analysis.
	AMDetectorInfo detectorInfo_;

};

#endif // CLSJJSLITOPTIMIZATIONACTIONINFO_H
