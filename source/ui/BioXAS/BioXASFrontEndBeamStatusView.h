#ifndef BIOXASFRONTENDBEAMSTATUSCONTROLVIEW_H
#define BIOXASFRONTENDBEAMSTATUSCONTROLVIEW_H

#include <QWidget>
#include <QLabel>
#include <QLayout>

#include "beamline/BioXAS/BioXASFrontEndBeamStatusControl.h"
#include "ui/beamline/AMExtendedControlEditor.h"
#include "ui/beamline/AMControlValueLEDView.h"
#include "ui/BioXAS/BioXASFrontEndBeamStatusControlLEDView.h"

class BioXASFrontEndBeamStatusControlView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASFrontEndBeamStatusControlView(BioXASFrontEndBeamStatusControl *beamStatus, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASFrontEndBeamStatusControlView();

	/// Returns the beam status control being viewed.
	BioXASFrontEndBeamStatusControl* control() const { return control_; }

	/// Returns the beam status control editor.
	AMExtendedControlEditor* controlEditor() const { return editor_; }

signals:
	/// Notifier that the beam status control being viewed has changed.
	void controlChanged(AMControl *newControl);

public slots:
	/// Clears the view.
	void clear();
	/// Updates the view with the latest beam status control information.
	void update();
	/// Refreshes the view.
	void refresh();

	/// Sets the beam status control being viewed.
	void setControl(BioXASFrontEndBeamStatusControl *newControl);

protected:
	/// The beam status being viewed.
	BioXASFrontEndBeamStatusControl *control_;

	/// The beam status control editor.
	AMExtendedControlEditor *editor_;
	/// The beam status control LED view.
	BioXASFrontEndBeamStatusControlLEDView *ledView_;
};

#endif // BIOXASFRONTENDBEAMSTATUSCONTROLVIEW_H
