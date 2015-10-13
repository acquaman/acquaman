#ifndef BIOXASFRONTENDBEAMSTATUSVIEW_H
#define BIOXASFRONTENDBEAMSTATUSVIEW_H

#include <QWidget>
#include <QLabel>
#include <QLayout>

#include "beamline/BioXAS/BioXASFrontEndBeamStatusControl.h"
#include "ui/beamline/AMExtendedControlEditor.h"

class BioXASFrontEndBeamStatusView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASFrontEndBeamStatusView(BioXASFrontEndBeamStatusControl *beamStatus, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASFrontEndBeamStatusView();

	/// Returns the beam status control being viewed.
	BioXASFrontEndBeamStatusControl* beamStatusControl() const { return beamStatus_; }

signals:
	/// Notifier that the beam status control being viewed has changed.
	void beamStatusControlChanged(BioXASFrontEndBeamStatusControl *newControl);

public slots:
	/// Clears the view.
	void clear();
	/// Updates the view with the latest beam status control information.
	void update();
	/// Refreshes the view.
	void refresh();

	/// Sets the beam status control being viewed.
	void setBeamStatusControl(BioXASFrontEndBeamStatusControl *newControl);
	/// Sets the beam status editor's minimum width.
	void setBeamStatusEditorMinimumWidth(int newWidth);
	/// Sets the beam status editor's title.
	void setBeamStatusEditorTitle(const QString &newTitle);

protected slots:
	/// Updates the upstream photon shutter LED.
	void updatePhotonShutterUpstreamLED();
	/// Updates the vacuum valve LED.
	void updateVacuumValveLED();
	/// Updates the fast valve LED.
	void updateFastValveLED();
	/// Updates the downstream photon shutter LED.
	void updatePhotonShutterDownstreamLED();
	/// Updates the safety shutter LED.
	void updateSafetyShutterLED();
	/// Updates the beamline valves LED.
	void updateBeamlineValvesLED();

	/// Updates the given LED using the given control's information.
	void updateLED(QLabel *led, AMControl *control);
	/// Updates the given LED using the given bi-state control's information.
	void updateBiStateControlLED(QLabel *led, CLSBiStateControl *control);
	/// Updates the given LED using the given BioXAS-specific beamline valves control's information.
	void updateBioXASBeamlineValvesLED(QLabel *led, BioXASValvesControl *control);

protected:
	/// The beam status control being viewed.
	BioXASFrontEndBeamStatusControl *beamStatus_;

	/// The beam status control editor.
	AMExtendedControlEditor *beamStatusEditor_;
	/// The upstream photon shutter LED.
	QLabel *photonShutterUpstreamLED_;
	/// The vacuum valve LED.
	QLabel *vacuumValveLED_;
	/// The fast valve LED.
	QLabel *fastValveLED_;
	/// The downstream photon shutter LED.
	QLabel *photonShutterDownstreamLED_;
	/// The safety shutter LED.
	QLabel *safetyShutterLED_;
	/// The beamline valves LED.
	QLabel *beamlineValvesLED_;
};

#endif // BIOXASFRONTENDBEAMSTATUSVIEW_H
