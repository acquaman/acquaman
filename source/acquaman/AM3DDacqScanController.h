#ifndef AM3DDACQSCANCONTROLLER_H
#define AM3DDACQSCANCONTROLLER_H

#include "acquaman/AMDacqScanController.h"
#include "acquaman/AM3DScanConfiguration.h"

#define AM3DDACQSCANCONTROLLER_CANT_CREATE_OUTPUTHANDLER 74001
#define AM3DDACQSCANCONTROLLER_DACQ_INITIALIZATION_FAILED 74002
#define AM3DDACQSCANCONTROLLER_NO_X_COLUMN 74003
#define AM3DDACQSCANCONTROLLER_NO_Y_COLUMN 74004
#define AM3DDACQSCANCONTROLLER_NO_Z_COLUMN 74005
#define AM3DDACQSCANCONTROLLER_NO_SPECTRUM_FILE 74006

/// This class allows the building of a 3D map scan controller using the DACQ library.
/*!
	This extends the functionality of the single dacq scan controller by extending
	the capabilities to do scans over three independent axes.  There is a strict requirement
	that the map be a parallelpiped (rectangular in 3 dimensions) and to enforce this there are three pure virtual functions
	that must be implemented by subclasses in order for the class to be instantiated.

	Currently, the axes are issued priorities that can be either in any order.  The only assumption
	is that the order inside of the configuration file is the lower the priority, the slower that axis
	is changed.

	This is important because the start implementation will set the control information based
	on that specification.  An important distinction is that regardless of the priority order,
	the order of the first three data sources are still x then y then z.

	Another assumption is that the first three data sources are assumed to be the x, y, and z
	coordinates, respectively.  If anything else is in the first two columns then the
	data will not be stored correctly.  The remaining columns can be in any order.

	Because AM3DScanConfiguration was unable to subclass itself from AM2DScanConfiguration,
	it prevents AM3DDacqScanController from subclassing itself from AM2DDacqScanController.
	This requires a fair bit of code duplication, which is not ideal.
  */
class AM3DDacqScanController : public AMDacqScanController
{
	Q_OBJECT

public:
	/// Constructor.  Requires a valid AM3DScanConfiguration.
	AM3DDacqScanController(AM3DScanConfiguration *config, QObject *parent = 0);

	/// Pure virtual function.  Returns the PV name that will be used for the x-axis.
	virtual QString xAxisPVName() const = 0;
	/// Pure virtual function.  Returns the PV name that will be used for the y-axis.
	virtual QString yAxisPVName() const = 0;
	/// Pure virtual function.  Returns the PV name that will be used for the z-axis.
	virtual QString zAxisPVName() const = 0;

	// THIS SHOULD BE MODIFIED TO BE AN AXIS OF SOME KIND
	/// Returns whether this controller will be stopping at the end of the line or not.
	bool stoppingAtTheEndOfLine() { return stopAtEndOfLine_; }

public slots:
	// THIS SHOULD BE MODIFIED TO BE AN AXIS OF SOME KIND
	/// Tells the scan controller that it should stop at the end of the line.  Ie: stop when it reaches the end of the fast axis.
	void stopAtTheEndOfLine() { stopAtEndOfLine_ = true; }

protected:
	/// Sets the config file and returns whether it was successful or not at loading the file.  \note This should be called instead of advAcq_->setConfigFile() so that AM3DDacqScanController knows where the configuration files are.
	bool setConfigFile(const QString &filename);

	/// Re-implementing the start implementation to build in some dacq safety features that are needed over and above the normal single scan.  Contains duplicate code from AMDacqScanController::startImplementation().
	/*!
		One very important aspect of AM3DDacqScanController::startImplementation() is that it ensures that the controls for each axis are set correctly.
		It requires the start, delta, and end for each control to be set correctly before it is called because it will work the magic of getting the
		DACQ to read them in correctly.
	  */
	virtual bool startImplementation();
	/// Re-implementing the event method to be able to properly put the data where it should be after every data point is collected.
	virtual bool event(QEvent *e);
	/// Re-implementing the toScanIndex method to properly add the next point in 3D space.  Handles incrementing xPosition_ and yPosition_ as appropriate.
	virtual AMnDIndex toScanIndex(QMap<int, double> aeData);
	/// Method that fleshes out the scan's raw data store with all of the points it will need.
	void prefillScanPoints();

	// THIS SHOULD BE MODIFIED TO BE AN AXIS OF SOME KIND
	/// Returns whether the controller is at the end of the fast axis.  Requires aeData for up-to-date information.
	bool atEndOfLine(QMap<int, double> aeData) const;

	/// Holds the current position in the x axis.
	int xPosition_;
	/// Holds the current position in the y axis.
	int yPosition_;
	/// Holds the current position in the z axis.
	int zPosition_;

	/// List that holds the index of the priority axes.  Highest = 0, next = 1, Lowest = 2.
	QList<int> axisPriorities_;
	/// Holds the initial position of the highest priority axis.  Used for determining when to increment the slow axis.
	double firstPriorityAxisStartPosition_;
	/// Holds the initial position of the second priority axis.  Used for determining when to increment the slow axis.
	double secondPriorityAxisStartPosition_;

	/// Pointer to a 3D scan configuration.  Used inside this class.  The general AMScanController will get the regular AMScanConfiguration.
	AM3DScanConfiguration *internal3DConfig_;

	/// String that holds the file name (including path).
	QString filename_;

	// THIS SHOULD BE MODIFIED TO BE AN AXIS OF SOME KIND
	/// Flag that holds whether the controller should stop once the end of the current fast axis is finshed.
	bool stopAtEndOfLine_;

private:
	/// Private method that takes the current start's, delta's, and end's for each axis and sets them inside of the config file.  It then loads the configuration file again to ensure that the dacq has the correct values.
	bool setScanAxesControl();
};

#endif // AM3DDACQSCANCONTROLLER_H
