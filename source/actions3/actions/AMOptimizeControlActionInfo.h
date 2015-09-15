#ifndef AMOPTIMIZECONTROLACTIONINFO_H
#define AMOPTIMIZECONTROLACTIONINFO_H

#include "actions3/AMListActionInfo3.h"
#include "acquaman/AMGenericStepScanConfiguration.h"

class AMOptimizeControlActionInfo : public AMListActionInfo3
{
    Q_OBJECT

public:
	/// Enum defining optimization options.
	enum Option { Maximum = 0 };

	/// Constructor.
	Q_INVOKABLE AMOptimizeControlActionInfo(AMGenericStepScanConfiguration *configuration, QObject *parent = 0);
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
	/// Returns the configuration.
	AMGenericStepScanConfiguration* configuration() const  { return configuration_; }

signals:
	/// Notifier that the optimization option has changed.
	void optionChanged(AMOptimizeControlActionInfo::Option newOption);
	/// Notifier that the configuration has changed.
	void configurationChanged(AMGenericStepScanConfiguration *newConfiguration);

public slots:
	/// Sets the optimization option.
	void setOption(AMOptimizeControlActionInfo::Option newOption);
	/// Sets the configuration.
	void setConfiguration(AMGenericStepScanConfiguration *newConfiguration);

protected:
	/// The optimization option.
	Option option_;
	/// The configuration.
	AMGenericStepScanConfiguration *configuration_;
};

#endif // AMOPTIMIZECONTROLACTIONINFO_H
