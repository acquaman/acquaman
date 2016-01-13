#ifndef BIOXASSIDEBEAMSTATUSVIEW_H
#define BIOXASSIDEBEAMSTATUSVIEW_H

#include <QWidget>
#include <QLayout>

class BioXASSideBeamStatus;
class BioXASControlEditor;
class BioXASSideBeamStatusBar;

class BioXASSideBeamStatusView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSideBeamStatusView(BioXASSideBeamStatus *beamStatus, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASSideBeamStatusView();

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
	/// Updates the beam status editor.
	void updateBeamStatusEditor();
	/// Updates the beam status bar.
	void updateBeamStatusBar();

protected:
	///  The beam status being viewed.
	BioXASSideBeamStatus *beamStatus_;

	/// The beam status editor.
	BioXASControlEditor *beamStatusEditor_;
	/// The components status bar.
	BioXASSideBeamStatusBar *statusBar_;
};

#endif // BIOXASSIDEBEAMSTATUSVIEW_H
