#ifndef VESPERSSCANCONTROLLER_H
#define VESPERSSCANCONTROLLER_H

#include "acquaman/VESPERS/VESPERSScanConfiguration.h"
#include "dataman/AMScan.h"
#include "actions3/AMAction3.h"
#include "dataman/info/AMDetectorInfoSet.h"

//class QEpicsAdvAcq;

/*!
	This class contains many small helper methods that are used commonly amongst all of VESPERS Dacq scan controllers.
	These are primarily adding in PV names to the dacq controller, adding non-configurable datasources and measurements,
	and some common initialization and clean-up actions.
  */
class VESPERSScanController
{

public:
	/// Constructor.
	VESPERSScanController(VESPERSScanConfiguration *configuration);

protected:
	/// Helper method that builds the base initialization actions.
	AMAction3 *buildBaseInitializationAction(const AMDetectorInfoSet &detectorConfigurations);
	/// Helper method that builds the CCD file path, name, and number for the beginning of a scan.  Requires the detector enum, ccd file name from the configuration, AND must be called after buildInitializationActions() because it assumes the list has already been created.
	AMAction3 *buildCCDInitializationAction(VESPERS::CCDDetector ccdChoice, const QString &ccdName);
	/// Helper method that builds all of the cleanup actions.
	AMAction3 *buildCleanupAction(bool usingMono);

//	/// Helper method that adds the standard extra PVs to the advAcq_.
//	void addStandardExtraPVs(QEpicsAdvAcq *advAcq, bool addEaAndDwellTime, bool addK);
//	/// Helper method that adds the dead time PV's to the advAcq_ that is provided for the single element vortex detector.
//	void addSingleElementDeadTimePVs(QEpicsAdvAcq *advAcq);
//	/// Helper method that adds the ROI PVs to the advAcq_ that is provided.
//	void addSingleElementRegionsOfInterestPVs(QEpicsAdvAcq *advAcq, int roiCount);
//	/// Helper method that adds the spectra PV's to the advAcq_ that is provided for the single element vortex detector.
//	void addSingleElementSpectraPVs(QEpicsAdvAcq *advAcq);
//	/// Helper method that adds the dead time PV's to the advAcq_ that is provided for the four element vortex detector.
//	void addFourElementDeadTimePVs(QEpicsAdvAcq *advAcq);
//	/// Helper method that adds the ROI PVs to the advAcq_ that is provided.
//	void addFourElementRegionsOfInterestPVs(QEpicsAdvAcq *advAcq, int roiCount);
//	/// Helper method that adds the spectra PV's to the advAcq_ that is provided for the four element vortex detector.
//	void addFourElementSpectraPVs(QEpicsAdvAcq *advAcq);

	/// Helper method that adds the standard extra measurements and raw data sources.
	void addStandardMeasurements(AMScan *scan, bool addEaAndDwellTime, bool addK);
	/// Helper method that adds the measurements and raw data sources to the scan provided for the single element dead time information.
	void addSingleElementDeadTimeMeasurements(AMScan *scan);
	/// Helper method that adds the measurements and raw data sources to the scan using the provided ROI info list.  The add suffix is for when the name should be personalized with "-1el".
	void addSingleElementRegionsOfInterestMeasurements(AMScan *scan, AMROIInfoList list, bool addSuffix);
	/// Helper method that adds the measurements and raw data sources for the spectra.
	void addSingleElementSpectraMeasurments(AMScan *scan, const AMMeasurementInfo &info);
	/// Helper method that adds the measurements and raw data sources to the scan provided for the four element dead time information.
	void addFourElementDeadTimeMeasurements(AMScan *scan);
	/// Helper method that adds the measurements and raw data sources to the scan using the provided ROI info list.  The add suffix is for when the name should be personalized with "-1el".
	void addFourElementRegionsOfInterestMeasurements(AMScan *scan, AMROIInfoList list, bool addSuffix);
	/// Helper method that adds the measurements and raw data sources for the spectra.
	void addFourElementSpectraMeasurments(AMScan *scan, const AMMeasurementInfo &info);

	/// Helper method that checks the \param name provided for the CCD in \param path for uniqueness.  If it is unique it returns the provided string, otherwise it creates a unique "-xxx" to the end until a valid name is found.
	QString getUniqueCCDName(const QString &path, const QString &name) const;

	/// Helper method that builds the standard set of notes added to every dacq scan.
	QString buildNotes();

	/// Pointer to the configuration.
	VESPERSScanConfiguration *config_;
};

#endif // VESPERSSCANCONTROLLER_H
