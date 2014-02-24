#ifndef IDEASXRFSCANCONTROLLER_H
#define IDEASXRFSCANCONTROLLER_H

#include "acquaman/IDEAS/IDEASXRFScanConfiguration.h"
#include "acquaman/AMScanController.h"
#include "beamline/AMXRFDetector.h"
#include "dataman/VESPERS/AMXRFScan.h"

class IDEASXRFScanController : public AMScanController
{
    Q_OBJECT
public:
    /// Default constructor.
    virtual ~IDEASXRFScanController();
    IDEASXRFScanController(IDEASXRFScanConfiguration *scanConfig, QObject *parent = 0);


signals:

public slots:

protected slots:
    /// Helper slot to finish up a scan.
    void onDetectorAcquisitionFinished();


protected:
    /// Initializes the scan
    virtual bool initializeImplementation() { setInitialized(); return true; }
    /// Starts current scan.
    virtual bool startImplementation();
    /// Cancels current scan.  Treated as finishing early.
    virtual void cancelImplementation() { setCancelled(); }

    /// Saves the data after a scan is stopped.
    void saveData();

    // Member variables.
    /// XRF detector.
    AMXRFDetector *detector_;

};

#endif // IDEASXRFSCANCONTROLLER_H
