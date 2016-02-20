#ifndef BIOXASBEAMSTATUSBAR_H
#define BIOXASBEAMSTATUSBAR_H

#include "ui/BioXAS/BioXASButtonEditorBar.h"

class AMControl;
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

protected:
	/// Creates and returns a button suitable for viewing the given control.
	QAbstractButton* createControlButton(AMControl *control) const;
	/// Creates and returns a view suitable for displaying information about the given control.
	QWidget* createControlView(AMControl *control) const;

protected:
	/// The beam status being viewed.
	BioXASBeamStatus *beamStatus_;

	/// The list of buttons.
	QList<QAbstractButton*> buttonsList_;
	/// The list of button views.
	QList<QWidget*> buttonViewsList_;
};

#endif // BIOXASBEAMSTATUSBAR_H
