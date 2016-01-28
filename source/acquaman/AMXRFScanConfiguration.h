#ifndef AMXRFSCANCONFIGURATION_H
#define AMXRFSCANCONFIGURATION_H

#include "acquaman/AMScanConfiguration.h"

/// This class contains the necessary information to do an XRF scan (single acquisition of an XRF detector).
class AMXRFScanConfiguration : public AMScanConfiguration
{
	Q_OBJECT

	Q_CLASSINFO("AMDbObject_Attributes", "description=XRF Scan Configuration")

public:
	/// Constructor.
	Q_INVOKABLE AMXRFScanConfiguration(QObject *parent = 0);
	/// Copy constructor.
	AMXRFScanConfiguration(const AMXRFScanConfiguration &original);
	/// Destructor.
	virtual ~AMXRFScanConfiguration();

	/// Returns a new instance of the scan configuration.
	virtual AMScanConfiguration *createCopy() const;
	/// Returns a new instance of the scan controller.
	virtual AMScanController *createController();

	/// A human-readable description of this scan configuration.
	virtual QString description() const;
	/// A human-readable synopsis of this scan configuration.
	virtual QString detailedDescription() const;
};

#endif // AMXRFSCANCONFIGURATION_H
