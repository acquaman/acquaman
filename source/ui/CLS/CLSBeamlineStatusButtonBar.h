#ifndef CLSBEAMLINESTATUSBUTTONBAR_H
#define CLSBEAMLINESTATUSBUTTONBAR_H

#include "ui/CLS/CLSControlButtonBar.h"

class CLSBeamlineStatus;

class CLSBeamlineStatusButtonBar : public CLSControlButtonBar
{
	Q_OBJECT

public:
	/// Constructor.
	CLSBeamlineStatusButtonBar(CLSBeamlineStatus *beamlineStatus, QWidget *parent = 0);
	/// Destructor.
	virtual ~CLSBeamlineStatusButtonBar();

	/// Returns the beam status being viewed.
	CLSBeamlineStatus* beamStatus() const { return beamlineStatus_; }

signals:
	/// Notifier that the beam status being viewed has changed.
	void beamlineStatusComponentChanged(CLSBeamlineStatus *newControl);

public slots:
	/// Refreshes the view.
	void refresh();
	/// Sets the beam status.
	void setBeamlineStatusComponent(CLSBeamlineStatus *newControl);

protected:
	/// Creates and returns a button for the given control.
	virtual QAbstractButton* createButton(AMControl *control, double greenValue);

protected:
	/// The beam status being viewed.
	CLSBeamlineStatus *beamlineStatus_;
};

#endif // CLSBEAMLINESTATUSBUTTONBAR_H
