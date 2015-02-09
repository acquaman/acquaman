#ifndef BIOXASSIDEXRFSCANCONFIGURATION_H
#define BIOXASSIDEXRFSCANCONFIGURATION_H

#include "acquaman/AMScanConfiguration.h"

class BioXASSideXRFScanConfiguration : public AMScanConfiguration
{
	Q_OBJECT

	Q_CLASSINFO("AMDbObject_Attributes", "description=BioXAS Side XRF Scan Configuration")

public:
	/// Constructor.
	Q_INVOKABLE BioXASSideXRFScanConfiguration(QObject *parent = 0);
	/// Copy Constructor.
	BioXASSideXRFScanConfiguration(const BioXASSideXRFScanConfiguration &other);
	/// Destructor.
	virtual ~BioXASSideXRFScanConfiguration();

	/// Returns a new instance of the scan configuration.
	virtual AMScanConfiguration *createCopy() const;

	/// Returns a new instance of the scan controller.
	virtual AMScanController *createController();

	/// A human-readable description of this scan configuration. Can be re-implemented to provide more details.
	virtual QString description() const {
		return QString("XRF Free Run Scan");
	}

	/// A human-readable synopsis of this scan configuration. Can be re-implemented to proved more details. Used by scan action to set the main text in the action view.
	virtual QString detailedDescription() const;
};

#endif // BIOXASSIDEXRFSCANCONFIGURATION_H
