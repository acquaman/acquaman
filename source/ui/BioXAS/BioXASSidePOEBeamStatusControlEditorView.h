#ifndef BIOXASSIDEPOEBEAMSTATUSCONTROLEDITORVIEW_H
#define BIOXASSIDEPOEBEAMSTATUSCONTROLEDITORVIEW_H

#include "ui/BioXAS/BioXASSidePOEBeamStatusControlView.h"
#include "ui/beamline/AMExtendedControlEditor.h"
#include "ui/BioXAS/BioXASSidePOEBeamStatusControlLEDView.h"

class BioXASSidePOEBeamStatusControlEditorView : public BioXASSidePOEBeamStatusControlView
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSidePOEBeamStatusControlEditorView(BioXASSidePOEBeamStatusControl *control, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASSidePOEBeamStatusControlEditorView();

	/// Returns the beam status editor.
	AMExtendedControlEditor* editor() const { return editor_; }

public slots:
	/// Clears the view.
	void clear();
	/// Updates the view.
	void update();

protected:
	/// The beam status control editor.
	AMExtendedControlEditor *editor_;
	/// The beam status LED view.
	BioXASSidePOEBeamStatusControlLEDView *ledView_;
};

#endif // BIOXASSIDEPOEBEAMSTATUSCONTROLEDITORVIEW_H
