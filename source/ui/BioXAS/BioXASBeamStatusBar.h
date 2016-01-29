#ifndef BIOXASBEAMSTATUSBAR_H
#define BIOXASBEAMSTATUSBAR_H

#include "ui/BioXAS/BioXASButtonEditorBar.h"

class AMControlToolButton;
class BioXASControlEditor;
class BioXASBeamStatus;

class BioXASBeamStatusBar : public BioXASButtonEditorBar
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASBeamStatusBar(BioXASBeamStatus *beamStatus, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASBeamStatusBar();

	/// Returns the beam status being viewed.
	BioXASBeamStatus* beamStatus() const { return beamStatus_; }

signals:
	/// Notifier that the beam status being viewed has changed.
	void beamStatusChanged(BioXASBeamStatus *newStatus);

public slots:
	/// Refreshes the view.
	void refresh();

	/// Sets the beam status being viewed.
	void setBeamStatus(BioXASBeamStatus *newStatus);

protected slots:
	/// Updates the shutters button and editors.
	void updateShuttersViews();
	/// Updates the valves button and editor.
	void updateValvesViews();
	/// Updates the mirror button and editor.
	void updateMirrorViews();
	/// Updates the mono button and editor.
	void updateMonoViews();

protected:
	/// The beam status being viewed.
	BioXASBeamStatus *beamStatus_;

	/// The shutters button.
	AMControlToolButton *shuttersButton_;
	/// The front-end shutters editor.
	BioXASControlEditor *frontEndShuttersEditor_;
	/// The endstation shutter editor.
	BioXASControlEditor *endstationShutterEditor_;

	/// The valves button.
	AMControlToolButton *valvesButton_;
	/// The valves editor.
	BioXASControlEditor *valvesEditor_;

	/// The mono button.
	AMControlToolButton *monoButton_;
	/// The mono editor.
	BioXASControlEditor *monoEditor_;

	/// The mirror button.
	AMControlToolButton *mirrorButton_;
	/// The mirror editor.
	BioXASControlEditor *mirrorEditor_;
};

#endif // BIOXASBEAMSTATUSBAR_H
