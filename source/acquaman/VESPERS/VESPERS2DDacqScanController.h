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
	virtual int xAxisCount() const { return xAxisCount_; }
	/// Returns the number of values in the second independent axis.
	virtual int yAxisCount() const { return yAxisCount_; }

	/// Returns the PV name that will be used for the x-axis.
	virtual QString xAxisPVName() const { return xAxisPVName_; }
	/// Returns the PV name that will be used for the y-axis.
	virtual QString yAxisPVName() const { return yAxisPVName_; }

protected:
	/// Specific implementation of the scan initialization.
	bool initializeImplementation();
	/// Specific implmentation of the scan start.
	bool startImplementation();

	/// Pointer to the VESPERS2DScanConfiguration this scan controls.
	VESPERS2DScanConfiguration *config_;

	/// Holds the x-axis PV name.
	QString xAxisPVName_;
	/// Holds the y-axis PV name.
	QString yAxisPVName_;
	/// Holds the x-axis count.
	int xAxisCount_;
	/// Holds the y-axis count.
	int yAxisCount_;
};

#endif // VESPERS2DDACQSCANCONTROLLER_H
