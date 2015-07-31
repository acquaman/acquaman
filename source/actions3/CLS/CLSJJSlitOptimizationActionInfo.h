#ifndef CLSJJSLITOPTIMIZATIONACTIONINFO_H
#define CLSJJSLITOPTIMIZATIONACTIONINFO_H

#include "acquaman/CLS/CLSJJSlitScanConfiguration.h"
#include "actions3/AMListActionInfo3.h"
#include "beamline/CLS/CLSJJSlits.h"

class CLSJJSlitOptimizationActionInfo : public AMListActionInfo3
{
    Q_OBJECT

public:
	/// Constructor.
	Q_INVOKABLE CLSJJSlitOptimizationActionInfo(CLSJJSlits::Blades::Direction bladesDirection = CLSJJSlits::Blades::None, CLSJJSlits::Blades::Value bladesValue = CLSJJSlits::Blades::Invalid, double initialVerticalGap = 0, double initialVerticalCenter = 0, double initialHorizontalGap = 0, double initialHorizontalCenter = 0, CLSJJSlitScanConfiguration *scanConfiguration = 0, QObject *parent = 0);
	/// Copy constructor.
	CLSJJSlitOptimizationActionInfo(const CLSJJSlitOptimizationActionInfo &original);
	/// Destructor.
	virtual ~CLSJJSlitOptimizationActionInfo();

	/// Returns an AMActionInfo3 instance that is identical to this action info.
	virtual AMActionInfo3* createCopy() const;
	/// Returns this action info's description.
	virtual QString typeDescription() const { return "JJ Slit Optimization"; }

	/// Returns the blade direction of interest.
	CLSJJSlits::Blades::Direction direction() const { return direction_; }
	/// Returns the blade value of interest.
	CLSJJSlits::Blades::Value value() const { return value_; }

	/// Returns the initial vertical gap.
	double initialVerticalGap() const { return initialVerticalGap_; }
	/// Returns the initial vertical center.
	double initialVerticalCenter() const { return initialVerticalCenter_; }
	/// Returns the initial horizontal gap.
	double initialHorizontalGap() const { return initialHorizontalGap_; }
	/// Returns the initial horizontal center.
	double initialHorizontalCenter() const { return initialHorizontalCenter_; }

	/// Returns the slit scan configuration.
	CLSJJSlitScanConfiguration* scanConfiguration() const { return scanConfiguration_; }

protected:
	/// The blade direction of interest.
	int direction_;
	/// The blade value of interest.
	int value_;

	/// The initial vertical gap.
	double initialVerticalGap_;
	/// The initial vertical center.
	double initialVerticalCenter_;
	/// The initial horizontal gap.
	double initialHorizontalGap_;
	/// The initial horizontal center.
	double initialHorizontalCenter_;

	/// The slit scan configuration.
	CLSJJSlitScanConfiguration *scanConfiguration_;
};

#endif // CLSJJSLITOPTIMIZATIONACTIONINFO_H
