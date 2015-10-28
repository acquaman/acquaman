#ifndef BIOXASSIDEBEAMSTATUSCONTROLVIEW_H
#define BIOXASSIDEBEAMSTATUSCONTROLVIEW_H

#include <QWidget>

#include "beamline/BioXAS/BioXASSideBeamStatusControl.h"
#include "ui/BioXAS/BioXASFrontEndBeamStatusControlEditorsView.h"
#include "ui/BioXAS/BioXASSidePOEBeamStatusControlEditorsView.h"
#include "ui/BioXAS/BioXASSideSOEBeamStatusControlEditorsView.h"

class BioXASSideBeamStatusControlView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSideBeamStatusControlView(BioXASSideBeamStatusControl *control, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASSideBeamStatusControlView();

	/// Returns the control being viewed.
	BioXASSideBeamStatusControl* control() const { return control_; }

signals:
	/// Notifier that the control being viewed has changed.
	void controlChanged(AMControl *newControl);

public slots:
	/// Clears the view.
	void clear();
	/// Updates the view.
	void update();
	/// Refreshes the view.
	void refresh();

	/// Sets the control being viewed.
	void setControl(BioXASSideBeamStatusControl *newControl);

protected:
	/// The control being viewed.
	BioXASSideBeamStatusControl *control_;

	/// The beam status editor.
	AMExtendedControlEditor *beamStatusEditor_;
	/// The front-end control view.
	BioXASFrontEndBeamStatusControlEditorsView *frontEndView_;
	/// The POE control view.
	BioXASSidePOEBeamStatusControlEditorsView *poeView_;
	/// The SOE control view.
	BioXASSideSOEBeamStatusControlEditorsView *soeView_;
};

#endif // BIOXASSIDEBEAMSTATUSCONTROLVIEW_H
