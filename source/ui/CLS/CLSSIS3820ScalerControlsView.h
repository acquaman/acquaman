#ifndef CLSSIS3820SCALERCONTROLSVIEW_H
#define CLSSIS3820SCALERCONTROLSVIEW_H

#include <QWidget>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QSpinBox>
#include <QLayout>

#include "beamline/CLS/CLSSIS3820Scaler.h"

#define MILLISECONDS_PER_SECOND 1000

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

public slots:
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

	/// Handles updating the start and stop buttons' enabled state and the scanning status LED, in response to the scaler's scanning status changed.
	void onScanningChanged();
	/// Handles updating the mode choice, in response to the scaler's continous mode status changed.
	void onContinuousChanged();
	/// Handles updating the dwell time, in response to the scaler's change in time.
	void onDwellTimeChanged();
	/// Handles updating the scans per buffer, in response to the scaler's change in scans per buffer.
	void onScansPerBufferChanged();
	/// Handles updating the total scans, in response to the scaler's change in total scans.
	void onTotalScansChanged();

protected:
	/// The scaler being viewed.
	CLSSIS3820Scaler *scaler_;

	/// Button that handles starting the scanning mode of the scaler.
	QPushButton *startButton_;
	/// Button that handles stopping the scanning mode of the scaler.
	QPushButton *stopButton_;
	/// Button that handles setting the mode of the scaler.
	QComboBox *modeChoice_;
	/// Label holding the overal scanning status of the scaler.  Matches the scanning button.
	QLabel *status_;
	/// Spin box holding the amount of time to per point.
	QSpinBox *time_;
	/// Spin box holding the number of scans per buffer.
	QSpinBox *scansPerBuffer_;
	/// Spin box holding the total number of scans.
	QSpinBox *totalScans_;

};

#endif // CLSSIS3820SCALERCONTROLSVIEW_H
