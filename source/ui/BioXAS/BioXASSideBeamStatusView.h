#ifndef BIOXASSIDEBEAMSTATUSVIEW_H
#define BIOXASSIDEBEAMSTATUSVIEW_H

#include <QWidget>
#include <QLayout>

class BioXASSideBeamStatus;
class BioXASFrontEndBeamStatusView;
class BioXASSidePOEBeamStatusView;
class BioXASSideSOEBeamStatusView;
class BioXASControlEditor;

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
	/// Updates the front-end beam status view.
	void updateFrontEndView();
	/// Updates the Side POE beam status view.
	void updateSidePOEView();
	/// Updates the Side SOE beam status view.
	void updateSideSOEView();

protected:
	///  The beam status being viewed.
	BioXASSideBeamStatus *beamStatus_;

	/// The beam status editor.
	BioXASControlEditor *beamStatusEditor_;
	/// The front-end beam status view.
	BioXASFrontEndBeamStatusView *frontEndView_;
	/// The Side POE beam status view.
	BioXASSidePOEBeamStatusView *sidePOEView_;
	/// The Side SOE beam status view.
	BioXASSideSOEBeamStatusView *sideSOEView_;
};

#endif // BIOXASSIDEBEAMSTATUSVIEW_H
