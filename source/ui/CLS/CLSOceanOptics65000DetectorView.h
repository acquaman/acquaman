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


#ifndef CLSOCEANOPTICS65000DETECTORVIEW_H
#define CLSOCEANOPTICS65000DETECTORVIEW_H

#include "ui/beamline/AMDetectorView.h"
#include "beamline/CLS/CLSOceanOptics65000Detector.h"

#include "MPlot/MPlot.h"
#include "MPlot/MPlotWidget.h"
#include "MPlot/MPlotSeries.h"
#include "MPlot/MPlotPoint.h"

class CLSOceanOptics65000BriefDetectorView : public AMBriefDetectorView
{
Q_OBJECT
public:
	/// General invokable constructor
	Q_INVOKABLE explicit CLSOceanOptics65000BriefDetectorView(CLSOceanOptics65000Detector *detector = 0, bool configureOnly = false, QWidget *parent = 0);

	/// Returns pointer to the detector object
	AMDetector* detector();

protected:
	/// Control editor to display the current reading
	AMExtendedControlEditor *readingCE_;
	QHBoxLayout *hl_;
	/// Pointer to the detector
	CLSOceanOptics65000Detector *detector_;

	/// We are trusting createDetectorView to pass in the correct type of detector, sub classes should trust AMDetector is actually their type
	bool setDetector(AMDetector *detector, bool configureOnly = false);
};

//class CLSOceanOptics65000DetailedDetectorView : public AMDetailedDetectorView
//{
//Q_OBJECT
//public:
//	/// General invokable constructor
//	Q_INVOKABLE explicit CLSOceanOptics65000DetailedDetectorView(CLSOceanOptics65000Detector *detector = 0, bool configureOnly = false, QWidget *parent = 0);

//	/// Returns pointer to the detector object
//	AMDetector *detector();

//	/// The view is managing this created object, hook up to destroyed() if you need long-term notification
//	AMDetectorInfo* configurationSettings() const;

//protected slots:
//	/// Handles when the control setpoint is requested
//	void onControlSetpointRequested();

//protected:
//	/// Copies the settings from the detector
//	void setConfigurationSettingsFromDetector();

//protected:
//	/// Pointer to the detector
//	CLSOceanOptics65000Detector *detector_;
//	/// Pointer to the detector settings
//	CLSOceanOptics65000DetectorInfo *configurationSettings_;
//	QGridLayout *gl_;
//	/// Control editor to display the current reading
//	AMExtendedControlEditor *readingCE_;
//	/// Control editor to read/set the integration time
//	AMExtendedControlEditor *integrationTimeCE_;

//	/// We are trusting createDetectorView to pass in the correct type of detector, sub classes should trust AMDetector is actually their type
//	bool setDetector(AMDetector *detector, bool configureOnly = false);
//};

/// This class builds a detailed view for the OceanOptics Optical Spectrometer detector.
class CLSOceanOptics65000DetailedDetectorView : public AMDetailedDetectorView
{
Q_OBJECT
public:
	/// Default constructor. Can build with a null pointer if necessary.
	Q_INVOKABLE explicit CLSOceanOptics65000DetailedDetectorView(CLSOceanOptics65000Detector *detector = 0, bool configureOnly = false, QWidget *parent = 0);

	/// Returns a pointer to the detector being view
	AMDetector* detector();
	/// The view is managing this created object, hook up to destroyed() if you need long-term notification
	AMDetectorInfo* configurationSettings() const;
	/// Returns a pointer to the plot
	MPlot* plot() const;

protected slots:
	/// Handles changes in the connection status
	void onConnectedChanged(bool connected);
	/// Handles changes in the status
	void onStatusChanged(bool status);
	/// Handles changes in the spectra's total counts
	void onTotalCountsChanged(double totalCounts);

	/// Handles button clicks from the start acqusition button
	void onStartAcquisitionButtonClicked();

protected:
	/*! Sets up the view based with the given detector.
	 We are trusting createDetectorView to pass in the correct type of detector, sub classes should trust AMDetector is actually their type. */
	bool setDetector(AMDetector *detector, bool configureOnly);

protected:
	/// The pointer to the detector
	CLSOceanOptics65000Detector *detector_;
	CLSOceanOptics65000DetectorInfo *configurationSettings_;

	/// The status indicator
	QLabel *statusLabel_;
	/// Control editor to read/set the integration time
	AMExtendedControlEditor *integrationTimeCE_;
	/// The total counts in the entire spectra
	QDoubleSpinBox *totalCountsDSB_;
	/// The start acquisition button
	QPushButton *startAcquisitionButton_;

	/// This is the plot widget that holds the plot used for viewing the spectra.
	MPlotWidget *view_;
	/// This is the plot itself.
	MPlot *plot_;
	/// This holds the detector's spectrum data
	MPlotSeriesBasic *spectrumData_;
};

#endif // CLSOCEANOPTICS65000DETECTORVIEW_H
