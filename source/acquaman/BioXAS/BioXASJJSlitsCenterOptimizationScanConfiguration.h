#ifndef BIOXASJJSLITSCENTEROPTIMIZATIONSCANCONFIGURATION_H
#define BIOXASJJSLITSCENTEROPTIMIZATIONSCANCONFIGURATION_H

#include "acquaman/AMGenericStepScanConfiguration.h"

class BioXASJJSlitsCenterOptimizationScanConfiguration : public AMGenericStepScanConfiguration
{
    Q_OBJECT

public:
	/// Constructor.
    explicit BioXASJJSlitsCenterOptimizationScanConfiguration(QObject *parent = 0);
	/// Copy constructor.
	BioXASJJSlitsCenterOptimizationScanConfiguration(const BioXASJJSlitsCenterOptimizationScanConfiguration &original);
	/// Destructor.
	virtual ~BioXASJJSlitsCenterOptimizationScanConfiguration();

	/// Returns a newly-created copy of this scan configuration.
	virtual AMScanConfiguration* createCopy() const;
	/// Returns a newly-created scan controller appropriate for executing this kind of scan configuration.
	virtual AMScanController* createController() const;
	/// Returns a newly-created view that is appropriate for viewing this kind of scan configuration.
	virtual AMScanConfigurationView* createView() const;

	/// Returns the technique.
	virtual QString technique() const { return "JJ Slits Center Optimization"; }
	/// Returns the description.
	virtual QString description() const { return "JJ Slits Center Optimization"; }
	/// Returns the detailed description.
	virtual QString detailedDescription() const { return "A generic scan that finds the optimal center position of the JJ slits on BioXAS."; }

signals:

public slots:

};

#endif // BIOXASJJSLITSCENTEROPTIMIZATIONSCANCONFIGURATION_H
