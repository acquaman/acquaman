#ifndef BIOXASSIDEPOEBEAMSTATUSVIEW_H
#define BIOXASSIDEPOEBEAMSTATUSVIEW_H

#include <QWidget>
#include <QLayout>

class BioXASControlEditor;
class BioXASSidePOEBeamStatus;

class BioXASSidePOEBeamStatusView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSidePOEBeamStatusView(BioXASSidePOEBeamStatus *beamStatus, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASSidePOEBeamStatusView();

	/// Returns the beam status being viewed.
	BioXASSidePOEBeamStatus* beamStatus() const { return beamStatus_; }

signals:
	/// Notifier that the beam status being viewed has changed.
	void beamStatusChanged(BioXASSidePOEBeamStatus *newStatus);

public slots:
	/// Refreshes the view.
	void refresh();
	/// Sets the beam status being viewed.
	void setBeamStatus(BioXASSidePOEBeamStatus *newStatus);

protected slots:
	/// Updates the mirror mask editor.
	void updateMirrorMaskEditor();
	/// Updates the mono mask editor.
	void updateMonoMaskEditor();

protected:
	/// The beam status being viewed.
	BioXASSidePOEBeamStatus *beamStatus_;

	/// The beam status editor.
	BioXASControlEditor *beamStatusEditor_;
	/// The mirror mask editor.
	BioXASControlEditor *mirrorMaskEditor_;
	/// The mono mask editor.
	BioXASControlEditor *monoMaskEditor_;
};

#endif // BIOXASSIDEPOEBEAMSTATUSVIEW_H
