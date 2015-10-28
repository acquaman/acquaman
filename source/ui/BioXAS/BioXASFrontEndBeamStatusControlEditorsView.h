#ifndef BIOXASFRONTENDBEAMSTATUSCONTROLEDITORSVIEW_H
#define BIOXASFRONTENDBEAMSTATUSCONTROLEDITORSVIEW_H

#include "ui/beamline/AMExtendedControlEditor.h"
#include "ui/BioXAS/BioXASFrontEndBeamStatusControlView.h"

class BioXASFrontEndBeamStatusControlEditorsView : public BioXASFrontEndBeamStatusControlView
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASFrontEndBeamStatusControlEditorsView(BioXASFrontEndBeamStatusControl *control, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASFrontEndBeamStatusControlEditorsView();

	/// Returns the beam status editor.
	AMExtendedControlEditor* beamStatusEditor() const { return beamStatusEditor_; }
	/// Returns the shutters editor.
	AMExtendedControlEditor* shuttersEditor() const { return shuttersEditor_; }
	/// Returns the valves editor.
	AMExtendedControlEditor* valvesEditor() const { return valvesEditor_; }

public  slots:
	/// Clears the view.
	virtual void clear();
	/// Updates the view with the latest beam status control information.
	virtual void update();
	/// Refreshes the view.
	virtual void refresh();

protected:
	/// The front-end beam status editor.
	AMExtendedControlEditor *beamStatusEditor_;
	/// The shutters editor.
	AMExtendedControlEditor *shuttersEditor_;
	/// The valves editor.
	AMExtendedControlEditor *valvesEditor_;
};

#endif // BIOXASFRONTENDBEAMSTATUSCONTROLEDITORSVIEW_H
