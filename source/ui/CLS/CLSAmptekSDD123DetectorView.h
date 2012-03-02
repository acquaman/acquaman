#ifndef CLSAMPTEKSDD123DETECTORVIEW_H
#define CLSAMPTEKSDD123DETECTORVIEW_H

#include "ui/beamline/AMDetectorView.h"
#include "beamline/CLS/CLSAmptekSDD123Detector.h"

#include "MPlot/MPlot.h"
#include "MPlot/MPlotWidget.h"
#include "MPlot/MPlotSeries.h"
#include "MPlot/MPlotPoint.h"

#include <QLabel>
#include <QDoubleSpinBox>
#include <QPushButton>

/// This class builds a very brief view for the Amptek SDD 123 detector. Its primary function is to show that the detector is working.
class CLSAmptekSDD123BriefDetectorView : public AMBriefDetectorView
{
Q_OBJECT

public:
	/// Default constructor. Can build a view with a null pointer (ie: not a useful view) if necessary.
	Q_INVOKABLE explicit CLSAmptekSDD123BriefDetectorView(CLSAmptekSDD123Detector *detector = 0, bool configureOnly = false, QWidget *parent = 0);

	/// Returns a pointer to the detector being viewed
	AMDetector* detector();

protected:
	/*! Sets up the view based with the given detector.
	 We are trusting createDetectorView to pass in the correct type of detector, sub classes should trust AMDetector is actually their type. */
	bool setDetector(AMDetector *detector, bool configureOnly);

	/// The pointer to the detector
	CLSAmptekSDD123Detector *detector_;
};


/// This class builds a detailed view for the Amptek SDD 123 detector.
class CLSAmptekSDD123DetailedDetectorView : public AMDetailedDetectorView
{
Q_OBJECT
public:
	/// Default constructor. Can build with a null pointer if necessary.
	Q_INVOKABLE explicit CLSAmptekSDD123DetailedDetectorView(CLSAmptekSDD123Detector *detector = 0, bool configureOnly = false, QWidget *parent = 0);

	/// Returns a pointer to the detector being view
	AMDetector* detector();
	/// Returns a pointer to the plot
	MPlot* plot() const;

protected slots:
	/// Handles changes in the connection status
	void onConnectedChanged(bool connected);
	/// Handles changes in the status
	void onStatusChanged(bool status);
	/// Handles changes in the integration time
	void onIntegrationTimeChanged(double integrationTime);
	/// Handles changes in the detector temperature
	void onDetectorTemperatureChanged(double detectorTemperature);
	/// Handles changes in the number of MCA Channels
	void onMCAChannelsChanged(double mcaChannels);

	/// Handles button clicks from the start acqusition button
	void onStartAcquisitionButtonClicked();

protected:
	/*! Sets up the view based with the given detector.
	 We are trusting createDetectorView to pass in the correct type of detector, sub classes should trust AMDetector is actually their type. */
	bool setDetector(AMDetector *detector, bool configureOnly);

protected:
	/// The pointer to the detector
	CLSAmptekSDD123Detector *detector_;

	/// The status indicator
	QLabel *statusLabel_;
	/// The integration time indicator
	QDoubleSpinBox *integrationTimeDSB_;
	/// The detector temperature indicator
	QLabel *detectorTemperatureLabel_;
	/// The MCA Channel indicator
	QLabel *mcaChannelLabel_;
	/// The start acquisition button
	QPushButton *startAcquisitionButton_;

	/// This is the plot widget that holds the plot used for viewing the spectra.
	MPlotWidget *view_;
	/// This is the plot itself.
	MPlot *plot_;
	/// This holds the detector's spectrum data
	MPlotSeriesBasic *spectrumData_;
};

#endif // CLSAMPTEKSDD123DETECTORVIEW_H
