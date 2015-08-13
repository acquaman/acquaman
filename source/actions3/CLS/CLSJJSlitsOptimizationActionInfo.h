#ifndef CLSJJSLITSOPTIMIZATIONACTIONINFO_H
#define CLSJJSLITSOPTIMIZATIONACTIONINFO_H

#include "acquaman/CLS/CLSJJSlitScanConfiguration.h"
#include "actions3/AMListActionInfo3.h"
#include "beamline/CLS/CLSJJSlits.h"

class CLSJJSlitsOptimizationActionInfo : public AMListActionInfo3
{
    Q_OBJECT

public:
	/// Constructor.
	Q_INVOKABLE CLSJJSlitsOptimizationActionInfo(CLSJJSlits::Direction::Option direction = CLSJJSlits::Direction::Invalid, CLSJJSlits::Property::Option property = CLSJJSlits::Property::Invalid, double initialVerticalGap = 0, double initialVerticalCenter = 0, double initialHorizontalGap = 0, double initialHorizontalCenter = 0, CLSJJSlitScanConfiguration *scanConfiguration = 0, QObject *parent = 0);
	/// Copy constructor.
	CLSJJSlitsOptimizationActionInfo(const CLSJJSlitsOptimizationActionInfo &original);
	/// Destructor.
	virtual ~CLSJJSlitsOptimizationActionInfo();

	/// Returns an AMActionInfo3 instance that is identical to this action info.
	virtual AMActionInfo3* createCopy() const;
	/// Returns this action info's description.
	virtual QString typeDescription() const { return "JJ Slits Optimization"; }

	/// Returns the slit direction of interest.
	CLSJJSlits::Direction::Option direction() const { return direction_; }
	/// Returns the slit value of interest.
	CLSJJSlits::Property::Option property() const { return property_; }

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
	/// The slit direction of interest.
	CLSJJSlits::Direction::Option direction_;
	/// The slit property of interest.
	CLSJJSlits::Property::Option property_;

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

#endif // CLSJJSLITSOPTIMIZATIONACTIONINFO_H
