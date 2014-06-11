#ifndef BIOXASSIDEXASSCANACTIONCONTROLLER_H
#define BIOXASSIDEXASSCANACTIONCONTROLLER_H

#include "acquaman/AMStepScanActionController.h"

class BioXASSideXASScanConfiguration;

class BioXASSideXASScanActionController : public AMStepScanActionController
{
    Q_OBJECT

public:
    explicit BioXASSideXASScanActionController(BioXASSideXASScanConfiguration *configuration, QObject *parent = 0);

signals:

public slots:

protected:
    /// Reimplemented to provide actions that will setupd the beamine for optimzed operation of the XAS scan.
    AMAction3* createInitializationActions();
    /// Reimplemented to put the beamline in a good state after a scan has been completed.
    AMAction3* createCleanupActions();
    /// Re-implemented due to the extra complexity of setting up IDEAS.
    virtual void cancelImplementation();
    /// Reimplemented to populate scan initial conditions
    void onInitializationActionsListSucceeded();

    /// Sets the scan axis and adds anything extra.
    virtual void buildScanControllerImplementation();
    /// Reimplemented for EXAFS capabilities.  Creates the scan assembler that builds all the actions used to run the scan.
    virtual void createScanAssembler();


protected:
    BioXASSideXASScanConfiguration *configuration_;

};

#endif // BIOXASSIDEXASSCANACTIONCONTROLLER_H
