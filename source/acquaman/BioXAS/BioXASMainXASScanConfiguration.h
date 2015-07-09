#ifndef BIOXASMAINXASSCANCONFIGURATION_H
#define BIOXASMAINXASSCANCONFIGURATION_H

#include "acquaman/BioXAS/BioXASXASScanConfiguration.h"

class BioXASMainXASScanConfiguration : public BioXASXASScanConfiguration
{
	Q_OBJECT

	Q_CLASSINFO("AMDbObject_Attributes", "description=BioXAS Main XAS Scan Configuration")

public:
	/// Constructor.
	Q_INVOKABLE BioXASMainXASScanConfiguration(QObject *parent = 0);
	/// Copy Constructor.
	BioXASMainXASScanConfiguration(const BioXASMainXASScanConfiguration &original);
	/// Destructor.
	virtual ~BioXASMainXASScanConfiguration();

	/// A human-readable synopsis of this scan configuration.
	virtual QString detailedDescription() const { return "BioXAS Main XAS Scan"; }

	/// Returns a pointer to a newly-created copy of this scan configuration.  (It takes the role of a copy constructor, but is virtual so that our high-level classes can copy a scan configuration without knowing exactly what kind it is.)
	virtual AMScanConfiguration* createCopy() const;
	/// Returns a pointer to a newly-created AMScanController that is appropriate for executing this kind of scan configuration.  The controller should be initialized to use this scan configuration object as its scan configuration.  Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanController* createController();
};

#endif // BIOXASMAINXASSCANCONFIGURATION_H
