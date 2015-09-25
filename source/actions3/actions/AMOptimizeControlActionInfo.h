#ifndef AMOPTIMIZECONTROLACTIONINFO_H
#define AMOPTIMIZECONTROLACTIONINFO_H

#include "actions3/AMActionInfo3.h"
#include "acquaman/AMGenericStepScanConfiguration.h"
#include "dataman/AMAnalysisBlock.h"

class AMOptimizeControlActionInfo : public AMActionInfo3
{
    Q_OBJECT

public:
	/// Constructor.
	Q_INVOKABLE AMOptimizeControlActionInfo(AMGenericStepScanConfiguration *configuration, AMAnalysisBlock *optimizationAB, QObject *parent = 0);
	/// Copy constructor.
	AMOptimizeControlActionInfo(const AMOptimizeControlActionInfo &original);
	/// Destructor.
	virtual ~AMOptimizeControlActionInfo();

	/// Returns an AMActionInfo3 instance that is identical to this action info.
	virtual AMActionInfo3* createCopy() const;
	/// Returns this action info's description.
	virtual QString typeDescription() const { return "Control Optimization Action"; }

	/// Returns the configuration.
	AMGenericStepScanConfiguration* configuration() const  { return configuration_; }
	/// Returns the optimization analysis block.
	AMAnalysisBlock* optimizationAB() const { return optimizationAB_; }

signals:
	/// Notifier that the configuration has changed.
	void configurationChanged(AMGenericStepScanConfiguration *newConfiguration);
	/// Notifier that the optimization analysis block has changed.
	void optimizationABChanged(AMAnalysisBlock *newOptimizationAB);

public slots:
	/// Sets the configuration.
	void setConfiguration(AMGenericStepScanConfiguration *newConfiguration);
	/// Sets the optimization analysis block.
	void setOptimizationAB(AMAnalysisBlock *newOptimizationAB);

protected:
	/// The configuration.
	AMGenericStepScanConfiguration *configuration_;
	/// The analysis block that determines the control's optimal position.
	AMAnalysisBlock *optimizationAB_;
};

#endif // AMOPTIMIZECONTROLACTIONINFO_H
