#ifndef BIOXASXASSCANACTIONCONTROLLER_H
#define BIOXASXASSCANACTIONCONTROLLER_H

#include "acquaman/AMGenericStepScanController.h"

#include "application/AMAppControllerSupport.h"
#include "application/BioXAS/BioXAS.h"

#include "dataman/export/AMExporterXDIFormat.h"
#include "dataman/export/AMExporterOptionXDIFormat.h"

class BioXASXASScanConfiguration;

class BioXASXASScanActionController : public AMGenericStepScanController
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASXASScanActionController(BioXASXASScanConfiguration *configuration, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASXASScanActionController();

protected:
	/// Creates the scan assembler that builds all the actions used to run the scan.
	virtual void createScanAssembler();

	/// Sets the scan axis and adds anything extra.
	virtual void buildScanControllerImplementation();

protected:
	/// The BioXAS XAS scan configuration.
	BioXASXASScanConfiguration *bioXASConfiguration_;
};

#endif // BIOXASXASSCANACTIONCONTROLLER_H
