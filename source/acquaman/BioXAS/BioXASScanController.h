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

	/// Returns true if the beamline has a scaler and the scaler is used in the scan.
	bool usingScaler() const;

	/// Returns the data source for the given detector in the scan, if there is one.
	AMDataSource* detectorDataSource(AMDetector *detector) const;

protected:
	/*!
	  * The scan which the scan controller is controlling.
	  */
	AMScan *generalScan_;
};

#endif // BIOXASGENERALSCANCONTROLLER_H
