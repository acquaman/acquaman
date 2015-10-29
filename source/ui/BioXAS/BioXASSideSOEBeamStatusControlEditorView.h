#ifndef BIOXASSIDESOEBEAMSTATUSCONTROLEDITORVIEW_H
#define BIOXASSIDESOEBEAMSTATUSCONTROLEDITORVIEW_H

#include "ui/beamline/AMExtendedControlEditor.h"
#include "ui/beamline/AMControlValueLEDView.h"
#include "ui/BioXAS/BioXASSideSOEBeamStatusControlView.h"

class BioXASSideSOEBeamStatusControlEditorView : public BioXASSideSOEBeamStatusControlView
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSideSOEBeamStatusControlEditorView(BioXASSideSOEBeamStatusControl *control, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASSideSOEBeamStatusControlEditorView();

	/// Returns the beam status editor.
	AMExtendedControlEditor* editor() const { return editor_; }

public slots:
	/// Clears the view.
	virtual void clear();
	/// Updates the view.
	virtual void update();

protected:
	/// The beam status control editor.
	AMExtendedControlEditor *editor_;
	/// The beam status LED view.
	AMControlValueLEDView *ledView_;
};

#endif // BIOXASSIDESOEBEAMSTATUSCONTROLEDITORVIEW_H
