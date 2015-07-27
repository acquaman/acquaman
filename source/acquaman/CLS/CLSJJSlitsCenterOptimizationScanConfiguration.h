#ifndef CLSJJSLITSCENTEROPTIMIZATIONSCANCONFIGURATION_H
#define CLSJJSLITSCENTEROPTIMIZATIONSCANCONFIGURATION_H

#include "acquaman/AMGenericStepScanConfiguration.h"
#include "beamline/CLS/CLSJJSlits.h"

class CLSJJSlitsCenterOptimizationScanConfiguration : public AMGenericStepScanConfiguration
{
    Q_OBJECT

public:
	/// Constructor.
	Q_INVOKABLE CLSJJSlitsCenterOptimizationScanConfiguration(QObject *parent = 0);
	/// Copy constructor.
	CLSJJSlitsCenterOptimizationScanConfiguration(const CLSJJSlitsCenterOptimizationScanConfiguration &original);
	/// Destructor.
	virtual ~CLSJJSlitsCenterOptimizationScanConfiguration();

	/// Returns a newly-created copy of this scan configuration.
	virtual AMScanConfiguration* createCopy() const;
	/// Returns a newly-created scan controller appropriate for executing this kind of scan configuration.
	virtual AMScanController* createController();
	/// Returns a newly-created view that is appropriate for viewing this kind of scan configuration.
	virtual AMScanConfigurationView* createView() const;

	/// Returns the technique.
	virtual QString technique() const { return "JJ Slits Center Optimization"; }
	/// Returns the description.
	virtual QString description() const { return "JJ Slits Center Optimization"; }
	/// Returns the detailed description.
	virtual QString detailedDescription() const { return "A generic scan that finds the optimal center position of the JJ slits on BioXAS."; }

public slots:
	/// Sets the JJ slits to scan.
	void setJJSlits(CLSJJSlits *jjSlits);
};

#endif // CLSJJSLITSCENTEROPTIMIZATIONSCANCONFIGURATION_H
