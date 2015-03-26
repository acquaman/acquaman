#ifndef SXRMBPERSISTENTVIEW_H
#define SXRMBPERSISTENTVIEW_H

#include <QWidget>

#include "application/SXRMB/SXRMB.h"

class QPushButton;
class QVBoxLayout;
class QGroupBox;
class QLabel;

class AMMotorGroupView;
class AMExtendedControlEditor;
class CLSSIS3820ScalerChannelView;
class AMAction3;

class SXRMBPersistentView : public QWidget
{

Q_OBJECT

public:
	/// Constructor, takes no arguments other than parent
	SXRMBPersistentView(QWidget *parent = 0);

	/// Destructor
	virtual ~SXRMBPersistentView();

protected slots:
	/// Handles turning beam on when button clicked
	void onBeamOnButtonClicked();
	/// Handles disconnecting signals and deleting when beam on action is done
	void onBeamOnActionFinished();

	/// Handles turning beam off when button clicked
	void onBeamOffButtonClicked();
	/// Handles disconnecting signals and deleting when beam off action is done
	void onBeamOffActionFinished();
	/// Handles SXRMBBeamline endStationChanged signal
	void onBeamlineEndStationChanged(SXRMB::Endstation endstation);


protected:
	/// Main layout holding overall group box
	QVBoxLayout *mainGroupBoxVL_;
	/// Main group box for the widget
	QGroupBox *mainGroupBox_;

	/// Main vertical layout inside of group box
	QVBoxLayout *mainVL_;

	/// Label to display the current endstation name
	QLabel *endStationLabel_;
	/// Control editor for the status (read only)
	AMExtendedControlEditor *statusControlEditor_;
	/// Button to turn on beam
	QPushButton *beamOnButton_;
	/// Button to turn off beam
	QPushButton *beamOffButton_;

	/// Control editor for the energy
	AMExtendedControlEditor *energyControlEditor_;

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

	/// layout the beamline status component and beam control buttons
	void layoutBeamlineStatus();
	/// layout the beamline Energy component
	void layoutBeamlineEnergy();
	/// layout the beamline motor view and the PVs
	void layoutMotorGroup();
	/// layout the beamline Scaler views
	void layoutScalers();
};

#endif // SXRMBPERSISTENTVIEW_H
