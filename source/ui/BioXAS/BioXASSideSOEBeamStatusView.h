#ifndef BIOXASSIDESOEBEAMSTATUSVIEW_H
#define BIOXASSIDESOEBEAMSTATUSVIEW_H

#include <QWidget>
#include <QLayout>

class BioXASControlEditor;
class BioXASSideSOEBeamStatus;

class BioXASSideSOEBeamStatusView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSideSOEBeamStatusView(BioXASSideSOEBeamStatus *beamStatus, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASSideSOEBeamStatusView();

	/// Returns the beam status being viewed.
	BioXASSideSOEBeamStatus* beamStatus() const { return beamStatus_; }

signals:
	/// Notifier that the beam status being viewed has changed.
	void beamStatusChanged(BioXASSideSOEBeamStatus *newStatus);

public slots:
	/// Refreshes the view.
	void refresh();
	/// Sets the beam status being viewed.
	void setBeamStatus(BioXASSideSOEBeamStatus *newStatus);

protected slots:
	/// Updates the shutter editor.
	void updateShutterEditor();

protected:
	/// The beam status being viewed.
	BioXASSideSOEBeamStatus *beamStatus_;

	/// The beam status editor.
	BioXASControlEditor *beamStatusEditor_;
	/// The endstation shutter editor.
	BioXASControlEditor *shutterEditor_;
};

#endif // BIOXASSIDESOEBEAMSTATUSVIEW_H
