#ifndef BIOXASGENERICSTEPSCANCONFIGURATION_H
#define BIOXASGENERICSTEPSCANCONFIGURATION_H

#include "acquaman/AMGenericStepScanConfiguration.h"
#include "acquaman/BioXAS/BioXASScanConfiguration.h"

class BioXASGenericStepScanConfiguration : public AMGenericStepScanConfiguration, public BioXASScanConfiguration
{
    Q_OBJECT

	Q_PROPERTY(AMDbObject *configurationDbObject READ dbReadScanConfigurationDbObject WRITE dbWriteScanConfigurationDbObject)
	Q_PROPERTY(QString header READ headerText WRITE setHeaderText)

	Q_CLASSINFO("AMDbObject_Attributes", "description=BioXAS Generic Scan Configuration")

public:
	/// Constructor.
	Q_INVOKABLE BioXASGenericStepScanConfiguration(QObject *parent = 0);
	/// Copy constructor.
	BioXASGenericStepScanConfiguration(const BioXASGenericStepScanConfiguration &original);
	/// Destructor.
	virtual ~BioXASGenericStepScanConfiguration();

	/// Returns a newly-created copy of this scan configuration.  (It takes the role of a copy constructor, but is virtual so that our high-level classes can copy a scan configuration without knowing exactly what kind it is.)
	virtual AMScanConfiguration* createCopy() const;
	/// Returns a newly-created AMScanController that is appropriate for executing this kind of scan configuration.
	virtual AMScanController* createController();

	/// Returns whether the scan configuration is configured to use a GeDetector.
	bool usingAnyGeDetector() const;
	/// Returns whether the scan configuration is configured to use a scaler.
	bool usingScaler() const;
	/// Returns whether the scan configuration is configured to use a zebra.
	bool usingZebra() const;
	/// Returns whether the scan configuration is configured to use a mono.
	bool usingMono() const;

protected:
	/// Returns a string containing scan header information.
	virtual QString headerText() const { return QString(); }
};

#endif // BIOXASGENERICSTEPSCANCONFIGURATION_H
