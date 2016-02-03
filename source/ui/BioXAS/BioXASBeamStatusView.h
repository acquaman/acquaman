#ifndef BIOXASBEAMSTATUSVIEW_H
#define BIOXASBEAMSTATUSVIEW_H

#include <QWidget>
#include <QLayout>

class BioXASBeamStatus;
class BioXASControlEditor;
class BioXASBeamStatusBar;

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
	void beamStatusChanged(BioXASBeamStatus *newStatus);

public slots:
	/// Refreshes the view.
	void refresh();

	/// Sets the beam status being viewed.
	void setBeamStatus(BioXASBeamStatus *newStatus);

protected slots:
	/// Updates the beam status editor.
	void updateBeamStatusEditor();
	/// Updates the beam status bar.
	void updateBeamStatusBar();

protected:
	///  The beam status being viewed.
	BioXASBeamStatus *beamStatus_;

	/// The beam status editor.
	BioXASControlEditor *beamStatusEditor_;
	/// The components status bar.
	BioXASBeamStatusBar *statusBar_;
};

#endif // BIOXASBEAMSTATUSVIEW_H
