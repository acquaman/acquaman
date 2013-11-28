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


#ifndef CLSPGTDETECTORVIEW_H
#define CLSPGTDETECTORVIEW_H

//#include "MPlot/MPlot.h"
//#include "MPlot/MPlotWidget.h"
//#include "MPlot/MPlotSeries.h"
//#include "MPlot/MPlotPoint.h"

//#include "ui/beamline/AMOldDetectorView.h"
//#include "beamline/CLS/CLSPGTDetector.h"

//class QToolButton;

//class CLSPGTBriefDetectorView : public AMBriefOldDetectorView
//{
//Q_OBJECT
//public:
//	Q_INVOKABLE explicit CLSPGTBriefDetectorView(CLSPGTDetector *detector = 0, bool configureOnly = false, QWidget *parent = 0);

//	AMOldDetector* detector();

//protected slots:
//	void onPoweredOnChanged(bool poweredOn);

//protected:
//	AMExtendedControlEditor *readingCE_;
//	QToolButton *powerState_;
//	QIcon powerOnState_, powerOffState_;
//	QHBoxLayout *hl_;
//	CLSPGTDetector *detector_;

//	/// We are trusting createDetectorView to pass in the correct type of detector, sub classes should trust AMDetector is actually their type
//	bool setDetector(AMOldDetector *detector, bool configureOnly = false);
//};

////class CLSPGTDetailedDetectorView : public AMDetailedDetectorView
////{
////Q_OBJECT
////public:
////	Q_INVOKABLE explicit CLSPGTDetailedDetectorView(CLSPGTDetector *detector = 0, bool configureOnly = false, QWidget *parent = 0);

////	/* NTBA March 14, 2011 David Chevrier
////	   Needs a destructor for configurationSettings_
////	   */

////	AMDetector* detector();

////	/// The view is managing this created object, hook up to destroyed() if you need long-term notification
////	AMDetectorInfo* configurationSettings() const;

////protected slots:
////	void onControlSetpointRequested();

////protected:
////	void setConfigurationSettingsFromDetector();

////protected:
////	CLSPGTDetector *detector_;
////	CLSPGTDetectorInfo *configurationSettings_;
////	QGridLayout *gl_;
////	AMExtendedControlEditor *readingCE_;
////	AMExtendedControlEditor *hvCE_;
////	AMExtendedControlEditor *integrationModeCE_;
////	AMExtendedControlEditor *integrationTimeCE_;

////	/// We are trusting createDetectorView to pass in the correct type of detector, sub classes should trust AMDetector is actually their type
////	bool setDetector(AMDetector *detector, bool configureOnly = false);
////};

///// This class builds a detailed view for the PGT Sahara SDD detector.
//class CLSPGTDetailedDetectorView : public AMDetailedOldDetectorView
//{
//Q_OBJECT
//public:
//	/// Default constructor. Can build with a null pointer if necessary.
//	Q_INVOKABLE explicit CLSPGTDetailedDetectorView(CLSPGTDetector *detector = 0, bool configureOnly = false, QWidget *parent = 0);

//	/// Returns a pointer to the detector being view
//	AMOldDetector* detector();
//	/// The view is managing this created object, hook up to destroyed() if you need long-term notification
//	AMOldDetectorInfo* configurationSettings() const;
//	/// Returns a pointer to the plot
//	MPlot* plot() const;

//protected slots:
//	/// Handles changes in the connection status
//	void onConnectedChanged(bool connected);
//	/// Handles changes in the status
//	void onStatusChanged(bool status);

//	/// Handles changes in the spectra's total counts
//	void onTotalCountsChanged(double totalCounts);

//	/// Handles button clicks from the start acqusition button
//	void onStartAcquisitionButtonClicked();

//protected:
//	/*! Sets up the view based with the given detector.
//	 We are trusting createDetectorView to pass in the correct type of detector, sub classes should trust AMDetector is actually their type. */
//	bool setDetector(AMOldDetector *detector, bool configureOnly);

//protected:
//	/// The pointer to the detector
//	CLSPGTDetector *detector_;
//	CLSPGTDetectorInfo *configurationSettings_;

//	/// The status indicator
//	QLabel *statusLabel_;

//	/// The integration time control editor
//	AMExtendedControlEditor *integrationTimeCE_;
//	/// The integration mode control editor
//	AMExtendedControlEditor *integrationModeCE_;

//	/// The total counts in the entire spectra
//	QDoubleSpinBox *totalCountsDSB_;

//	/// The start acquisition button
//	QPushButton *startAcquisitionButton_;

//	/// This is the plot widget that holds the plot used for viewing the spectra.
//	MPlotWidget *view_;
//	/// This is the plot itself.
//	MPlot *plot_;
//	/// This holds the detector's spectrum data
//	MPlotSeriesBasic *spectrumData_;
//};


#endif // CLSPGTDETECTORVIEW_H
