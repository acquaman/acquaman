#ifndef VESPERSSCANCONTROLLER_H
#define VESPERSSCANCONTROLLER_H

#include "acquaman/VESPERS/VESPERSScanConfiguration.h"
#include "dataman/AMScan.h"
#include "actions/AMBeamlineActionItem.h"
#include "actions/AMBeamlineListAction.h"

class QEpicsAdvAcq;

/*!
	This class contains many small helper methods that are used commonly amongst all of VESPERS Dacq scan controllers.
	These are primarily adding in PV names to the dacq controller, adding non-configurable datasources and measurements,
	and some common initialization and clean-up actions.
  */
class VESPERSScanController
{

public:
	/// Constructor.
	VESPERSScanController(VESPERSScanConfiguration *config);
	/// Destructor.  Makes sure all the memory from the actions that were created is freed.
	~VESPERSScanController() { onInitializationActionFinished(); onCleanupActionFinished(); }

protected:
	/// Helper method that builds the base initialization actions.
	void buildBaseInitializationAction(double timeStep);
	/// Helper method that builds all of the cleanup actions.
	void buildCleanupAction(bool usingMono);
	/// Helper method that cleans up the intializationAction.
	void onInitializationActionFinished();
	/// Helper method that cleans up the cleanupAction.
	void onCleanupActionFinished();

	/// Helper method that adds the standard extra PVs to the advAcq_.
	void addStandardExtraPVs(QEpicsAdvAcq *advAcq, bool addEaAndDwellTime, bool addK);
	/// Helper method that adds the dead time PV's to the advAcq_ that is provided for the single element vortex detector.
	void addSingleElementDeadTimePVs(QEpicsAdvAcq *advAcq);
	/// Helper method that adds the ROI PVs to the advAcq_ that is provided.
	void addSingleElementRegionsOfInterestPVs(QEpicsAdvAcq *advAcq, int roiCount);
	/// Helper method that adds the spectra PV's to the advAcq_ that is provided for the single element vortex detector.
	void addSingleElementSpectraPVs(QEpicsAdvAcq *advAcq);
	/// Helper method that adds the dead time PV's to the advAcq_ that is provided for the four element vortex detector.
	void addFourElementDeadTimePVs(QEpicsAdvAcq *advAcq);
	/// Helper method that adds the ROI PVs to the advAcq_ that is provided.
	void addFourElementRegionsOfInterestPVs(QEpicsAdvAcq *advAcq, int roiCount);
	/// Helper method that adds the spectra PV's to the advAcq_ that is provided for the four element vortex detector.
	void addFourElementSpectraPVs(QEpicsAdvAcq *advAcq);

	/// Helper method that adds the standard extra measurements and raw data sources.
	void addStandardMeasurements(AMScan *scan, bool addEaAndDwellTime, bool addK);
	/// Helper method that adds the measurements and raw data sources to the scan provided for the single element dead time information.
	void addSingleElementDeadTimeMeasurements(AMScan *scan);
	/// Helper method that adds the measurements and raw data sources for the spectra.
	void addSingleElementSpectraMeasurments(AMScan *scan, const AMMeasurementInfo &info);
	/// Helper method that adds the measurements and raw data sources to the scan provided for the four element dead time information.
	void addFourElementDeadTimeMeasurements(AMScan *scan);
	/// Helper method that adds the measurements and raw data sources for the spectra.
	void addFourElementSpectraMeasurments(AMScan *scan, const AMMeasurementInfo &info);

	/// Helper method that builds the standard set of notes added to every dacq scan.
	QString buildNotes();

	/// Pointer to the configuration.
	VESPERSScanConfiguration *config_;

	/// Action that contains all of the initialization actions for the controller.
	AMBeamlineListAction *intializationAction_;
	/// Action that contains all of the cleanup actions for the controller.
	AMBeamlineListAction *cleanupAction_;
};

#endif // VESPERSSCANCONTROLLER_H
