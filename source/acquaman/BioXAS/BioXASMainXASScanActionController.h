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
	/// The BioXAS-specific scan configuration.
	BioXASMainXASScanConfiguration *mainConfiguration_;
};

#endif // BIOXASMAINXASSCANACTIONCONTROLLER_H
