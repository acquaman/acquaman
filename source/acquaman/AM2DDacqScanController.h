/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").
Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef AM2DDACQSCANCONTROLLER_H
#define AM2DDACQSCANCONTROLLER_H

//#include "acquaman/AMDacqScanController.h"
//#include "acquaman/AM2DScanConfiguration.h"

//#define AM2DDACQSCANCONTROLLER_CANT_CREATE_OUTPUTHANDLER 73001
//#define AM2DDACQSCANCONTROLLER_DACQ_INITIALIZATION_FAILED 73002
//#define AM2DDACQSCANCONTROLLER_NO_X_COLUMN 73003
//#define AM2DDACQSCANCONTROLLER_NO_Y_COLUMN 73004
//#define AM2DDACQSCANCONTROLLER_NO_SPECTRUM_FILE 73005

///// This class allows the building of a 2D map scan controller using the DACQ library.
///*!
//	This extends the functionality of the single dacq scan controller by extending
//	the capabilities to do scans over two independent axes.  There is a strict requirement
//	that the map be rectangular and to enforce this there are two pure virtual functions
//	that must be implemented by subclasses in order for the class to be instantiated.

//	Currently, the fast and slow axis can be either the x-axis or y-axis.  The only assumption
//	is that the order inside of the configuration file is:

//		1) Slow axis
//		2) Fast axis.

//	This is important because the start implementation will set the control information based
//	on that specification.  An important distinction is that the even if the fast and slow axes
//	are switched, the order of the first two data sources are still x then y.

//	Another assumption is that the first two data sources are assumed to be the x and y
//	coordinates, respectively.  If anything else is in the first two columns then the
//	data will not be stored correctly.  The remaining columns can be in any order.
//  */
//class AM2DDacqScanController : public AMDacqScanController
//{
//	Q_OBJECT

//public:
//	/// Constructor.
//	AM2DDacqScanController(AM2DScanConfiguration *cfg, QObject *parent = 0);

//	/// Pure virtual function.  Returns the PV name that will be used for the x-axis.
//	virtual QString xAxisPVName() const = 0;
//	/// Pure virtual function.  Returns the PV name that will be used for the y-axis.
//	virtual QString yAxisPVName() const = 0;

//	/// Returns whether this controller will be stopping at the end of the line or not.
//	bool stoppingAtTheEndOfLine() { return stopAtEndOfLine_; }

//public slots:
//	/// Tells the scan controller that it should stop at the end of the line.  Ie: stop when it reaches the end of the fast axis.
//	void stopAtTheEndOfLine() { stopAtEndOfLine_ = true; }

//protected:
//	/// Sets the config file and returns whether it was successful or not at loading the file.  \note This should be called instead of advAcq_->setConfigFile() so that AM2DDacqScanController knows where the configuration files are.
//	bool setConfigFile(const QString &filename);

//	/// Re-implementing the start implementation to build in some dacq safety features that are needed over and above the normal single scan.  Contains duplicate code from AMDacqScanController::startImplementation().
//	/*!
//		One very important aspect of AM2DDacqScanController::startImplementation() is that it ensures that the controls for each axis are set correctly.
//		It requires the start, delta, and end for each control to be set correctly before it is called because it will work the magic of getting the
//		DACQ to read them in correctly.
//	  */
//	virtual bool startImplementation();
//	/// Re-implementing the event method to be able to properly put the data where it should be after every data point is collected.
//	virtual bool event(QEvent *e);
//	/// Re-implementing the toScanIndex method to properly add the next point in 2D space.  Handles incrementing xPosition_ and yPosition_ as appropriate.
//	virtual AMnDIndex toScanIndex(QMap<int, double> aeData);
//	/// Method that fleshes out the scan's raw data store with all of the points it will need.
//	void prefillScanPoints();

//	/// Returns whether the controller is at the end of the fast axis.  Requires aeData for up-to-date information.
//	bool atEndOfLine(QMap<int, double> aeData) const;

//	/// Holds the current position in the x axis.
//	int xPosition_;
//	/// Holds the current position in the y axis.
//	int yPosition_;
//	/// Holds the initial position of the fast axis.  Used for determining when to increment the slow axis.
//	double fastAxisStartPosition_; // This should be able to be removed.  It should be the same as the start position in AM2DScanConfiguration.

//	/// Pointer to a 2D scan configuration.  Used inside this class.  The general AMScanController will get the regular AMScanConfiguration.
//	AM2DScanConfiguration *internal2DConfig_;

//	/// String that holds the file name (including path).
//	QString filename_;

//	/// Flag that holds whether the controller should stop once the end of the current fast axis is finshed.
//	bool stopAtEndOfLine_;

//private:
//	/// Private method that takes the current start's, delta's, and end's for each axis and sets them inside of the config file.  It then loads the configuration file again to ensure that the dacq has the correct values.
//	bool setScanAxesControl();
//};

#endif // AM2DDACQSCANCONTROLLER_H
