#ifndef AMCONTINUOUSSCANACTIONCONTROLLER_H
#define AMCONTINUOUSSCANACTIONCONTROLLER_H

#include "acquaman/AMScanActionController.h"
#include "acquaman/AMContinuousScanConfiguration.h"
#include "dataman/AMUser.h"

#include <QThread>
#include <QTimer>

class AMScanActionControllerScanAssembler;
class AMListAction3;

class AMDSClientDataRequest;

#include "beamline/SGM/energy/SGMGratingSupport.h"

#define AMCONTINUOUSSCANACTIONCONTROLLER_COULD_NOT_ADD_DETECTOR 285000

/// This class is the base class for all continuous based scan controllers.
class AMContinuousScanActionController : public AMScanActionController
{
	Q_OBJECT

public:
	/// Constructor.  Takes an AMContinuousScanConfiguration.
	AMContinuousScanActionController(AMContinuousScanConfiguration *configuration, QObject *parent = 0);
	/// Destructor.
	virtual ~AMContinuousScanActionController();

	/// Method that builds all the general aspects, such as measurements and raw data sources, and the file writer capabilities for the scan controller.
	virtual void buildScanController();

signals:

public slots:

protected slots:
	/// Helper slot that tells AMCDFDataStore to flush it's contents to disk.  This prevents it from corrupting itself.
	void flushCDFDataStoreToDisk();

protected:
	/// Implementation to ensure that the data acquisition event is caught and handled.
	virtual bool event(QEvent *e);
	/// Creates the scan assembler that builds all the actions used to run the scan.
	virtual void createScanAssembler();

	double energyFromGrating(SGMGratingSupport::GratingTranslation gratingTranslationSelection, double gratingAngleEncoderTarget) const;

	/// The assembler that takes in the region scan configuration and turns it into a tree of scanning actions.
	AMScanActionControllerScanAssembler *scanAssembler_;
	/// The pointer to the region scan configuration to provide the subclass information.
	AMContinuousScanConfiguration *continuousConfiguration_;

	/// A timer used when using AMCDFDataStore.  After a timeout it flushes the contents to disk.
	QTimer flushToDiskTimer_;

	AMDSClientDataRequest *oneClientDataRequest_;
	AMnDIndex insertionIndex_;
};

#endif // AMCONTINUOUSSCANACTIONCONTROLLER_H
