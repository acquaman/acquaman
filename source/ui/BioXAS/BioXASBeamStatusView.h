#ifndef BIOXASBEAMSTATUSVIEW_H
#define BIOXASBEAMSTATUSVIEW_H

#include <QWidget>
#include <QLayout>

#include "beamline/BioXAS/BioXASBeamStatus.h"
#include "ui/beamline/AMControlToolButton.h"

class BioXASBeamStatusView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASBeamStatusView(BioXASBeamStatus *beamStatus, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASBeamStatusView();

	/// Returns the beam status being viewed.
	BioXASBeamStatus* beamStatus() const { return beamStatus_; }

signals:
	/// Notifier that the beam status being viewed has changed.
	void beamStatusChanged(BioXASBeamStatus *newBeamStatus);

public slots:
	/// Sets the beam status being viewed.
	void setBeamStatus(BioXASBeamStatus *newBeamStatus);

protected slots:
	/// Updates the whole beam status view.
	void updateBeamStatusView();
	/// Updates the wiggler button.
	void updateWigglerButton();
	/// Updates the shutter button.
	void updateShutterButton();
	/// Updates the valves button.
	void updateValvesButton();
	/// Updates the mirror mask button.
	void updateMirrorMaskButton();
	/// Updates the mono mask button.
	void updateMonoMaskButton();

protected:
	/// The beam status.
	BioXASBeamStatus *beamStatus_;

	/// The wiggler button.
	AMControlToolButton *wigglerButton_;
	/// The shutter button.
	AMControlToolButton *shutterButton_;
	/// The valves button.
	AMControlToolButton *valvesButton_;
	/// The mirror mask button.
	AMControlToolButton *mirrorMaskButton_;
	/// The mono mask button.
	AMControlToolButton *monoMaskButton_;
};

#endif // BIOXASBEAMSTATUSVIEW_H
