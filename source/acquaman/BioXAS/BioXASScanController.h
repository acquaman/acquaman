#ifndef BIOXASGENERALSCANCONTROLLER_H
#define BIOXASGENERALSCANCONTROLLER_H

#include "dataman/AMScan.h"

class AMControl;
class AMDetector;
class BioXAS32ElementGeDetector;

/*!
  This class is a non-QObject class that holds the common functions for BioXAS scan controllers.
  This class exists to bypass the limitation that you can't multiply inherit QObjects.
  */

class BioXASScanController
{
public:
	BioXASScanController();

	void setGeneralScanControllerScan(AMScan * scan) ;

	/// Returns true if the given detector is used in the scan.
	bool usingDetector(AMDetector *detector) const;

	/// Returns true if the beamline has an I0 detector and the I0 detector is used in the scan.
	bool usingI0Detector() const;
	/// Returns true if the beamline has an I1 detector and the I1 detector is used in the scan.
	bool usingI1Detector() const;
	/// Returns true if the beamline has an I2 detector and the I2 detector is used in the scan.
	bool usingI2Detector() const;

	/// Returns true if the beamline has the Ge detector and the Ge detector is used in the scan.
	bool usingGeDetector(BioXAS32ElementGeDetector *detector) const;

	/// Returns true if the beamline has a scaler and the scaler is used in the scan.
	bool usingScaler() const;

	/// Returns the data source for the given detector in the scan, if there is one.
	AMDataSource* detectorDataSource(AMDetector *detector) const;
	/// Returns the I0 detector source in the given scan, if there is one.
	AMDataSource* i0DetectorDataSource() const;
	/// Returns the I1 detector source in the given scan, if there is one.
	AMDataSource* i1DetectorDataSource() const;
	/// Returns the I2 detector source in the given scan, if there is one.
	AMDataSource* i2DetectorDataSource() const;
	/// Returns the diode detector source in the given scan, if there is one.
	AMDataSource* diodeDetectorDataSource() const;
	/// Returns the PIPS detector source in the given scan, if there is one.
	AMDataSource* pipsDetectorDataSource() const;
	/// Returns the Lytle detector source in the given scan, if there is one.
	AMDataSource* lytleDetectorDataSource() const;
	/// Returns the source for the given Ge detector in the scan, if there is one.
	AMDataSource* geDetectorDataSource(BioXAS32ElementGeDetector *detector) const;
	/// Returns the scaler dwell time detector source in the given scan, if there is one.
	AMDataSource* scalerDwellTimeDetectorDataSource() const;


protected:
	/*!
	  * The scan which the scan controller is controlling.
	  */
	AMScan *generalScan_;
};

#endif // BIOXASGENERALSCANCONTROLLER_H
