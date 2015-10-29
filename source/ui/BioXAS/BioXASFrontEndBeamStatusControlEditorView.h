#ifndef BIOXASFRONTENDBEAMSTATUSCONTROLEDITORVIEW_H
#define BIOXASFRONTENDBEAMSTATUSCONTROLEDITORVIEW_H

#include <QWidget>

#include "ui/beamline/AMExtendedControlEditor.h"
#include "ui/BioXAS/BioXASFrontEndBeamStatusControlView.h"
#include "ui/BioXAS/BioXASFrontEndBeamStatusControlLEDView.h"

class BioXASFrontEndBeamStatusControlEditorView : public BioXASFrontEndBeamStatusControlView
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASFrontEndBeamStatusControlEditorView(BioXASFrontEndBeamStatusControl *control, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASFrontEndBeamStatusControlEditorView();

	/// Returns the beam status control editor.
	AMExtendedControlEditor* editor() const { return editor_; }

public slots:
	/// Clears the view.
	virtual void clear();
	/// Updates the view with the latest beam status control information.
	virtual void update();

protected:
	/// The beam status control editor.
	AMExtendedControlEditor *editor_;
	/// The beam status control LED view.
	BioXASFrontEndBeamStatusControlLEDView *ledView_;
};

#endif // BIOXASFRONTENDBEAMSTATUSCONTROLEDITORVIEW_H
