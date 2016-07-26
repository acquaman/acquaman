#ifndef SXRMBPERSISTENTVIEW_H
#define SXRMBPERSISTENTVIEW_H

#include <QWidget>

#include "application/SXRMB/SXRMB.h"

class QVBoxLayout;
class QLabel;

class AMAction3;
class AMControl;
class AMMotorGroupView;
class CLSSIS3820ScalerChannelView;

class SXRMBPersistentView : public QWidget
{

Q_OBJECT

public:
	/// Constructor, takes no arguments other than parent
	SXRMBPersistentView(QWidget *parent = 0);

	/// Destructor
	virtual ~SXRMBPersistentView();

signals:
	/// Notifier that the selected control in the beam status buttons view has changed.
	void beamlineStatusControlClicked(AMControl *control);

protected slots:
	/// Handles SXRMBBeamline endstationChanged signal
	void onBeamlineEndstationChanged(SXRMB::Endstation fromEndstation, SXRMB::Endstation toEndstation);

protected:
	/// layout the beamline status component and beam control buttons
	void layoutBeamlineStatus();
	/// layout the beamline Energy component
	void layoutBeamlineEnergy();
	/// layout the beamline motor view and the PVs
	void layoutMotorGroup();
	/// layout the beamline Scaler views
	void layoutScalers();
	/// layout the beamline HV Control views
	void layoutHVControls();

protected:
	/// Main vertical layout inside of group box
	QVBoxLayout *contentLayout_;

	/// Label to display the current endstation name
	QLabel *endstationLabel_;

	/// The view that holds all the motor group objects.
	AMMotorGroupView *motorGroupView_;

	/// View for the beamline I0 detector SR570.
	CLSSIS3820ScalerChannelView *beamlineI0DetectorSR570View_;
	/// View for the I0Detector SR570
	CLSSIS3820ScalerChannelView *i0DetectorSR570View_;
	/// View for the TEYDetector SR570
	CLSSIS3820ScalerChannelView *teyDetectorSR570View_;
	/// View for the transmission detector SR570.
	CLSSIS3820ScalerChannelView *transmissionDetectorSR570View_;

	/// Our copy of the beam on action
	AMAction3 *beamOnAction_;
	/// Our copy of the beam off action
	AMAction3 *beamOffAction_;
};

#endif // SXRMBPERSISTENTVIEW_H
