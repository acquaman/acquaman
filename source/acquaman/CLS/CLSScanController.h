#ifndef CLSSCANCONTROLLER_H
#define CLSSCANCONTROLLER_H

#include <QList>

#include "acquaman/CLS/CLSScanConfiguration.h"

class AMScan;
class AMXRFDetector;
class AMDataSource;

class CLSScanController
{
public:
	CLSScanController(CLSScanConfiguration *configuration);

	void setContollerScanInstance(AMScan *scan) { controllerScanInstance_ = scan; }

protected:
	void build2DXRFAnalysisBlock(AMXRFDetector *xrfDetector, const AMDataSource *spectraSource, const QList<AMDataSource *> &i0Sources);

protected:
	/*!
	  * The scan which the scan controller is controlling.
	  */
	AMScan *controllerScanInstance_;

	CLSScanConfiguration* controllerScanconfiguration_;
};

#endif // CLSSCANCONTROLLER_H
