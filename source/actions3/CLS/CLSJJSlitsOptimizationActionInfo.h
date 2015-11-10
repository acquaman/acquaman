#ifndef CLSJJSLITSOPTIMIZATIONACTIONINFO_H
#define CLSJJSLITSOPTIMIZATIONACTIONINFO_H

#include "actions3/AMActionInfo3.h"
#include "acquaman/AMGenericStepScanConfiguration.h"
#include ""

class CLSJJSlitsOptimizationActionInfo : public AMActionInfo3
{
    Q_OBJECT

public:
	/// Constructor.
	Q_INVOKABLE CLSJJSlitsOptimizationActionInfo(const QList<AMGenericStepScanConfiguration*> &configurationList, const AMDetectorInfo &detectorInfo, QObject *parent = 0);
	/// Copy constructor.
	CLSJJSlitsOptimizationActionInfo(const CLSJJSlitsOptimizationActionInfo &original);
	/// Destructor.
	virtual ~CLSJJSlitsOptimizationActionInfo();

	/// Returns an AMActionInfo3 instance that is identical to this action info.
	virtual AMActionInfo3* createCopy() const;
	/// Returns this action info's description.
	virtual QString typeDescription() const { return "Optimize the JJ slits"; }

	/// Returns the scan configurations list.
	QList<AMGenericStepScanConfiguration*> configurationList() const { return configurationList_; }
	/// Returns the analysis detector info.
	AMDetectorInfo detectorInfo() const { return detectorInfo_; }

protected:
	/// Returns the scan configurations list.
	AMDbObjectList dbReadConfigurationList();
	/// Called by the database system on loadFromDb() to give a list of scan configurations.
	void dbLoadConfigurationList(const AMDbObjectList &newConfigurations);

	/// Returns the detector info.
	AMDbObject *dbReadDetectorInfo() { return &detectorInfo_; }
	/// Called by the database system on loadFromDb() to give the detector info.
	void dbLoadDetectorInfo(AMDbObject *);

protected:
	/// The list of scan configurations to be run.
	QList<AMGenericStepScanConfiguration*> configurationList_;
	/// The detector info to use for the optimization analysis.
	AMDetectorInfo detectorInfo_;
};

#endif // CLSJJSLITSOPTIMIZATIONACTIONINFO_H
