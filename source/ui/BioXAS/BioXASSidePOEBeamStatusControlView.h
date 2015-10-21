#ifndef BIOXASSIDEPOEBEAMSTATUSCONTROLVIEW_H
#define BIOXASSIDEPOEBEAMSTATUSCONTROLVIEW_H

#include <QWidget>

#include "beamline/BioXAS/BioXASSidePOEBeamStatusControl.h"
#include "ui/beamline/AMExtendedControlEditor.h"
#include "ui/BioXAS/BioXASSidePOEBeamStatusControlLEDView.h"

class BioXASSidePOEBeamStatusControlView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSidePOEBeamStatusControlView(BioXASSidePOEBeamStatusControl *control, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASSidePOEBeamStatusControlView();

	/// Returns the beam status control being viewed.
	BioXASSidePOEBeamStatusControl* control() const { return control_; }

signals:
	/// Notifier that the beam status control being viewed has changed.
	void controlChanged(AMControl *newControl);

public slots:
	/// Clears the view.
	void clear();
	/// Updates the view.
	void update();
	/// Refreshes the view, clears and updates it.
	void refresh();

	/// Sets the beam status control being viewed.
	void setControl(BioXASSidePOEBeamStatusControl *newControl);

protected:
	/// The beam status control being viewed.
	BioXASSidePOEBeamStatusControl *control_;

	/// The beam status control editor.
	AMExtendedControlEditor *editor_;
	/// The beam status LED view.
	BioXASSidePOEBeamStatusControlLEDView *ledView_;
};

#endif // BIOXASSIDEPOEBEAMSTATUSCONTROLVIEW_H
