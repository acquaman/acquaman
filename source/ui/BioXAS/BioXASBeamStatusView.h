#ifndef BIOXASBEAMSTATUSVIEW_H
#define BIOXASBEAMSTATUSVIEW_H

#include <QWidget>

#include "beamline/BioXAS/BioXASBeamStatus.h"
#include "ui/beamline/AMExtendedControlEditor.h"
#include "ui/BioXAS/BioXASFrontEndBeamStatusView.h"


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
	void beamStatusChanged(BioXASBeamStatus *beamStatus);

public slots:
	/// Clears the view.
	void clear();
	/// Updates the view.
	void update();
	/// Refreshes the view.
	void refresh();

	/// Sets the front-end beam status control.
	void setBeamStatus(BioXASBeamStatus *newStatus);

protected:
	/// The beam status being viewed.
	BioXASBeamStatus *beamStatus_;

	/// The front-end beam status view.
	BioXASFrontEndBeamStatusView *frontEndBeamStatusView_;
};

#endif // BIOXASBEAMSTATUSVIEW_H
