#ifndef BIOXASMAINXASSCANACTIONCONTROLLER_H
#define BIOXASMAINXASSCANACTIONCONTROLLER_H

#include "acquaman/BioXAS/BioXASXASScanActionController.h"

class BioXASMainXASScanConfiguration;

class BioXASMainXASScanActionController : public BioXASXASScanActionController
{
    Q_OBJECT

public:
    /// Constructor.
    explicit BioXASMainXASScanActionController(BioXASMainXASScanConfiguration *configuration, QObject *parent = 0);
    /// Destructor.
    virtual ~BioXASMainXASScanActionController();

protected:
	/// Reimplemented to provide actions that will setup the beamine for optimized operation of the XAS scan.
	virtual AMAction3* createInitializationActions();
	/// Reimplemented to put the beamline in a good state after a scan has been completed.
	virtual AMAction3* createCleanupActions();

protected:
	/// The BioXAS-specific scan configuration.
    BioXASMainXASScanConfiguration *configuration_;
};

#endif // BIOXASMAINXASSCANACTIONCONTROLLER_H
