#ifndef BIOXASFRONTENDBEAMSTATUSVIEW_H
#define BIOXASFRONTENDBEAMSTATUSVIEW_H

#include <QWidget>
#include <QLayout>

class BioXASControlEditor;
class BioXASFrontEndBeamStatus;

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

signals:
	/// Notifier that the beam status being viewed has changed.
	void beamStatusChanged(BioXASFrontEndBeamStatus *newStatus);

public slots:
	/// Refreshes the view.
	void refresh();
	/// Sets the beam status being viewed.
	void setBeamStatus(BioXASFrontEndBeamStatus *newStatus);

protected slots:
	/// Updates the shutters editor.
	void updateShuttersEditor();
	/// Updates the valves editor.
	void updateValvesEditor();

protected:
	/// The beam status being viewed.
	BioXASFrontEndBeamStatus *beamStatus_;

	/// The beam status editor.
	BioXASControlEditor *beamStatusEditor_;
	/// The front-end shutters editor.
	BioXASControlEditor *shuttersEditor_;
	/// The beamline valves editor.
	BioXASControlEditor *valvesEditor_;
};

#endif // BIOXASFRONTENDBEAMSTATUSVIEW_H
