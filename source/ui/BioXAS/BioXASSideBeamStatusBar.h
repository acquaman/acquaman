#ifndef BIOXASSIDEBEAMSTATUSBAR_H
#define BIOXASSIDEBEAMSTATUSBAR_H

#include "ui/BioXAS/BioXASBeamlineStatusBar.h"

class AMControlToolButton;
class BioXASControlEditor;
class BioXASSideBeamStatus;
class BioXASShuttersButton;
class BioXASMonochromatorButton;
class BioXASValvesButton;

class BioXASSideBeamStatusBar : public BioXASBeamlineStatusBar
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSideBeamStatusBar(BioXASSideBeamStatus *beamStatus, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASSideBeamStatusBar();

	/// Returns the beam status being viewed.
	BioXASSideBeamStatus* beamStatus() const { return beamStatus_; }

signals:
	/// Notifier that the beam status being viewed has changed.
	void beamStatusChanged(BioXASSideBeamStatus *newStatus);

public slots:
	/// Refreshes the view.
	void refresh();

	/// Sets the beam status being viewed.
	void setBeamStatus(BioXASSideBeamStatus *newStatus);

protected slots:
	/// Updates the front-end shutters button and editor.
	void updateFrontEndShuttersViews();
	/// Updates the valves button and editor.
	void updateValvesViews();
	/// Updates the mirror button and editor.
	void updateMirrorViews();
	/// Updates the mono button and editor.
	void updateMonoViews();
	/// Updates the endstation shutter button and editor.
	void updateEndstationShutterViews();

protected:
	/// The beam status being viewed.
	BioXASSideBeamStatus *beamStatus_;

	/// The front-end shutters button.
	AMControlToolButton *frontEndShuttersButton_;
	/// The front-end shutters editor.
	BioXASControlEditor *frontEndShuttersEditor_;

	/// The valves button.
	AMControlToolButton *valvesButton_;
	/// The valves editor.
	BioXASControlEditor *valvesEditor_;

	/// The mono button.
	BioXASMonochromatorButton *monoButton_;
	/// The mono editor.
	BioXASControlEditor *monoEditor_;

	/// The mirror button.
	AMControlToolButton *mirrorButton_;
	/// The mirror editor.
	BioXASControlEditor *mirrorEditor_;

	/// The endstation shutter button.
	AMControlToolButton *endstationShutterButton_;
	/// The endstation shutter editor.
	BioXASControlEditor *endstationShutterEditor_;
};

#endif // BIOXASSIDEBEAMSTATUSBAR_H
