#ifndef AMDETECTORVIEW_H
#define AMDETECTORVIEW_H

#include <QWidget>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QCheckBox>
#include <QComboBox>

#include "beamline/AMDetector.h"

#include "MPlot/MPlot.h"
#include "MPlot/MPlotWidget.h"
#include "MPlot/MPlotSeries.h"
#include "MPlot/MPlotPoint.h"

class AMDetectorViewStatusView;
class QHBoxLayout;

class AMDetectorGeneralBriefView : public QWidget
{
Q_OBJECT
public:
	/// Default Constructor
 	virtual ~AMDetectorGeneralBriefView();
	AMDetectorGeneralBriefView(AMDetector *detector, QWidget *parent = 0);

	/// Returns a pointer to the detector in use
	AMDetector* detector() { return detector_; }

protected slots:
	/// Handles changes in the status
	void onAcquisitionStateChanged(AMDetector::AcqusitionState acquisitionState);
	/// Handles changes for the single reading by watching the AcquisitionSucceeded signal
	void onAcquisitionSucceeded();

protected:
	/// Holds the pointer to the detector we're viewing
	AMDetector *detector_;

	/// The status indicator
	QLabel *statusLabel_;
	/// The reading indicator
	QLabel *singleReadingLabel_;
};

class AMDetectorGeneralDetailedView : public QWidget
{
Q_OBJECT
public:
	/// Default constructor
 	virtual ~AMDetectorGeneralDetailedView();
	AMDetectorGeneralDetailedView(AMDetector *detector, QWidget *parent = 0);

	/// Returns a pointer to the detector in use
	AMDetector* detector() { return detector_; }
	/// Returns the current detector configuration
	AMDetectorInfo configurationSettings() const { return detector_->toInfo(); }

	/// Returns a pointer to the plot widget
	MPlot* plot() const { return plot_; }

protected slots:
	/// Handles changes in the connection status
	void onConnectedChanged(bool connected);
	/// Handles changes in the status
	void onAcquisitionStateChanged(AMDetector::AcqusitionState acquisitionState);
	/// Handles changes in the integration time
	void onAcquisitionTimeChanged(double integrationTime);
	/// Handles changes in the read mode
	void onReadModeChanged(AMDetectorDefinitions::ReadMode readMode);

	/// Handles button clicks from the start acqusition button
	void onStartAcquisitionButtonClicked();
	/// Handles changes to the integration time spin box
	void onAcquisitionTimeDSBEditingFinished();
	/// Handles changes to the read mode selector
	void onReadModeSelectedCurrentIndexChanged(const QString &indexString);

protected:
	/// Holds the pointer to the detector we're viewing
	AMDetector *detector_;
	/// Holds a pointer to the additional status view
	AMDetectorViewStatusView *statusView_;

	/// The status indicator
	QLabel *statusLabel_;
	/// The integration time indicator
	QDoubleSpinBox *acquisitionTimeDSB_;
	/// The start acquisition button
	QPushButton *startAcquisitionButton_;
	/// The singleReading indicator
	QDoubleSpinBox *singleReadingDSB_;
	/// The readMode indicator
	QLabel *readModeLabel_;
	/// The readMode selector
	QComboBox *readModeSelector_;

	QHBoxLayout *mainHL_;

	/// This is the plot widget that holds the plot used for viewing the spectra.
	MPlotWidget *view_;
	/// This is the plot itself.
	MPlot *plot_;
	/// This holds the detector's spectrum data
	MPlotSeriesBasic *spectrumData_;
};

class AMDetectorViewStatusView : public QWidget
{
Q_OBJECT
public:
 	virtual ~AMDetectorViewStatusView();
	AMDetectorViewStatusView(AMDetector *detector, QWidget *parent = 0);

protected:
	/// Holds the detector pointer
	AMDetector *detector_;

	/// Displays the connected status
	QLabel *connectedLabel_;
	/// Displays the read method
	QLabel *readMethodLabel_;
	/// Displays the read mode
	QLabel *readModeLabel_;
};

#endif // AMDETECTORVIEW_H
