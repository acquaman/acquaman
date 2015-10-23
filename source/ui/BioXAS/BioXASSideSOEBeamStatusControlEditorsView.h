#ifndef BIOXASSIDESOEBEAMSTATUSCONTROLEDITORSVIEW_H
#define BIOXASSIDESOEBEAMSTATUSCONTROLEDITORSVIEW_H

#include <QLayout>

#include "ui/BioXAS/BioXASSideSOEBeamStatusControlView.h"
#include "ui/beamline/AMExtendedControlEditor.h"

class BioXASSideSOEBeamStatusControlEditorsView : public BioXASSideSOEBeamStatusControlView
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSideSOEBeamStatusControlEditorsView(BioXASSideSOEBeamStatusControl *control, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASSideSOEBeamStatusControlEditorsView();

	/// Returns the beam status editor.
	AMExtendedControlEditor* beamStatusEditor() const { return beamStatusEditor_; }
	/// Returns the shutter editor.
	AMExtendedControlEditor* shutterEditor() const { return shutterEditor_; }

public slots:
	/// Clears the view.
	virtual void clear();
	/// Updates the view.
	virtual void update();
	/// Refreshes the view, clears and updates it.
	virtual void refresh();

protected:
	/// The beam status editor.
	AMExtendedControlEditor *beamStatusEditor_;
	/// The endstation shutter editor.
	AMExtendedControlEditor *shutterEditor_;
};

#endif // BIOXASSIDESOEBEAMSTATUSCONTROLEDITORSVIEW_H
