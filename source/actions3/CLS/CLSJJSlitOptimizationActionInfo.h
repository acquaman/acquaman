#ifndef CLSJJSLITOPTIMIZATIONACTIONINFO_H
#define CLSJJSLITOPTIMIZATIONACTIONINFO_H

#include "acquaman/AMGenericStepScanConfiguration.h"
#include "actions3/AMListActionInfo3.h"
#include "beamline/CLS/CLSJJSlits.h"

class CLSJJSlitOptimizationActionInfo : public AMListActionInfo3
{
    Q_OBJECT

public:
	/// Constructor.
	Q_INVOKABLE CLSJJSlitOptimizationActionInfo(AMGenericStepScanConfiguration *configuration, double initialGap = 0, double initialCenter = 0, QObject *parent = 0);
	/// Copy constructor.
	CLSJJSlitOptimizationActionInfo(const CLSJJSlitOptimizationActionInfo &original);
	/// Destructor.
	virtual ~CLSJJSlitOptimizationActionInfo();

	/// Returns an AMActionInfo3 instance that is identical to this action info.
	virtual AMActionInfo3* createCopy() const;
	/// Returns this action info's description.
	virtual QString typeDescription() const { return "JJ Slit Optimization"; }

	/// Returns the slit scan configuration.
	AMGenericStepScanConfiguration* configuration() const { return configuration_; }

	/// Returns the initial gap.
	double initialGap() const { return initialGap_; }
	/// Returns the initial center.
	double initialCenter() const { return initialCenter_; }

signals:
	/// Notifier that the configuration has changed.
	void configurationChanged(AMGenericStepScanConfiguration *newConfiguration);

public slots:
	/// Sets the scan configuration.
	void setConfiguration(AMGenericStepScanConfiguration *newConfiguration);

protected:
	/// The slit scan configuration.
	AMGenericStepScanConfiguration *configuration_;

	/// The initial gap.
	double initialGap_;
	/// The initial center.
	double initialCenter_;
};

#endif // CLSJJSLITOPTIMIZATIONACTIONINFO_H
