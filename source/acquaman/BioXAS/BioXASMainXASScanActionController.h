#ifndef BIOXASMAINXASSCANACTIONCONTROLLER_H
#define BIOXASMAINXASSCANACTIONCONTROLLER_H

#include <QObject>

#include "acquaman/AMStepScanActionController.h"

class BioXASMainXASScanConfiguration;

class BioXASMainXASScanActionController : public AMStepScanActionController
{
    Q_OBJECT

public:
    /// Constructor.
    explicit BioXASMainXASScanActionController(BioXASMainXASScanConfiguration *configuration, QObject *parent = 0);
    /// Destructor.
    virtual ~BioXASMainXASScanActionController();

protected:
	/// Provides details for the beamline current settings.
	QString beamlineSettings();
	/// Reimplemented to provide actions that will setupd the beamine for optimzed operation of the XAS scan.
	AMAction3* createInitializationActions();
	/// Reimplemented to put the beamline in a good state after a scan has been completed.
	AMAction3* createCleanupActions();
	/// Sets the scan axis and adds anything extra.
	virtual void buildScanControllerImplementation();
	/// Reimplemented for EXAFS capabilities.  Creates the scan assembler that builds all the actions used to run the scan.
	virtual void createScanAssembler();

protected:
    BioXASMainXASScanConfiguration *configuration_;

};

#endif // BIOXASMAINXASSCANACTIONCONTROLLER_H