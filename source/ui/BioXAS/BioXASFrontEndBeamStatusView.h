#ifndef BIOXASFRONTENDBEAMSTATUSVIEW_H
#define BIOXASFRONTENDBEAMSTATUSVIEW_H

#include <QWidget>
#include <QLabel>
#include <QLayout>

#include "beamline/BioXAS/BioXASFrontEndBeamStatus.h"
#include "ui/beamline/AMExtendedControlEditor.h"
#include "ui/beamline/AMControlValueLEDView.h"

class BioXASFrontEndBeamStatusView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASFrontEndBeamStatusView(BioXASFrontEndBeamStatus *beamStatus, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASFrontEndBeamStatusView();

	/// Returns the beam status being viewed.
	BioXASFrontEndBeamStatus* beamStatus() const { return beamStatus_; }

	/// Returns the beam status editor.
	AMExtendedControlEditor* beamStatusEditor() const { return beamStatusEditor_; }

signals:
	/// Notifier that the beam status being viewed has changed.
	void beamStatusControlChanged(BioXASFrontEndBeamStatus *newControl);

public slots:
	/// Clears the view.
	void clear();
	/// Updates the view with the latest beam status control information.
	void update();
	/// Refreshes the view.
	void refresh();

	/// Sets the beam status control being viewed.
	void setBeamStatus(BioXASFrontEndBeamStatus *newControl);

protected:
	/// The beam status being viewed.
	BioXASFrontEndBeamStatus *beamStatus_;

	/// The beam status control editor.
	AMExtendedControlEditor *beamStatusEditor_;
	/// The upstream photon shutter LED.
	AMControlValueLEDView *photonShutterUpstreamLED_;
	/// The downstream photon shutter LED.
	AMControlValueLEDView *photonShutterDownstreamLED_;
	/// The safety shutter LED.
	AMControlValueLEDView *safetyShutterLED_;
	/// The beamline valves LED.
	AMControlValueLEDView *beamlineValvesLED_;
};

#endif // BIOXASFRONTENDBEAMSTATUSVIEW_H
