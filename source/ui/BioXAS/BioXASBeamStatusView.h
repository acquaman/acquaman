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

protected:
	/// Creates and returns a new control button.
	virtual QAbstractButton* createButton(AMControl *control);

protected:
	/// The beam status.
	BioXASBeamStatus *beamStatus_;

	/// The list of buttons.
	QList<QAbstractButton*> buttons_;
	/// The buttons layout.
	QHBoxLayout *buttonsLayout_;
};

#endif // BIOXASBEAMSTATUSVIEW_H
