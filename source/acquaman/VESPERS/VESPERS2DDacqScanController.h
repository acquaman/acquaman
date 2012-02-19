#ifndef VESPERS2DDACQSCANCONTROLLER_H
#define VESPERS2DDACQSCANCONTROLLER_H

#include "acquaman/AM2DDacqScanController.h"
#include "acquaman/VESPERS/VESPERS2DScanConfiguration.h"

class VESPERS2DDacqScanController : public AM2DDacqScanController
{
	Q_OBJECT

public:
	/// Constructor.  \param cfg is the 2D scan configuration that the controller will run.
	VESPERS2DDacqScanController(VESPERS2DScanConfiguration *cfg, QObject *parent = 0);

	/// Returns the number of values in the first independent axis.
	virtual int xAxisCount() const { return 0; }
	/// Returns the number of values in the second independent axis.
	virtual int yAxisCount() const { return 0; }

protected:
	/// Specific implementation of the scan initialization.
	bool initializeImplementation() { return false; }
	/// Specific implmentation of the scan start.
	bool startImplementation() { return false; }
};

#endif // VESPERS2DDACQSCANCONTROLLER_H
