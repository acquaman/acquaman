#ifndef CLSSIS3820SCALERCONTROLSVIEW_H
#define CLSSIS3820SCALERCONTROLSVIEW_H

#include <QWidget>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QLayout>

#include "beamline/CLS/CLSSIS3820Scaler.h"

class CLSSIS3820ScalerControlsView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit CLSSIS3820ScalerControlsView(CLSSIS3820Scaler *scaler, QWidget *parent = 0);
	/// Destructor.
	virtual ~CLSSIS3820ScalerControlsView();
	/// Returns the scaler being viewed.
	CLSSIS3820Scaler* scaler() const { return scaler_; }

signals:
	/// Notifier that the scaler being viewed has changed.
	void scalerChanged(CLSSIS3820Scaler *newScaler);

public slots:
	/// Refreshes the view.
	void refresh();
	/// Sets the scaler being viewed.
	void setScaler(CLSSIS3820Scaler *newScaler);

protected slots:
	/// Starts a scan.
	void startScanning();
	/// Stops a scan.
	void stopScanning();
	/// Sets the scaler scan mode, using the view input.
	void setContinuous();
	/// Sets the scaler dwell time, using the view input.
	void setDwellTime();
	/// Sets the scaler's number of scans per buffer, using the view input.
	void setScansPerBuffer();
	/// Sets the scaler's total number of scans, using the view input.
	void setTotalScans();

	/// Updates the start button.
	void updateStartButton();
	/// Updates the stop button.
	void updateStopButton();
	/// Updates the status label.
	void updateStatusLabel();
	/// Updates the acquisition mode box with the scaler's current mode.
	void updateAcquisitionModeBox();
	/// Handles updating the dwell time box with the scaler's current dwell time.
	void updateDwellTimeBox();
	/// Handles updating the scans per buffer, in response to the scaler's change in scans per buffer.
	void updateScansPerBufferBox();
	/// Handles updating the total scans, in response to the scaler's change in total scans.
	void updateTotalScansBox();

	/// Handles updating the start and stop buttons and the status label when the scaler scanning state changes.
	void onScalerScanningStateChanged();
	/// Handles updating the acquisition mode box, the dwell time box, the scans per buffer box, and the total number of scans box when the scaler continuous state changes.
	void onScalerContinuousStateChanged();

protected:
	/// The scaler being viewed.
	CLSSIS3820Scaler *scaler_;

	/// Button that handles starting the scanning mode of the scaler.
	QPushButton *startButton_;
	/// Button that handles stopping the scanning mode of the scaler.
	QPushButton *stopButton_;
	/// Button that handles setting the mode of the scaler.
	QComboBox *acquisitionModeBox_;
	/// Label holding the overall scanning status of the scaler.  Matches the scanning button.
	QLabel *statusLabel_;
	/// Spin box holding the dwell time per point, in milliseconds.
	QDoubleSpinBox *dwellTimeBox_;
	/// Spin box holding the number of scans per buffer.
	QSpinBox *scansPerBufferBox_;
	/// Spin box holding the total number of scans.
	QSpinBox *totalScansBox_;
};

#endif // CLSSIS3820SCALERCONTROLSVIEW_H
