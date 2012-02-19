#ifndef AM2DDACQSCANCONTROLLER_H
#define AM2DDACQSCANCONTROLLER_H

#include "acquaman/AMDacqScanController.h"

#define AM2DDACQSCANCONTROLLER_CANT_CREATE_OUTPUTHANDLER 73001
#define AM2DDACQSCANCONTROLLER_DACQ_INITIALIZATION_FAILED 73002
#define AM2DDACQSCANCONTROLLER_NO_X_COLUMN 73003
#define AM2DDACQSCANCONTROLLER_NO_Y_COLUMN 73004
#define AM2DDACQSCANCONTROLLER_NO_SPECTRUM_FILE 73005

/// This class allows the building of a 2D map scan controller using the DACQ library.
/*!
	This extends the functionality of the single dacq scan controller by extending
	the capabilities to do scans over two independent axes.  There is a strict requirement
	that the map be rectangular and to enforce this there are two pure virtual functions
	that must be implemented by subclasses in order for the class to be instantiated.

	Currently, there is also an assumption that the x-axis of the map is the "fast axis".
	This means that if doing a 2D map, it rasters across the screen along the x-axis and
	then only increments the y-axis (slow axis) after it finishes moving across the entire
	line.  It is conceivable that the y-axis could be the fast axis, but for now it is
	hard coded as such.
  */
class AM2DDacqScanController : public AMDacqScanController
{
	Q_OBJECT

public:
	/// Constructor.
	AM2DDacqScanController(AMScanConfiguration *cfg, QObject *parent = 0);

	/// Pure virtual functions.  Returns the number of values in the first independent axis.
	virtual int xAxisCount() const = 0;
	/// Pure virtual functions.  Returns the number of values in the second independent axis.
	virtual int yAxisCount() const = 0;

protected:
	/// Re-implementing the start implementation to build in some dacq safety features that are needed over and above the normal single scan.  Contains duplicate code from AMDacqScanController::startImplementation().
	virtual bool startImplementation();
	/// Re-implementing the event method to be able to properly put the data where it should be after every data point is collected.
	virtual bool event(QEvent *e);
	/// Re-implementing the toScanIndex method to properly add the next point in 2D space.  Handles incrementing xPosition_ and yPosition_ as appropriate.
	virtual AMnDIndex toScanIndex(QMap<int, double> aeData);

	/// Holds the current position in the x axis.
	int xPosition_;
	/// Holds the current position in the y axis.
	int yPosition_;
};

#endif // AM2DDACQSCANCONTROLLER_H
