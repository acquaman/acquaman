#ifndef BIOXASBEAMLINESUPPORT
#define BIOXASBEAMLINESUPPORT

class AMScan;
class AMDataSource;
class AMControl;
class AMDetector;
class AMGenericStepScanConfiguration;
class BioXAS32ElementGeDetector;

namespace BioXASBeamlineSupport
{
	// Finding detectors and controls in configurations.

	/// Returns true if the beamline has an I0 detector and the I0 detector is used in the configuration.
	bool usingI0Detector(AMGenericStepScanConfiguration *configuration);
	/// Returns true if the beamline has an I1 detector and the I1 detector is used in the configuration.
	bool usingI1Detector(AMGenericStepScanConfiguration *configuration);
	/// Returns true if the beamline has an I2 detector and the I2 detector is used in the configuration.
	bool usingI2Detector(AMGenericStepScanConfiguration *configuration);
	/// Returns true if the beamline has a diode detector and the diode detector is used in the configuration.
	bool usingDiodeDetector(AMGenericStepScanConfiguration *configuration);
	/// Returns true if the beamline has a PIPS detector and the PIPS detector is used in the configuration.
	bool usingPIPSDetector(AMGenericStepScanConfiguration *configuration);
	/// Returns true if the beamline has a Lytle detector and the Lytle detector is used in the configuration.
	bool usingLytleDetector(AMGenericStepScanConfiguration *configuration);

	/// Returns true if the beamline has a scaler and the scaler is used in the configuration.
	bool usingScaler(AMGenericStepScanConfiguration *configuration);
	/// Returns true if the beamline has a Zebra and the Zebra is used in the configuration.
	bool usingZebra(AMGenericStepScanConfiguration *configuration);

	/// Returns true if the beamline has the Ge detector and the Ge detector is used in the configuration.
	bool usingGeDetector(AMGenericStepScanConfiguration *configuration, BioXAS32ElementGeDetector *detector);
	/// Returns true if any Ge detector is being used in the configuration.
	bool usingAnyGeDetector(AMGenericStepScanConfiguration *configuration);

	/// Returns true if the given detector is used in the configuration.
	bool usingDetector(AMGenericStepScanConfiguration *configuration, AMDetector *detector);

	/// Returns true if the beamline has a mono and it's used in the configuration.
	bool usingMono(AMGenericStepScanConfiguration *configuration);

	/// Returns true if the given control is used in the configuration.
	bool usingControl(AMGenericStepScanConfiguration *configuration, AMControl *control);

	/// Returns true if the beamline has a standards wheel and it's used in the configuration.
	bool usingStandardsWheel(AMGenericStepScanConfiguration *configuration);

	// Finding detectors and controls in scans.

	/// Returns true if the beamline has an I0 detector and the I0 detector is used in the scan.
	bool usingI0Detector(AMScan *scan);
	/// Returns true if the beamline has an I1 detector and the I1 detector is used in the scan.
	bool usingI1Detector(AMScan *scan);
	/// Returns true if the beamline has an I2 detector and the I2 detector is used in the scan.
	bool usingI2Detector(AMScan *scan);

	/// Returns true if the beamline has a diode detector and the diode detector is used in the scan.
	bool usingDiodeDetector(AMScan *scan);
	/// Returns true if the beamline has a PIPS detector and the PIPS detector is used in the scan.
	bool usingPIPSDetector(AMScan *scan);
	/// Returns true if the beamline has a Lytle detector and the Lytle detector is used in the scan.
	bool usingLytleDetector(AMScan *scan);

	/// Returns true if the beamline has a scaler and the scaler is used in the scan.
	bool usingScaler(AMScan *scan);
	/// Returns true if the beamline has a Zebra and the Zebra is used in the scan.
	bool usingZebra(AMScan *scan);

	/// Returns true if the beamline has the Ge detector and the Ge detector is used in the scan.
	bool usingGeDetector(AMScan *scan, BioXAS32ElementGeDetector *detector);
	/// Returns true if any Ge detector is being used in the scan.
	bool usingAnyGeDetector(AMScan *scan);

	/// Returns true if the given detector is used in the scan.
	bool usingDetector(AMScan *scan, AMDetector *detector);

	/// Returns the I0 detector source in the given scan, if there is one.
	AMDataSource* i0DetectorSource(AMScan *scan);
	/// Returns the I1 detector source in the given scan, if there is one.
	AMDataSource* i1DetectorSource(AMScan *scan);
	/// Returns the I2 detector source in the given scan, if there is one.
	AMDataSource* i2DetectorSource(AMScan *scan);
	/// Returns the diode detector source in the given scan, if there is one.
	AMDataSource* diodeDetectorSource(AMScan *scan);
	/// Returns the PIPS detector source in the given scan, if there is one.
	AMDataSource* pipsDetectorSource(AMScan *scan);
	/// Returns the Lytle detector source in the given scan, if there is one.
	AMDataSource* lytleDetectorSource(AMScan *scan);

	/// Returns the source for the given Ge detector in the scan, if there is one.
	AMDataSource* geDetectorSource(AMScan *scan, BioXAS32ElementGeDetector *detector);

	/// Returns the scaler dwell time detector source in the given scan, if there is one.
	AMDataSource* scalerDwellTimeDetectorSource(AMScan *scan);

	/// Returns the data source for the given detector in the scan, if there is one.
	AMDataSource* detectorSource(AMScan *scan, AMDetector *detector);
};

#endif // BIOXASBEAMLINESUPPORT
