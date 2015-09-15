#ifndef AMOPTIMIZECONTROLACTIONINFO_H
#define AMOPTIMIZECONTROLACTIONINFO_H

#include "actions3/AMListActionInfo3.h"
#include "acquaman/AMGenericStepScanConfiguration.h"

class AMOptimizeControlActionInfo : public AMListActionInfo3
{
    Q_OBJECT

public:
	/// Enum defining optimization options.
	enum Option { Maximum = 0, Minimum };

	/// Constructor.
	Q_INVOKABLE AMOptimizeControlActionInfo(const AMControlInfo &controlInfo, QObject *parent = 0);
	/// Copy constructor.
	AMOptimizeControlActionInfo(const AMOptimizeControlActionInfo &original);
	/// Destructor.
	virtual ~AMOptimizeControlActionInfo();

	/// Returns an AMActionInfo3 instance that is identical to this action info.
	virtual AMActionInfo3* createCopy() const;
	/// Returns this action info's description.
	virtual QString typeDescription() const { return "Control Optimization Action"; }

	/// Returns the current optimization option.
	AMOptimizeControlActionInfo::Option option() const { return option_; }
	/// Returns the info for the control to be optimized.
	AMControlInfo control() const { return controlInfo_; }
	/// Returns the minimum of the control value range to be optimized over.
	double minimum() const { return minimum_; }
	/// Returns the maximum of the control value range to be optimized over.
	double maximum() const { return maximum_; }

signals:
	/// Notifier that the optimization option has changed.
	void optionChanged(AMOptimizeControlActionInfo::Option newOption);
	/// Notifier that the control has changed.
	void controlChanged(const AMControlInfo &newInfo);
	/// Notifier that the minimum of the control value range has changed.
	void minimumChanged(double newMin);
	/// Notifier that the maximum of the control value range has changed.
	void maximumChanged(double newMax);

public slots:
	/// Sets the optimization option.
	void setOption(AMOptimizeControlActionInfo::Option newOption);
	/// Sets the control info.
	void setControl(const AMControlInfo &newInfo);
	/// Sets the minimum of the control value range to be optimized over.
	void setMinimum(double newMin);
	/// Sets the maximum of the control value range to be optimized over.
	void setMaximum(double newMax);

protected:
	/// The optimization option.
	Option option_;
	/// The info for the control to be optimized.
	AMControlInfo controlInfo_;
	/// The minimum of the control value range.
	double minimum_;
	/// The maximum of the control value range.
	double maximum_;
};

#endif // AMOPTIMIZECONTROLACTIONINFO_H
