#ifndef IDEASXASSCANACTIONCONTROLLER_H
#define IDEASXASSCANACTIONCONTROLLER_H

#include "acquaman/AMRegionScanActionController.h"
#include "acquaman/IDEAS/IDEASXASScanConfiguration.h"

#include <QTimer>

class IDEASXASScanActionController : public AMRegionScanActionController
{
	Q_OBJECT

public:
	/// Constructor.  Builds a runnable scan action controller for the SGM beamline.
	IDEASXASScanActionController(IDEASXASScanConfiguration *cfg, QObject *parent = 0);
	virtual ~IDEASXASScanActionController();

protected:
	/// Reimplemented to provide actions that will setupd the beamine for optimzed operation of the XAS scan.
	AMAction3* createInitializationActions();
	/// Reimplemented to put the beamline in a good state after a scan has been completed.
	AMAction3* createCleanupActions();
	/// Re-implemented due to the extra complexity of setting up IDEAS.
	virtual void cancelImplementation();



	/// Sets the scan axis and adds anything extra.
	virtual void buildScanControllerImplementation();

        QTimer* pokeSyncDwell_;


protected:
	/// Specific scan configuration with all the SGM specific information inside.
	IDEASXASScanConfiguration *configuration_;

protected slots:
        void onSyncDwellStatusChanged(bool);
};

#endif // IDEASXASSCANACTIONCONTROLLER_H
